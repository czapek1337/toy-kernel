#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../interrupts/interrupts.h"
#include "../mem/virt.h"
#include "../utils/elf.h"

typedef struct process process_t;
typedef struct thread thread_t;

struct thread {
  size_t tid;
  size_t exit_code;

  address_space_t *vm;
  isr_frame_t *regs;
};

thread_t *thread_create(vaddr_t entry, bool is_user);
thread_t *thread_create_elf(elf64_header_t *elf, bool is_user);
