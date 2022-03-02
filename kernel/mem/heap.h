#pragma once

#include <stddef.h>
#include <stdint.h>

void heap_init();
void heap_free(void *pointer);

void *heap_alloc(size_t size);
void *heap_alloc_zero(size_t size);
