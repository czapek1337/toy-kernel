#pragma once

#include "../boot/stivale2.h"
#include "../fs/device_fs.h"
#include "../fs/vfs.h"

struct DeviceZeroNode : DeviceFsNode {
    DeviceZeroNode();

    void init(VfsOpenedFile *file) override;

    uint64_t read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) override;
};

struct DeviceFramebufferNode : DeviceFsNode {
    uint64_t addr;
    uint64_t size;

    DeviceFramebufferNode(Stivale2StructFramebufferTag *framebuffer);

    void init(VfsOpenedFile *file) override;

    uint64_t read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) override;
};

namespace dev {

void init_udev(Stivale2StructFramebufferTag *framebuffer);

}
