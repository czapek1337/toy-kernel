#include <stdarg.h>
#include <string.h>

#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>

#include <arch/port.h>
#include <proc/cpu.h>
#include <utils/print.h>

static frg::ticket_spinlock print_lock;

static elf64_header_t *kernel_elf_header;
static elf64_section_header_t *kernel_symbol_table;
static elf64_section_header_t *kernel_string_table;

static void print_char(char ch) {
  Port(0x3f8).out8(ch);
}

static void print_string_n(const char *string, size_t len) {
  for (auto i = 0u; i < len; i++) {
    Port(0x3f8).out8(string[i]);
  }
}

static void print_string(const char *string) {
  print_string_n(string, strlen(string));
}

static void print_int(int64_t value) {
  char buffer[20] = {0};

  if (!value) {
    print_char('0');
  } else {
    auto sign = value < 0;

    if (sign)
      value = -value;

    int i = 0;

    for (i = 18; value; i--) {
      buffer[i] = (value % 10) + '0';
      value = value / 10;
    }

    if (sign)
      buffer[i] = '-';
    else
      i++;

    print_string(buffer + i);
  }
}

static void print_uint(uint64_t value) {
  char buffer[21] = {0};

  if (!value) {
    print_char('0');
  } else {
    auto i = 0;

    for (i = 19; value; i--) {
      buffer[i] = (value % 10) + '0';
      value = value / 10;
    }

    print_string(buffer + i + 1);
  }
}

static void print_hex(uint64_t value) {
  char buffer[17] = {0};

  if (!value) {
    print_string("0x0");
  } else {
    auto i = 0;

    for (i = 15; value; i--) {
      buffer[i] = "0123456789abcdef"[value & 0xf];
      value = value >> 4;
    }

    print_string("0x");
    print_string(buffer + i + 1);
  }
}

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

static void print_args(const char *format, va_list args) {
  while (1) {
    while (*format && *format != '%') {
      print_char(*format++);
    }

    if (!*format++)
      break;

    switch (*format++) {
    case 's': {
      char *str = va_arg(args, char *);

      print_string(str ? str : "(null)");
    }; break;
    case 'S': {
      char *str = va_arg(args, char *);

      if (str)
        print_string_n(str, va_arg(args, size_t));
      else
        print_string("(null)");
    }; break;
    case 'd':
      print_int(va_arg(args, int32_t));
      break;
    case 'u':
      print_uint(va_arg(args, uint32_t));
      break;
    case 'x':
      print_hex(va_arg(args, uint32_t));
      break;
    case 'D':
      print_int(va_arg(args, int64_t));
      break;
    case 'U':
      print_uint(va_arg(args, uint64_t));
      break;
    case 'X':
      print_hex(va_arg(args, uint64_t));
      break;
    case 'p':
      print_hex(va_arg(args, uintptr_t));
      break;
    case 'c':
      print_char((char) va_arg(args, int));
      break;
    default:
      print_char('?');
      break;
    }
  }
}

static void print_format(const char *format, ...) {
  va_list args;

  va_start(args, format);

  print_args(format, args);

  va_end(args);
}

void utils::load_kernel_symbols(elf64_header_t *elf) {
  if (memcmp(elf->e_ident, ELFMAG, ELFMAGSZ) != 0)
    klog_panic("Given kernel ELF file is not valid");

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

      klog_error("=> %p [%s+%p]", stack_frame->rip, name, stack_frame->rip - symbol->st_value);
    } else {
      klog_error("=> %p [?]", stack_frame->rip);
    }

    stack_frame = (stack_frame_t *) stack_frame->rbp;
  }

  while (1) {
    asm("cli");
    asm("hlt");
  }
}

void utils::print_line(const char *status, const char *file, size_t line, const char *format, ...) {
  frg::unique_lock lock(print_lock);

  va_list args;

  va_start(args, format);

  print_format("\x1b[30;1m(CPU %d) ", cpu::get()->id);
  print_format("\x1b[37;1m%s:%d %s \x1b[0m", file, line, status);
  print_args(format, args);
  print_char('\n');

  va_end(args);
}
