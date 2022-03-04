#pragma once

#include <stddef.h>
#include <stdint.h>

#include <frg/formatting.hpp>
#include <frg/string.hpp>

#include <arch/port.h>
#include <utils/elf.h>
#include <utils/utils.h>

namespace utils {

  void noreturn print_backtrace();

  void load_kernel_symbols(elf64_header_t *elf);
  void print_header(frg::string_view prefix, frg::string_view file, size_t line);

  struct kernel_formatter_t {
    void append(char ch) {
      Port(0x3f8).out8(ch);
    }

    void append(const char *str) {
      while (*str) {
        append(*str++);
      }
    }
  };

  inline kernel_formatter_t formatter;

  template <typename... Args>
  void print_line(frg::string_view prefix, frg::string_view file, size_t line, frg::string_view format, Args &&...args) {
    auto fmt = frg::fmt(format, std::forward<Args>(args)...);

    print_header(prefix, file, line);

    frg::format(fmt, formatter);
    frg::format("\n", formatter);
  }

} // namespace utils

#define klog(prefix, format, ...) utils::print_line(prefix, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define kdebug(format, ...) klog("\x1b[36;1mdebug", format, ##__VA_ARGS__)
#define kinfo(format, ...) klog("\x1b[32;1minfo", format, ##__VA_ARGS__)
#define kwarn(format, ...) klog("\x1b[33;1mwarn", format, ##__VA_ARGS__)
#define kerror(format, ...) klog("\x1b[31;1merror", format, ##__VA_ARGS__)
#define kpanic(format, ...)                                                                                                                \
  do {                                                                                                                                     \
    kerror(format, ##__VA_ARGS__);                                                                                                         \
    utils::print_backtrace();                                                                                                              \
  } while (0)

#define kassert(expr)                                                                                                                      \
  do {                                                                                                                                     \
    if (!(expr))                                                                                                                           \
      kpanic("Assertion failed: " #expr);                                                                                                  \
  } while (0)

#define kassert_msg(expr, format, ...)                                                                                                     \
  do {                                                                                                                                     \
    if (!(expr))                                                                                                                           \
      kpanic(format, ##__VA_ARGS__);                                                                                                       \
  } while (0)
