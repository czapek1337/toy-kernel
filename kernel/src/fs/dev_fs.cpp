#include "dev_fs.h"

dev_fs_t::dev_fs_t(vfs_node_t *node) {
    node->name = "dev";

    vfs::mount(this, nullptr, node);
}

uint64_t dev_fs_t::open(vfs_opened_file_t *file, const core::string_t &path) {
    return -1;
}

uint64_t dev_fs_t::close(vfs_opened_file_t *file, uint64_t fd) {
    return -1;
}

uint64_t dev_fs_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    return -1;
}

uint64_t dev_fs_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    return -1;
}
