#pragma once

#define ALIGN_DOWN(value, align) ((value) & ~((align) - (1)))
#define ALIGN_UP(value, align) ALIGN_DOWN(value + align - 1, align)
