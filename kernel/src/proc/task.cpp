#include "task.h"
#include "../arch/gdt.h"
#include "syscall.h"

Task::Task() {
    pml4 = vmm::create_pml4();
}

Task::~Task() {
    vmm::destroy_pml4(pml4);
}

void Task::save(Registers *regs) {
    __builtin_memcpy(&this->regs, regs, sizeof(Registers));
}

void Task::load(Registers *regs) {
    __builtin_memcpy(regs, &this->regs, sizeof(Registers));

    syscall::set_gs_base((uint64_t) this);

    vmm::switch_to(pml4);
}

void Task::kill(uint64_t exit_code) {
    this->is_running = false;
    this->exit_code = exit_code;
}
