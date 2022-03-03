#include <string.h>

#include <proc/cpu.h>
#include <proc/sched.h>
#include <utils/print.h>

// TODO: Move this to a separate file?
static void sched_syscall_handler(interrupts::isr_frame_t *frame) {
  klog_debug("System call %x with params: (%x, %x, %x, %x, %x, %x)", //
             frame->rax, frame->rdi, frame->rsi, frame->rdx, frame->rcx, frame->r8, frame->r9);
}

static void sched_idle_thread() {
  while (1) {
    asm("hlt");
  }
}

void scheduler::init() {
  interrupts::register_handler(IRQ_SYSCALL, sched_syscall_handler);

  schedule(proc::create_thread((vaddr_t) sched_idle_thread, false));
}

// TODO: Rewrite the rest of this in C++ so it makes sense

void scheduler::schedule([[maybe_unused]] proc::thread_t *thread) {
  klog_panic("TODO: Implement scheduler::schedule");
}

void scheduler::preempt([[maybe_unused]] interrupts::isr_frame_t *frame) {
  klog_panic("TODO: Implement scheduler::preempt");
}
