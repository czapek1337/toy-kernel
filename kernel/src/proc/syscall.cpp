#include <system/syscalls.h>

#include "../arch/asm.h"
#include "../arch/cpu.h"
#include "../arch/gdt.h"
#include "../lib/log.h"
#include "sched.h"
#include "syscall.h"

extern "C" void syscall_entry();

extern void error_print_registers(registers_t *regs);

void syscall::init() {
    // Enable syscall in EFER
    arch::write_msr(MSR_EFER, arch::read_msr(MSR_EFER) | 0x01 /* EFER.SCE */);

    // Set up syscall handler
    arch::write_msr(MSR_STAR, ((uint64_t) GDT_KERNEL_CS64 << 32) | ((uint64_t) (GDT_USER_BASE | 3 /* CPL3 */) << 48));
    arch::write_msr(MSR_LSTAR, (uint64_t) syscall_entry);
    arch::write_msr(MSR_SYSCALL_FLAG_MASK, 0x200);
}

void syscall::set_gs_base(uint64_t user_gs) {
    arch::write_msr(MSR_GS_BASE, user_gs);
    arch::write_msr(MSR_KERN_GS_BASE, user_gs /* TODO: Fix this */);
}

extern "C" uint64_t syscall_handler(registers_t *regs) {
    // RDI - syscall ID
    // RBX - first argument
    // RDX - second argument
    // RSI - third argument
    // R8  - fourth argument
    // R9  - fifth argument

    if (regs->rdi == SYSCALL_TRACE) {
        log_info("{}", (const char *) regs->rbx);
    } else if (regs->rdi == SYSCALL_EXIT) {
        task::get_current_task()->kill(regs->rbx);
    } else if (regs->rdi == SYSCALL_YIELD) {
        task::reschedule(regs);
    } else {
        log_error("Unknown syscall with ID {#016x}", regs->rdi);
    }

    return 0;
}
