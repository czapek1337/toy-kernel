#include <string.h>

#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>
#include <frg/vector.hpp>

#include <proc/cpu.h>
#include <proc/sched.h>
#include <utils/print.h>

static frg::ticket_spinlock scheduler_lock;
static frg::vector<smarter::shared_ptr<proc::thread_t>, mem::kernel_allocator_t> thread_queue(mem::kernel_allocator);

// TODO: Move this to a separate file?
static void sched_syscall_handler(interrupts::isr_frame_t *frame) {
  klog_debug("System call %x with params: (%x, %x, %x, %x, %x, %x)", //
             frame->rax, frame->rdi, frame->rsi, frame->rdx, frame->rcx, frame->r8, frame->r9);
}

static void sched_idle_thread() {
  while (1) {
    klog_debug("System is currently idle");

    asm("hlt");
  }
}

void scheduler::init() {
  interrupts::register_handler(IRQ_SYSCALL, sched_syscall_handler);

  schedule(proc::create_thread((uintptr_t) sched_idle_thread, false));
}

void scheduler::schedule(smarter::shared_ptr<proc::thread_t> thread) {
  frg::unique_lock lock(scheduler_lock);

  thread_queue.push(thread);
}

// TODO: Make this an actual scheduler? lol
//
// Currently because of the switch to using frigg/libsmarter I had to get rid
// of the every so primitive scheduler because I haven't figured out a way
// to make it work with frigg, soon it will turn into a fully working on tho :^)
void scheduler::preempt(interrupts::isr_frame_t *frame) {
  auto cpu = cpu::get();
  auto thread = thread_queue.front();

  thread->vm->switch_to();

  if (cpu->thread)
    memcpy(&cpu->thread->regs, frame, sizeof(interrupts::isr_frame_t));

  cpu->thread = thread;

  memcpy(frame, &thread->regs, sizeof(interrupts::isr_frame_t));
}
