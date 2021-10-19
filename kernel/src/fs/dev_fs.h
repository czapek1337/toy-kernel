#pragma once

#include "vfs.h"

struct dev_fs_t : vfs_file_system_t {
    dev_fs_t(vfs_node_t *node);

    uint64_t open(vfs_opened_file_t *file, const core::string_t &path) override;
    uint64_t close(vfs_opened_file_t *file, uint64_t fd) override;
    uint64_t read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) override;
};
