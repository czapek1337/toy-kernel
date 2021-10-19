#pragma once

#include "../boot/stivale2.h"
#include "vfs.h"

struct module_fs_node_t : vfs_node_t {
    uint64_t base;
    uint64_t end;

    module_fs_node_t(uint64_t base, uint64_t end);
};

struct module_fs_t : vfs_file_system_t {
    module_fs_t(vfs_node_t *node, stivale2_struct_modules_tag_t *modules);

    uint64_t open(vfs_opened_file_t *file, const string_t &path) override;
    uint64_t close(vfs_opened_file_t *file, uint64_t fd) override;
    uint64_t read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) override;
};
