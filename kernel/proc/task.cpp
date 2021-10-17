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

    vmm::switch_to(pml4);
}
