#include "intr.h"
#include "../lib/log.h"

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
    assert_msg(retain_enabled, "retain() called when interrupt retaining was not enabled");

    asm("cli");

    retain_depth++;
}

void intr::release() {
    assert_msg(retain_enabled, "release() called when interrupt retaining was not enabled");
    assert_msg(retain_depth > 0, "release() called when interrupts were not retained");

    retain_depth--;

    if (retain_depth == 0)
        asm("sti");
}

extern "C" void interrupt_handler(interrupt_frame_t *regs) {
    intr::retain_disable();

    if (regs->isr_number < 32) {
        log_fatal_unlocked("A CPU has raised an exception #{}", regs->isr_number);
    } else {
        log_info_unlocked("Received an interrupt request #{}", regs->isr_number);
    }

    intr::retain_enable();
}
