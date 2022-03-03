#include <string.h>

#include <proc/cpu.h>
#include <proc/sched.h>
#include <utils/print.h>
#include <utils/spin.h>
#include <utils/vector.h>

static utils::spin_lock_t scheduler_lock;
static utils::vector_t<proc::thread_t *> thread_queue;

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

void scheduler::schedule(proc::thread_t *thread) {
  utils::spin_lock_guard_t lock(scheduler_lock);

  thread_queue.push(thread);
}

void scheduler::preempt([[maybe_unused]] interrupts::isr_frame_t *frame) {
  utils::spin_lock_guard_t lock(scheduler_lock);

  auto cpu = cpu::get();

  if (thread_queue.size() == 0)
    return;

  if (cpu->thread)
    thread_queue.push(cpu->thread);

  auto next_thread = thread_queue[0];

  thread_queue.remove(next_thread);

  if (cpu->thread)
    memcpy(&cpu->thread->regs, frame, sizeof(interrupts::isr_frame_t));

  memcpy(frame, &next_thread->regs, sizeof(interrupts::isr_frame_t));

  cpu->thread = next_thread;
  cpu->thread->vm->switch_to();
}
