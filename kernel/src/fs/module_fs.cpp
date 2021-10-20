#include "module_fs.h"
#include "../lib/log.h"
#include "fd.h"
#include "vfs.h"

module_fs_node_t::module_fs_node_t(uint64_t base, uint64_t end) {
    this->base = base;
    this->end = end;
}

module_fs_t::module_fs_t(vfs_node_t *node, stivale2_struct_modules_tag_t *modules) {
    node->name = "modules";

    vfs::mount(this, nullptr, node);

    for (auto i = 0; i < modules->count; i++) {
        auto module = &modules->modules[i];
        auto module_node = new module_fs_node_t(module->base, module->end);

        module_node->name = module->name;

        vfs::append_child(node, module_node);
    }
}

uint64_t module_fs_t::open(vfs_opened_file_t *file, const core::string_t &path) {
    auto module_node = (module_fs_node_t *) file->node;

    file->seek = 0;
    file->file_size = module_node->end - module_node->base;

    return vfs::fd::allocate(file);
}

uint64_t module_fs_t::close(vfs_opened_file_t *file, uint64_t fd) {
    vfs::fd::destroy(fd);

    return 0;
}

uint64_t module_fs_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    auto module_node = (module_fs_node_t *) file->node;
    auto bytes_left = file->file_size - file->seek;
    auto max_read_size = size > bytes_left ? bytes_left : size;

    if (max_read_size == 0)
        return 0;

    __builtin_memcpy(buffer, (uint8_t *) module_node->base + file->seek, max_read_size);

    return max_read_size;
}

uint64_t module_fs_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    // Unsupported

    return -1;
}
