#include "intr.h"
#include "../arch/asm.h"
#include "../arch/gdt.h"
#include "../lib/log.h"
#include "../proc/sched.h"
#include "apic.h"

static bool retain_enabled = true;

static uint64_t retain_depth;

interrupt_retainer_t::interrupt_retainer_t() {
    intr::retain();
}

interrupt_retainer_t::~interrupt_retainer_t() {
    intr::release();
}

void intr::retain_disable() {
    assert_msg(retain_enabled, "retain_disable() called when interrupt retaining was not enabled");

    if (retain_depth > 0)
        log_warn("retain_disable() called when interrupts were retained (retain depth is {})", retain_depth);

    retain_enabled = false;
}

void intr::retain_enable() {
    assert_msg(!retain_enabled, "retain_enable() called when interrupt retaining was already enabled");

    retain_enabled = true;
}

void intr::retain() {
    if (!retain_enabled)
        return;

    arch::disable_interrupts();

    retain_depth++;
}

void intr::release() {
    if (!retain_enabled)
        return;

    assert_msg(retain_depth > 0, "release() called when interrupts were not retained");

    retain_depth--;

    if (retain_depth == 0)
        arch::enable_interrupts();
}

extern "C" void interrupt_handler(registers_t *regs) {
    intr::retain_disable();

    if (regs->isr_number < 32) {
        if (regs->cs == (GDT_USER_CS64 | 3) /* Privilege level */)
            log_info_unlocked("Interrupt came in from CPL3");

        log_fatal_unlocked("A CPU has raised an exception #{}", regs->isr_number);
    } else {
        if (regs->isr_number == 32) {
            auto current = task::get_current_task();
            auto next = task::reschedule();

            if (current != next) {
                if (current)
                    current->save(regs);

                next->load(regs);
            }
        } else {
            log_info_unlocked("Received an interrupt request #{}", regs->isr_number);
        }

        apic::send_eoi();
    }

    intr::retain_enable();
}
