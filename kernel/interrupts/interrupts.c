#include "interrupts.h"
#include "../utils/print.h"

void interrupt_handle(isr_frame_t *frame) {
  if (frame->vec < 32)
    klog_panic("An unexpected exception occurred: %x", frame->vec);
}
