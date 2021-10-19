#include "task.h"
#include "../arch/gdt.h"
#include "syscall.h"

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

    syscall::set_gs_base((uint64_t) this);

    vmm::switch_to(pml4);
}

void task_t::kill(uint64_t exit_code) {
    this->is_running = false;
    this->exit_code = exit_code;
}
