#pragma once

#include <core/string.h>
#include <core/vector.h>
#include <stdint.h>

#include "../boot/stivale2.h"

struct VfsNode;

struct VfsOpenedFile {
    uint64_t seek;
    uint64_t file_size;
    VfsNode *node;
};

struct VfsFileSystem {
    virtual uint64_t open(VfsOpenedFile *file, const core::string_t &path) = 0;
    virtual uint64_t close(VfsOpenedFile *file, uint64_t fd) = 0;
    virtual uint64_t read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) = 0;
    virtual uint64_t write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) = 0;
};

struct VfsNode {
    core::string_t name;
    core::vector_t<VfsNode *> children;
    VfsNode *parent;
    VfsFileSystem *file_system;
};

namespace vfs {

void init(Stivale2Struct *boot_info);

VfsNode *get(VfsNode *parent, const core::string_t &path);

void mount(VfsFileSystem *fs, VfsNode *parent, VfsNode *node);
void append_child(VfsNode *parent, VfsNode *node);
void remove_child(VfsNode *parent, VfsNode *node);

uint64_t open(const core::string_t &fs_path, const core::string_t &path);
uint64_t close(uint64_t fd);
uint64_t read(uint64_t fd, uint8_t *buffer, uint64_t size);
uint64_t write(uint64_t fd, const uint8_t *buffer, uint64_t size);

} // namespace vfs
