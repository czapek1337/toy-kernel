#include "cpu.h"
#include "../intr/apic.h"
#include "../lib/addr.h"
#include "../lib/log.h"
#include "../mm/pmm.h"

// In future if we need more CPUs increate the size of this array
static ProcessorState cpu_info[16];

void arch::init_bsp() {
    __builtin_memset(cpu_info, 0, sizeof(cpu_info));

    auto current = &cpu_info[0];

    current->ap_id = 0;
    current->lapic_id = 0;

    current->is_present = true;
    current->retain_enable = true;

    current->tss.rsp[0] = phys_to_io(pmm::alloc(2) + kib(8));
    current->tss.ist[0] = phys_to_io(pmm::alloc(2) + kib(8));
}

void arch::init_cpu(uint64_t ap_id, uint64_t lapic_id) {
    auto current = get_current_cpu();

    __builtin_memset(current, 0, sizeof(ProcessorState));

    current->ap_id = ap_id;
    current->lapic_id = lapic_id;

    current->is_present = true;
    current->retain_enable = true;

    current->tss.rsp[0] = phys_to_io(pmm::alloc(2) + kib(8));
    current->tss.ist[0] = phys_to_io(pmm::alloc(2) + kib(8));
}

ProcessorState *arch::get_cpu(uint64_t index) {
    assert_msg(index < 16, "The kernel was booted with more APs than supported. Please refer to instructions in arch/cpu.cpp");

    auto cpu = &cpu_info[index];

    assert_msg(cpu->is_present, "The CPU #{} is not present in the system", index);

    return cpu;
}

ProcessorState *arch::get_current_cpu() {
    return get_cpu(apic::get_current_cpu());
}
