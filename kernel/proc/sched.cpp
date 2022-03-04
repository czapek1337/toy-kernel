#include <string.h>

#include <frg/list.hpp>
#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>

#include <proc/cpu.h>
#include <proc/sched.h>
#include <utils/print.h>

namespace frg {

  template <typename T>
  struct intrusive_traits<T, smarter::shared_ptr<T>, smarter::shared_ptr<T>> {
    static smarter::shared_ptr<T> decay(smarter::shared_ptr<T> owner) {
      return owner;
    }
  };

} // namespace frg

using scheduler_list_t = frg::intrusive_list< //
  scheduler::thread_t,                        //
  frg::locate_member<scheduler::thread_t, scheduler::thread_t::hook_t, &scheduler::thread_t::list_hook>>;

static frg::ticket_spinlock scheduler_lock;
static smarter::shared_ptr<scheduler::thread_t> idle_thread;
static scheduler_list_t thread_queue;

// TODO: Move this to a separate file?
static void sched_syscall_handler(interrupts::isr_frame_t *frame) {
  kdebug("System call 0x{:x} with params: (0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}, 0x{:x})", //
         frame->rax, frame->rdi, frame->rsi, frame->rdx, frame->rcx, frame->r8, frame->r9);
}

static void sched_idle_thread() {
  while (1) {
    kdebug("System is currently idle");

    asm("hlt");
  }
}

static void task1() {
  while (1) {
    kinfo("Thread 1");

    asm("hlt");
  }
}

static void task2() {
  while (1) {
    kinfo("Thread 2");

    asm("hlt");
  }
}

void scheduler::init() {
  interrupts::register_handler(IRQ_SYSCALL, sched_syscall_handler);

  idle_thread = create_thread((uintptr_t) sched_idle_thread, false);

  schedule(create_thread((uintptr_t) task1, false));
  schedule(create_thread((uintptr_t) task2, false));
}

void scheduler::schedule(smarter::shared_ptr<thread_t> thread) {
  frg::unique_lock lock(scheduler_lock);

  thread_queue.push_back(thread);
}

// TODO: Make this an actual scheduler? lol
//
// Currently because of the switch to using frigg/libsmarter I had to get rid
// of the every so primitive scheduler because I haven't figured out a way
// to make it work with frigg, soon it will turn into a fully working on tho :^)
void scheduler::preempt(interrupts::isr_frame_t *frame) {
  frg::unique_lock lock(scheduler_lock);

  auto cpu = cpu::get();

  if (cpu->thread && cpu->thread->state == TS_EXITING) {
    // TODO: Properly exit the thread

    cpu->thread = nullptr;
  }

  if (!thread_queue.empty()) {
    auto next_thread = thread_queue.pop_front();

    if (cpu->thread) {
      memcpy(&cpu->thread->regs, frame, sizeof(interrupts::isr_frame_t));

      thread_queue.push_back(cpu->thread);
    }

    memcpy(frame, &next_thread->regs, sizeof(interrupts::isr_frame_t));

    cpu->thread = next_thread;
    cpu->thread->vm->switch_to();
  } else {
    if (cpu->thread)
      return;

    memcpy(frame, &idle_thread->regs, sizeof(interrupts::isr_frame_t));

    cpu->thread = idle_thread;
    cpu->thread->vm->switch_to();
  }
}
