#pragma once

#include <utils/print.h>

// TODO: Rewrite this in C++ so it makes sense lol

#define ALLOC(type)                                                                                                                        \
  ({                                                                                                                                       \
    klog_warn("Allocation request for %d bytes", sizeof(type));                                                                            \
    (type *) nullptr;                                                                                                                      \
  })

#define ALLOC_ZERO(type) ALLOC(type)
