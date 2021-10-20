#include "dev_fs.h"
#include "fd.h"

dev_fs_t::dev_fs_t(vfs_node_t *node) {
    node->name = "dev";

    vfs::mount(this, nullptr, node);
}

uint64_t dev_fs_t::open(vfs_opened_file_t *file, const core::string_t &path) {
    auto udev_node = (udev_vfs_node_t *) file->node;

    udev_node->init(file);

    return vfs::fd::allocate(file);
}

uint64_t dev_fs_t::close(vfs_opened_file_t *file, uint64_t fd) {
    vfs::fd::destroy(fd);

    return 0;
}

uint64_t dev_fs_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    auto udev_node = (udev_vfs_node_t *) file->node;

    return udev_node->read(file, buffer, size);
}

uint64_t dev_fs_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    auto udev_node = (udev_vfs_node_t *) file->node;

    return udev_node->write(file, buffer, size);
}
