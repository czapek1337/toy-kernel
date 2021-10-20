#include "udev.h"

udev_zero_node_t::udev_zero_node_t() {
    name = "zero";
}

void udev_zero_node_t::init(vfs_opened_file_t *file) {
    file->seek = 0;
    file->file_size = 0;
}

uint64_t udev_zero_node_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    __builtin_memset(buffer, 0, size);

    return size;
}

uint64_t udev_zero_node_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    return size;
}

udev_fb_node_t::udev_fb_node_t(stivale2_struct_framebuffer_tag_t *framebuffer) {
    name = "st2fb";
    addr = framebuffer->addr;
    size = framebuffer->height * framebuffer->pitch;
}

void udev_fb_node_t::init(vfs_opened_file_t *file) {
    file->seek = 0;
    file->file_size = size;
}

uint64_t udev_fb_node_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    auto remaining = file->file_size - file->seek;
    auto max_read_bytes = remaining > size ? size : remaining;

    __builtin_memcpy(buffer, (uint8_t *) addr + file->seek, max_read_bytes);

    file->seek += max_read_bytes;

    return max_read_bytes;
}

uint64_t udev_fb_node_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    auto remaining = file->file_size - file->seek;
    auto max_write_bytes = remaining > size ? size : remaining;

    __builtin_memcpy((uint8_t *) addr + file->seek, buffer, max_write_bytes);

    file->seek += max_write_bytes;

    return max_write_bytes;
}

void dev::init_udev(stivale2_struct_framebuffer_tag_t *framebuffer) {
    auto dev_node = vfs::get(nullptr, "/dev");

    vfs::append_child(dev_node, new udev_zero_node_t);
    vfs::append_child(dev_node, new udev_fb_node_t(framebuffer));
}
