#include <core/lock.h>
#include <core/vector.h>

#include "../arch/asm.h"
#include "../arch/cpu.h"
#include "../lib/log.h"
#include "sched.h"
#include "syscall.h"

static core::SpinLock scheduler_lock;
static core::Vector<Thread *> scheduler_queue;

static Process *kernel_process;

static void kernel_idle_thread() {
    while (true) {
        arch::halt();
    }
}

void sched::init() {
    kernel_process = Process::spawn(nullptr, "kernel", (uint64_t) kernel_idle_thread, false);
}

void sched::queue(Thread *thread) {
    core::LockGuard lock(scheduler_lock);

    if (thread->registers->rip == (uint64_t) kernel_idle_thread) // Do not queue kernel idle thread for scheduling
        return;

    scheduler_queue.push(thread);
}

void sched::dequeue(Thread *thread) {
    core::LockGuard lock(scheduler_lock);

    scheduler_queue.remove(thread);
}

void sched::reschedule(Registers *regs) {
    core::LockGuard lock(scheduler_lock);

    auto current_cpu = arch::get_current_cpu();
    auto current_thread = current_cpu->current_thread;

    if (current_thread && !current_thread->is_running) {
        if (current_thread->process->main_thread == current_thread) {
            current_thread->process->exit_code = current_thread->exit_code;
            current_thread->process->is_running = false;
        }

        current_thread->process->threads.remove(current_thread->id);

        delete current_thread;

        current_cpu->current_thread = nullptr;
        current_thread = nullptr;
    }

    if (scheduler_queue.size() > 0) {
        auto next_thread = scheduler_queue[0];

        scheduler_queue.remove(next_thread);

        if (current_thread) {
            __builtin_memcpy(current_thread->registers, regs, sizeof(Registers));

            scheduler_queue.push(current_thread);
        }

        __builtin_memcpy(regs, next_thread->registers, sizeof(Registers));

        syscall::set_gs_base((uint64_t) next_thread);

        vmm::switch_to(next_thread->process->pml4);

        current_cpu->current_thread = next_thread;
    } else {
        if (current_thread)
            return;

        auto idle_thread = kernel_process->main_thread;

        __builtin_memcpy(regs, idle_thread->registers, sizeof(Registers));

        syscall::set_gs_base((uint64_t) idle_thread);

        vmm::switch_to(kernel_process->pml4);

        current_cpu->current_thread = idle_thread;
    }
}

Thread *sched::get_current_thread() {
    auto current_cpu = arch::get_current_cpu();

    return current_cpu->current_thread;
}
