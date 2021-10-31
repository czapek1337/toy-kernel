#include "udev.h"

DeviceZeroNode::DeviceZeroNode() {
    name = "zero";
}

void DeviceZeroNode::init(VfsOpenedFile *file) {
    file->seek = 0;
    file->file_size = 0;
}

uint64_t DeviceZeroNode::read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) {
    __builtin_memset(buffer, 0, size);

    return size;
}

uint64_t DeviceZeroNode::write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) {
    return size;
}

DeviceFramebufferNode::DeviceFramebufferNode(Stivale2StructFramebufferTag *framebuffer) {
    name = "st2fb";
    addr = framebuffer->addr;
    size = framebuffer->height * framebuffer->pitch;
}

void DeviceFramebufferNode::init(VfsOpenedFile *file) {
    file->seek = 0;
    file->file_size = size;
}

uint64_t DeviceFramebufferNode::read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) {
    auto remaining = file->file_size - file->seek;
    auto max_read_bytes = remaining > size ? size : remaining;

    __builtin_memcpy(buffer, (uint8_t *) addr + file->seek, max_read_bytes);

    file->seek += max_read_bytes;

    return max_read_bytes;
}

uint64_t DeviceFramebufferNode::write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) {
    auto remaining = file->file_size - file->seek;
    auto max_write_bytes = remaining > size ? size : remaining;

    __builtin_memcpy((uint8_t *) addr + file->seek, buffer, max_write_bytes);

    file->seek += max_write_bytes;

    return max_write_bytes;
}

void dev::init_udev(Stivale2StructFramebufferTag *framebuffer) {
    auto dev_node = vfs::get(nullptr, "/dev");

    vfs::append_child(dev_node, new DeviceZeroNode);
    vfs::append_child(dev_node, new DeviceFramebufferNode(framebuffer));
}
