#include "stivale2.h"

static Stivale2HeaderFramebufferTag framebuffer_tag = {
    .tag = {.type = STIVALE2_HEADER_FRAMEBUFFER_TAG, .next = 0},
    .width = 0,
    .height = 0,
    .bpp = 32,
};

[[gnu::aligned(16)]] //
static uint8_t kernel_stack[8192];

[[gnu::section(".stivale2hdr"), gnu::used]] //
static Stivale2Header header = {
    .entry = 0,
    .stack = (uint64_t) &kernel_stack[sizeof(kernel_stack)],
    .flags = STIVALE2_HEADER_HIGHER_HALF | STIVALE2_HEADER_PMRS,
    .tags = (uintptr_t) &framebuffer_tag,
};

Stivale2Tag *Stivale2Struct::query_tag(uint64_t type) {
    for (auto tag = (Stivale2Tag *) tags; tag; tag = (Stivale2Tag *) tag->next) {
        if (tag->type == type)
            return tag;
    }

    return nullptr;
}

Stivale2Module *Stivale2Struct::query_module(const char *name) {
    auto modules = (Stivale2StructModulesTag *) query_tag(STIVALE2_STRUCT_MODULES_TAG);

    if (!modules)
        return nullptr;

    for (auto i = 0; i < modules->count; i++) {
        auto module = &modules->modules[i];

        if (__builtin_strcmp(module->name, name) == 0)
            return module;
    }

    return nullptr;
}
