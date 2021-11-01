#include "intr.h"
#include "../arch/asm.h"
#include "../arch/cpu.h"
#include "../arch/gdt.h"
#include "../lib/log.h"
#include "../sched/sched.h"
#include "apic.h"

static constexpr const char *exception_messages[] = {
    "Divide-by-zero error",
    "Debug exception",
    "Non-maskable interrupt",
    "Breakpoint exception",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 floating-point error",
    "Alignment check",
    "Machine check",
    "SIMD floating-point error",
    "Virtualization exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

InterruptGuard::InterruptGuard() {
    intr::retain();
}

InterruptGuard::~InterruptGuard() {
    intr::release();
}

void intr::retain_disable() {
    auto current_cpu = arch::get_current_cpu();

    assert_msg(current_cpu->retain_enable, "retain_disable() called when interrupt retaining was not enabled");

    if (current_cpu->retain_depth > 0)
        log_warn("retain_disable() called when interrupts were retained (retain depth is {})", current_cpu->retain_depth);

    current_cpu->retain_enable = false;
}

void intr::retain_enable() {
    auto current_cpu = arch::get_current_cpu();

    assert_msg(!current_cpu->retain_enable, "retain_enable() called when interrupt retaining was already enabled");

    current_cpu->retain_enable = true;
}

void intr::retain() {
    auto current_cpu = arch::get_current_cpu();

    if (!current_cpu->retain_enable)
        return;

    arch::disable_interrupts();

    current_cpu->retain_depth++;
}

void intr::release() {
    auto current_cpu = arch::get_current_cpu();

    if (!current_cpu->retain_enable)
        return;

    assert_msg(current_cpu->retain_depth > 0, "release() called when interrupts were not retained");

    current_cpu->retain_depth--;

    if (current_cpu->retain_depth == 0)
        arch::enable_interrupts();
}

void error_print_registers(Registers *regs) {
    uint64_t cr2, cr3;

    asm("mov %%cr2, %0\n"
        "mov %%cr3, %1"
        : "=r"(cr2), "=r"(cr3));

    log_info_unlocked("Register dump:");
    log_info_unlocked("- CR2: {#016x} | CR3: {#016x}", cr2, cr3);
    log_info_unlocked("- RIP: {#016x} | RSP: {#016x}", regs->rip, regs->rsp);
    log_info_unlocked("- RAX: {#016x} | RBX: {#016x} | RCX: {#016x}", regs->rax, regs->rbx, regs->rcx);
    log_info_unlocked("- RDX: {#016x} | RSI: {#016x} | RDI: {#016x}", regs->rdx, regs->rsi, regs->rdi);
    log_info_unlocked("- RBP: {#016x} | R8:  {#016x} | R9:  {#016x}", regs->rbp, regs->r8, regs->r9);
    log_info_unlocked("- R10: {#016x} | R11: {#016x} | R12: {#016x}", regs->r10, regs->r11, regs->r12);
    log_info_unlocked("- R13: {#016x} | R14: {#016x} | R15: {#016x}", regs->r13, regs->r14, regs->r15);
}

void interrupt_error_handler(Registers *regs) {
    log_error_unlocked("An unexpected CPU exception occurred{}!", (regs->cs & 3) ? " in user mode" : "");
    log_error_unlocked("- Exception: {} ({})", exception_messages[regs->isr_number], regs->isr_number);
    log_error_unlocked("- Error code: {#016x}", regs->err_code);

    if (regs->isr_number == 14) {
        uint64_t cr2;

        asm("mov %%cr2, %0" : "=r"(cr2));

        log_error_unlocked("- Faulting address: {#016x}", cr2);

        if (!(regs->err_code & 0x1))
            log_error_unlocked("- Faulting address is contained in a non-present page");

        if (regs->err_code & 0x10)
            log_error_unlocked("- Fault caused by an instruction fetch");
        else if (regs->err_code & 0x2)
            log_error_unlocked("- Fault caused by a write");
        else
            log_error_unlocked("- Fault caused by a read");
    }

    error_print_registers(regs);

    while (true) {
        arch::halt_forever();
    }
}

extern "C" void interrupt_handler(Registers *regs) {
    intr::retain_disable();

    if (regs->isr_number < 32) {
        interrupt_error_handler(regs);
    } else {
        if (regs->isr_number == 32) {
            sched::reschedule(regs);
        } else {
            log_info_unlocked("Received an interrupt request #{}", regs->isr_number);
        }

        apic::send_eoi();
    }

    intr::retain_enable();
}
