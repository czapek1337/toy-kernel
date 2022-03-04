#include <stdarg.h>
#include <string.h>

#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>

#include <arch/port.h>
#include <proc/cpu.h>
#include <utils/print.h>

static elf64_header_t *kernel_elf_header;
static elf64_section_header_t *kernel_symbol_table;
static elf64_section_header_t *kernel_string_table;

static elf64_symbol_t *resolve_symbol(uint64_t rip) {
  if (!kernel_elf_header || !kernel_symbol_table || !kernel_string_table)
    return nullptr;

  auto symbols = (elf64_symbol_t *) ((uintptr_t) kernel_elf_header + kernel_symbol_table->sh_offset);

  for (auto i = 0u; i < kernel_symbol_table->sh_size / kernel_symbol_table->sh_entsize; i++) {
    auto symbol = &symbols[i];

    if (rip < symbol->st_value || rip > symbol->st_value + symbol->st_size)
      continue;

    if (symbol->st_name < kernel_string_table->sh_size)
      return symbol;
  }

  return nullptr;
}

void utils::print_backtrace() {
  struct stack_frame_t {
    uint64_t rbp;
    uint64_t rip;
  };

  stack_frame_t *stack_frame;

  asm("mov %%rbp, %0" : "=r"(stack_frame) : : "memory");

  while (stack_frame && stack_frame->rip) {
    if (auto symbol = resolve_symbol(stack_frame->rip)) {
      auto name = (const char *) ((uintptr_t) kernel_elf_header + kernel_string_table->sh_offset + symbol->st_name);

      kerror("=> 0x{:x} [{}+0x{:x}]", stack_frame->rip, name, stack_frame->rip - symbol->st_value);
    } else {
      kerror("=> 0x{:x} [?]", stack_frame->rip);
    }

    stack_frame = (stack_frame_t *) stack_frame->rbp;
  }

  while (1) {
    asm("cli");
    asm("hlt");
  }
}

void utils::load_kernel_symbols(elf64_header_t *elf) {
  if (memcmp(elf->e_ident, ELFMAG, ELFMAGSZ) != 0)
    kpanic("Given kernel ELF file is not valid");

  kernel_elf_header = elf;

  for (size_t i = 0; i < elf->e_shnum; i++) {
    elf64_section_header_t *section = (elf64_section_header_t *) ((uintptr_t) elf + elf->e_shoff + elf->e_shentsize * i);

    if (section->sh_type == SHT_SYMTAB && !kernel_symbol_table) {
      kernel_symbol_table = section;
    } else if (section->sh_type == SHT_STRTAB && kernel_symbol_table && kernel_symbol_table->sh_link == i) {
      kernel_string_table = section;
    }
  }

  kassert(kernel_symbol_table);
  kassert(kernel_string_table);
}

void utils::print_header(frg::string_view prefix, frg::string_view file, size_t line) {
  auto fmt = frg::fmt("\x1b[30;1m(CPU {}) \x1b[37;1m{}:{} {} \x1b[0m", cpu::get()->id, file, line, prefix);

  frg::format(fmt, formatter);
}
