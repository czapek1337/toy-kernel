#pragma once

#include <stddef.h>
#include <stdint.h>

#include <proc/cpu.h>
#include <utils/elf.h>
#include <utils/utils.h>

namespace utils {

  void noreturn print_backtrace();

  void load_kernel_symbols(elf64_header_t *elf);
  void print_line(const char *format, ...);

} // namespace utils

#define klog(status, format, ...)                                                                                                          \
  utils::print_line("\x1b[30;1m(CPU %d) \x1b[37;1m%s:%d " status " \x1b[0m" format, cpu::get()->id, __FILE__, __LINE__, ##__VA_ARGS__)

#define klog_debug(format, ...) klog("\x1b[36;1mdebug", format, ##__VA_ARGS__)
#define klog_info(format, ...) klog("\x1b[32;1minfo", format, ##__VA_ARGS__)
#define klog_warn(format, ...) klog("\x1b[33;1mwarn", format, ##__VA_ARGS__)
#define klog_error(format, ...) klog("\x1b[31;1merror", format, ##__VA_ARGS__)
#define klog_panic(format, ...)                                                                                                            \
  do {                                                                                                                                     \
    klog_error(format, ##__VA_ARGS__);                                                                                                     \
    utils::print_backtrace();                                                                                                              \
  } while (0)

#define assert(expr)                                                                                                                       \
  do {                                                                                                                                     \
    if (!(expr))                                                                                                                           \
      klog_panic("Assertion failed: " #expr);                                                                                              \
  } while (0)

#define assert_msg(expr, format, ...)                                                                                                      \
  do {                                                                                                                                     \
    if (!(expr))                                                                                                                           \
      klog_panic(format, ##__VA_ARGS__);                                                                                                   \
  } while (0)
