#pragma once

#include <stddef.h>
#include <stdint.h>

#include "elf.h"

void panic_load_symbols(elf64_header_t *elf);
void __attribute__((noreturn)) panic_backtrace();

void println(const char *format, ...);

#define klog(status, format, ...) println("\x1b[37;1m%s:%d " status " \x1b[0m" format, __FILE__, __LINE__, ##__VA_ARGS__)

#define klog_debug(format, ...) klog("\x1b[36;1mdebug", format, ##__VA_ARGS__)
#define klog_info(format, ...) klog("\x1b[32;1minfo", format, ##__VA_ARGS__)
#define klog_warn(format, ...) klog("\x1b[33;1mwarn", format, ##__VA_ARGS__)
#define klog_error(format, ...) klog("\x1b[31;1merror", format, ##__VA_ARGS__)
#define klog_panic(format, ...)                                                                                                            \
  do {                                                                                                                                     \
    klog_error(format, ##__VA_ARGS__);                                                                                                     \
    panic_backtrace();                                                                                                                     \
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
