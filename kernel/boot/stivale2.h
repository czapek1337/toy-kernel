#pragma once

#include <stdint.h>

enum stivale2_header_tag_t : uint64_t {
    STIVALE2_HEADER_FRAMEBUFFER_TAG = 0x3ecc1bc43d0f7971,
    STIVALE2_HEADER_UNMAP_NULL_TAG = 0x92919432b16fe7e7,
};

enum stivale2_struct_tag_t : uint64_t {
    STIVALE2_STRUCT_PMRS_TAG = 0x5df266a64047b6bd,
    STIVALE2_STRUCT_CMD_LINE_TAG = 0xe5e76a1b4597a781,
    STIVALE2_STRUCT_MMAP_TAG = 0x2187f79e8612de07,
    STIVALE2_STRUCT_FRAMEBUFFER_TAG = 0x506461d2950408fa,
    STIVALE2_STRUCT_MODULES_TAG = 0x4b6fe466aade04ce,
    STIVALE2_STRUCT_RSDP_TAG = 0x9e1786930a375e78,
};

struct stivale2_tag_t {
    uint64_t type;
    uint64_t next;
};

enum stivale2_header_flags_t : uint64_t {
    STIVALE2_HEADER_KASLR = 1 << 0,
    STIVALE2_HEADER_HIGHER_HALF = 1 << 1,
    STIVALE2_HEADER_PMRS = 1 << 2,
};

struct stivale2_header_t {
    uint64_t entry;
    uint64_t stack;
    uint64_t flags;
    uint64_t tags;
};

struct stivale2_header_framebuffer_tag_t {
    stivale2_tag_t tag;

    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint16_t reserved;
};

struct stivale2_header_unmap_null_tag_t {
    stivale2_tag_t tag;
};

struct stivale2_struct_t {
    char loader_name[64];
    char loader_version[64];

    uint64_t tags;
};

enum stivale2_pmr_permissions_t : uint64_t {
    STIVALE2_PMR_EXECUTABLE = 1 << 0,
    STIVALE2_PMR_WRITABLE = 1 << 1,
    STIVALE2_PMR_READABLE = 1 << 2,
};

struct stivale2_pmr_t {
    uint64_t base;
    uint64_t size;
    uint64_t permissions;
};

struct stivale2_struct_pmrs_tag_t {
    stivale2_tag_t tag;

    uint64_t count;
    stivale2_pmr_t pmrs[];
};

struct stivale2_struct_cmd_line_tag_t {
    stivale2_tag_t tag;
    uint64_t cmd_line;
};

enum stivale2_mmap_type_t : uint32_t {
    STIVALE2_MMAP_USABLE = 1,
    STIVALE2_MMAP_RESERVED = 2,
    STIVALE2_MMAP_ACPI_RECLAIMABLE = 3,
    STIVALE2_MMAP_ACPI_NVS = 4,
    STIVALE2_MMAP_BAD_MEMORY = 5,
    STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE = 0x1000,
    STIVALE2_MMAP_KERNEL_AND_MODULES = 0x1001,
    STIVALE2_MMAP_FRAMEBUFFER = 0x1002,
};

struct stivale2_mmap_t {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t reserved;
};

struct stivale2_struct_mmap_tag_t {
    stivale2_tag_t tag;

    uint64_t count;
    stivale2_mmap_t mmap[];
};

struct stivale2_struct_framebuffer_tag_t {
    stivale2_tag_t tag;

    uint64_t addr;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused;
};

struct stivale2_module_t {
    uint64_t base;
    uint64_t end;

    char name[128];
};

struct stivale2_struct_modules_tag_t {
    stivale2_tag_t tag;

    uint64_t count;
    stivale2_module_t modules[];
};

struct stivale2_struct_rsdp_tag_t {
    stivale2_tag_t tag;
    uint64_t addr;
};

stivale2_tag_t *query_tag(stivale2_struct_t *boot_info, uint64_t type);
stivale2_module_t *query_module(stivale2_struct_t *boot_info, const char *name);
