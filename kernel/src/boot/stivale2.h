#pragma once

#include <stdint.h>

enum Stivale2HeaderTag : uint64_t {
    STIVALE2_HEADER_FRAMEBUFFER_TAG = 0x3ecc1bc43d0f7971,
};

enum Stivale2StructTag : uint64_t {
    STIVALE2_STRUCT_PMRS_TAG = 0x5df266a64047b6bd,
    STIVALE2_STRUCT_CMD_LINE_TAG = 0xe5e76a1b4597a781,
    STIVALE2_STRUCT_MMAP_TAG = 0x2187f79e8612de07,
    STIVALE2_STRUCT_FRAMEBUFFER_TAG = 0x506461d2950408fa,
    STIVALE2_STRUCT_MODULES_TAG = 0x4b6fe466aade04ce,
    STIVALE2_STRUCT_RSDP_TAG = 0x9e1786930a375e78,
};

struct Stivale2Tag {
    uint64_t type;
    uint64_t next;
};

enum Stivale2HeaderFlags : uint64_t {
    STIVALE2_HEADER_KASLR = 1 << 0,
    STIVALE2_HEADER_HIGHER_HALF = 1 << 1,
    STIVALE2_HEADER_PMRS = 1 << 2,
};

struct Stivale2Header {
    uint64_t entry;
    uint64_t stack;
    uint64_t flags;
    uint64_t tags;
};

struct Stivale2HeaderFramebufferTag {
    Stivale2Tag tag;

    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint16_t reserved;
};

enum Stivale2PmrPermissions : uint64_t {
    STIVALE2_PMR_EXECUTABLE = 1 << 0,
    STIVALE2_PMR_WRITABLE = 1 << 1,
    STIVALE2_PMR_READABLE = 1 << 2,
};

struct Stivale2Pmr {
    uint64_t base;
    uint64_t size;
    uint64_t permissions;
};

struct Stivale2StructPmrsTag {
    Stivale2Tag tag;

    uint64_t count;
    Stivale2Pmr pmrs[];
};

struct Stivale2StructCmdLineTag {
    Stivale2Tag tag;
    uint64_t cmd_line;
};

enum Stivale2MemoryMapEntryType : uint32_t {
    STIVALE2_MMAP_USABLE = 1,
    STIVALE2_MMAP_RESERVED = 2,
    STIVALE2_MMAP_ACPI_RECLAIMABLE = 3,
    STIVALE2_MMAP_ACPI_NVS = 4,
    STIVALE2_MMAP_BAD_MEMORY = 5,
    STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE = 0x1000,
    STIVALE2_MMAP_KERNEL_AND_MODULES = 0x1001,
    STIVALE2_MMAP_FRAMEBUFFER = 0x1002,
};

struct Stivale2MemoryMapEntry {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t reserved;
};

struct Stivale2StructMemoryMapTag {
    Stivale2Tag tag;

    uint64_t count;
    Stivale2MemoryMapEntry mmap[];
};

struct Stivale2StructFramebufferTag {
    Stivale2Tag tag;

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

struct Stivale2Module {
    uint64_t base;
    uint64_t end;

    char name[128];
};

struct Stivale2StructModulesTag {
    Stivale2Tag tag;

    uint64_t count;
    Stivale2Module modules[];
};

struct Stivale2StructRsdpTag {
    Stivale2Tag tag;
    uint64_t addr;
};

struct Stivale2Struct {
    char loader_name[64];
    char loader_version[64];

    uint64_t tags;

    Stivale2Tag *query_tag(uint64_t type);
    Stivale2Module *query_module(const char *name);
};
