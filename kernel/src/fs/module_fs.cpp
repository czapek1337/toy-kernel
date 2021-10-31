#include "module_fs.h"
#include "../lib/log.h"
#include "fd.h"
#include "vfs.h"

ModuleFsNode::ModuleFsNode(uint64_t base, uint64_t end) {
    this->base = base;
    this->end = end;
}

ModuleFs::ModuleFs(VfsNode *node, Stivale2StructModulesTag *modules) {
    node->name = "modules";

    vfs::mount(this, nullptr, node);

    for (auto i = 0; i < modules->count; i++) {
        auto module = &modules->modules[i];
        auto module_node = new ModuleFsNode(module->base, module->end);

        module_node->name = module->name;

        vfs::append_child(node, module_node);
    }
}

uint64_t ModuleFs::open(VfsOpenedFile *file, const core::String &path) {
    auto module_node = (ModuleFsNode *) file->node;

    file->seek = 0;
    file->file_size = module_node->end - module_node->base;

    return vfs::fd::allocate(file);
}

uint64_t ModuleFs::close(VfsOpenedFile *file, uint64_t fd) {
    vfs::fd::destroy(fd);

    return 0;
}

uint64_t ModuleFs::read(VfsOpenedFile *file, uint8_t *buffer, uint64_t size) {
    auto module_node = (ModuleFsNode *) file->node;
    auto bytes_left = file->file_size - file->seek;
    auto max_read_size = size > bytes_left ? bytes_left : size;

    if (max_read_size == 0)
        return 0;

    __builtin_memcpy(buffer, (uint8_t *) module_node->base + file->seek, max_read_size);

    return max_read_size;
}

uint64_t ModuleFs::write(VfsOpenedFile *file, const uint8_t *buffer, uint64_t size) {
    // Unsupported

    return -1;
}
