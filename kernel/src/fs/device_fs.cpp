#include "device_fs.h"
#include "fd.h"

DeviceFs::DeviceFs(VfsNode *node) {
    node->name = "dev";

    vfs::mount(this, nullptr, node);
}

uint64_t DeviceFs::open(VfsOpenedFile *file, const core::String &path) {
    auto udev_node = (DeviceFsNode *) file->node;

    udev_node->init(file);

    return vfs::fd::allocate(file);
}

uint64_t DeviceFs::close(VfsOpenedFile *file, uint64_t fd) {
    vfs::fd::destroy(fd);

    return 0;
}

uint64_t DeviceFs::read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) {
    auto udev_node = (DeviceFsNode *) file->node;

    return udev_node->read(file, buffer, size);
}

uint64_t DeviceFs::write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) {
    auto udev_node = (DeviceFsNode *) file->node;

    return udev_node->write(file, buffer, size);
}
