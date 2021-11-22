#include <system/syscalls.h>

#include "../arch/asm.h"
#include "../arch/cpu.h"
#include "../arch/gdt.h"
#include "../lib/log.h"
#include "sched.h"
#include "syscall.h"

extern "C" void syscall_entry();

extern void error_print_registers(Registers *regs);

void syscall::init() {
    // Enable syscall in EFER
    Msr::efer().write(Msr::efer().read() | 0x01 /* EFER.SCE */);

    // Set up syscall handler
    Msr::star().write(((uint64_t) GDT_KERNEL_CS64 << 32) | ((uint64_t) (GDT_USER_BASE | 3 /* CPL3 */) << 48));
    Msr::lstar().write((uint64_t) syscall_entry);
    Msr::syscall_flag_mask().write(0x200);
}

void syscall::set_gs_base(uint64_t user_gs) {
    Msr::gs_base().write(user_gs);
    Msr::kernel_gs_base().write(user_gs);
}

extern "C" uint64_t syscall_handler(Registers *regs) {
    // RDI - syscall ID
    // RBX - first argument
    // RDX - second argument
    // RSI - third argument
    // R8  - fourth argument
    // R9  - fifth argument

        switch (regs->rdi) {
            case SYSCALL_TRACE:
                log_info("{}", (const char *) regs->rbx);
                break;

            case SYSCALL_EXIT:
                // TODO: Implement
                // sched::get_current_thread()->kill(regs->rbx);
                break;

            case SYSCALL_YIELD:
                sched::reschedule(regs);
                break;

            case SYSCALL_OPEN:
                return vfs::open("/", (const char *) regs->rbx);
                break;

            case SYSCALL_CLOSE:
                return vfs::close(regs->rbx);
                break;

            case SYSCALL_READ:
                return vfs::read(regs->rbx, (uint8_t *) regs->rdx, regs->rsi);
                break;

            case SYSCALL_WRITE:
                return vfs::write(regs->rbx, (const uint8_t *) regs->rdx, regs->rsi);
                break;

            default: log_error("Unknown syscall with ID {#016x}", regs->rdi);
            return -1;
    }

    return 0;
}
