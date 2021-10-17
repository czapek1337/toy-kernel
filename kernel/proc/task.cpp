#include "task.h"
#include "../arch/gdt.h"

task_t::task_t() {
    pml4 = vmm::create_pml4();
}

task_t::~task_t() {
    vmm::destroy_pml4(pml4);
}

void task_t::save(registers_t *regs) {
    __builtin_memcpy(&this->regs, regs, sizeof(registers_t));
}

void task_t::load(registers_t *regs) {
    __builtin_memcpy(regs, &this->regs, sizeof(registers_t));
}

task_t *task::create(uint64_t entry, uint64_t stack) {
    auto task = new task_t;

    task->pml4 = vmm::create_pml4();

    __builtin_memset(&task->regs, 0, sizeof(registers_t));

    task->regs.rip = entry;
    task->regs.rsp = stack;
    task->regs.rflags = CPU_FLAGS_INTERRUPT | 2; // ?

    task->regs.cs = GDT_KERNEL_CS64;
    task->regs.ss = GDT_KERNEL_DS64;

    return task;
}
