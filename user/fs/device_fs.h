#pragma once

#include "vfs.h"

struct DeviceFsNode : VfsNode {
    virtual void init(VfsOpenedFile *file) = 0;

    virtual uint64_t read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) = 0;
    virtual uint64_t write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) = 0;
};

struct DeviceFs : VfsFileSystem {
    DeviceFs(VfsNode *node);

    uint64_t open(VfsOpenedFile *file, const core::String &path) override;
    uint64_t close(VfsOpenedFile *file, uint64_t fd) override;
    uint64_t read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) override;
};
