#pragma once

#include "../boot/stivale2.h"
#include "../fs/dev_fs.h"
#include "../fs/vfs.h"

struct udev_zero_node_t : udev_vfs_node_t {
    udev_zero_node_t();

    void init(vfs_opened_file_t *file) override;

    uint64_t read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) override;
};

struct udev_fb_node_t : udev_vfs_node_t {
    uint64_t addr;
    uint64_t size;

    udev_fb_node_t(stivale2_struct_framebuffer_tag_t *framebuffer);

    void init(vfs_opened_file_t *file) override;

    uint64_t read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) override;
};

namespace dev {

void init_udev(stivale2_struct_framebuffer_tag_t *framebuffer);

}
