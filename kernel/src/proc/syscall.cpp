#include "syscall.h"
#include "../arch/asm.h"
#include "../arch/gdt.h"
#include "../lib/log.h"
#include "sched.h"

extern "C" void syscall_entry();

void syscall::init() {
    // Enable syscall in EFER
    arch::write_msr(MSR_EFER, arch::read_msr(MSR_EFER) | 1 /* EFER.SCE */);

    // Set up syscall handler
    arch::write_msr(MSR_STAR, ((uint64_t) GDT_KERNEL_CS64 << 32) | ((uint64_t) (GDT_USER_BASE | 3 /* CPL3 */) << 48));
    arch::write_msr(MSR_LSTAR, (uint64_t) syscall_entry);
    arch::write_msr(MSR_SYSCALL_FLAG_MASK, 0x200);
}

void syscall::set_gs_base(uint64_t user_gs) {
    arch::write_msr(MSR_GS_BASE, user_gs);
    arch::write_msr(MSR_KERN_GS_BASE, user_gs /* TODO: Fix this */);
}

extern "C" void syscall_handler(registers_t *regs) {
    if (regs->rax == 0) { // Exit syscall
        task::get_current_task()->kill(regs->rbx);
    } else if (regs->rax == 1) { // Trace syscall
        log_info("{}", (const char *) regs->rbx);
    } else {
        // Unknown syscall
        log_error("Unknown syscall {}", regs->rax);
    }
}
