#include "stivale2.h"

static stivale2_header_framebuffer_tag_t framebuffer_tag = {
    .tag = {.type = STIVALE2_HEADER_FRAMEBUFFER_TAG, .next = 0},
    .width = 0,
    .height = 0,
    .bpp = 32,
};

__attribute__((aligned(16), section(".bss"))) //
static uint8_t kernel_stack[8192];

__attribute__((section(".stivale2hdr"), used)) //
static stivale2_header_t header = {
    .entry = 0,
    .stack = (uint64_t) &kernel_stack[sizeof(kernel_stack)],
    .flags = STIVALE2_HEADER_HIGHER_HALF | STIVALE2_HEADER_PMRS,
    .tags = (uintptr_t) &framebuffer_tag,
};

stivale2_tag_t *query_tag(stivale2_struct_t *boot_info, uint64_t type) {
    auto tag = (stivale2_tag_t *) boot_info->tags;

    while (tag) {
        if (tag->type == type)
            return tag;

        tag = (stivale2_tag_t *) tag->next;
    }

    return nullptr;
}

stivale2_module_t *query_module(stivale2_struct_t *boot_info, const char *name) {
    auto modules = (stivale2_struct_modules_tag_t *) query_tag(boot_info, STIVALE2_STRUCT_MODULES_TAG);

    if (!modules)
        return nullptr;

    for (auto i = 0; i < modules->count; i++) {
        auto module = &modules->modules[i];

        if (__builtin_strcmp(module->name, name) == 0)
            return module;
    }

    return nullptr;
}
