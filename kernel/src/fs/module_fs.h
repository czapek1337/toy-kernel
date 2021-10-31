#pragma once

#include "../boot/stivale2.h"
#include "vfs.h"

struct ModuleFsNode : VfsNode {
    uint64_t base;
    uint64_t end;

    ModuleFsNode(uint64_t base, uint64_t end);
};

struct ModuleFs : VfsFileSystem {
    ModuleFs(VfsNode *node, Stivale2StructModulesTag *modules);

    uint64_t open(VfsOpenedFile *file, const core::String &path) override;
    uint64_t close(VfsOpenedFile *file, uint64_t fd) override;
    uint64_t read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) override;
    uint64_t write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) override;
};
