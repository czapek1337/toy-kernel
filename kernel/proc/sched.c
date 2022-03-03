#include <string.h>

#include "../utils/print.h"
#include "../utils/spin.h"
#include "cpu.h"
#include "sched.h"

static spin_lock_t scheduler_lock;

static ll_node_t heap_free_list = {
  .prev = &heap_free_list,
  .next = &heap_free_list,
};

static void sched_syscall_handler(isr_frame_t *frame) {
  klog_debug("System call %x with params: (%x, %x, %x, %x, %x, %x)", //
             frame->rax, frame->rdi, frame->rsi, frame->rdx, frame->rcx, frame->r8, frame->r9);
}

static void sched_idle_thread() {
  while (1) {
    asm("hlt");
  }
}

void sched_init() {
  interrupt_register(0x80, sched_syscall_handler);

  sched_push(thread_create((vaddr_t) sched_idle_thread, false));
}

void sched_push(thread_t *thread) {
  spin_lock(&scheduler_lock);

  ll_add_back(&thread->node, &heap_free_list);

  spin_unlock(&scheduler_lock);
}

void sched_preempt(isr_frame_t *frame) {
  cpu_info_t *cpu = cpu_get();

  spin_lock(&scheduler_lock);

  if (cpu->thread)
    ll_add_back(&cpu->thread->node, &heap_free_list);

  thread_t *thread = ll_entry(heap_free_list.next, thread_t, node);

  ll_delete(&thread->node);

  spin_unlock(&scheduler_lock);

  if (cpu->thread)
    memcpy(cpu->thread->regs, frame, sizeof(isr_frame_t));

  memcpy(frame, thread->regs, sizeof(isr_frame_t));

  cpu->thread = thread;

  vm_switch(thread->vm);
}
