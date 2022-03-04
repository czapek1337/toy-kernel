#pragma once

#include <stddef.h>
#include <stdint.h>

#include <utils/elf.h>
#include <utils/utils.h>

namespace utils {

  void noreturn print_backtrace();

  void load_kernel_symbols(elf64_header_t *elf);
  void print_line(const char *status, const char *file, size_t line, const char *format, ...);

} // namespace utils

#define klog(status, format, ...) utils::print_line(status, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define klog_debug(format, ...) klog("\x1b[36;1mdebug", format, ##__VA_ARGS__)
#define klog_info(format, ...) klog("\x1b[32;1minfo", format, ##__VA_ARGS__)
#define klog_warn(format, ...) klog("\x1b[33;1mwarn", format, ##__VA_ARGS__)
#define klog_error(format, ...) klog("\x1b[31;1merror", format, ##__VA_ARGS__)
#define klog_panic(format, ...)                                                                                                            \
  do {                                                                                                                                     \
    klog_error(format, ##__VA_ARGS__);                                                                                                     \
    utils::print_backtrace();                                                                                                              \
  } while (0)

#define kassert(expr)                                                                                                                      \
  do {                                                                                                                                     \
    if (!(expr))                                                                                                                           \
      klog_panic("Assertion failed: " #expr);                                                                                              \
  } while (0)

#define kassert_msg(expr, format, ...)                                                                                                     \
  do {                                                                                                                                     \
    if (!(expr))                                                                                                                           \
      klog_panic(format, ##__VA_ARGS__);                                                                                                   \
  } while (0)
