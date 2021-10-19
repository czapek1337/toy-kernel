#include "module_fs.h"
#include "vfs.h"

module_fs_node_t::module_fs_node_t(uint64_t base, uint64_t end) {
    this->base = base;
    this->end = end;
}

module_fs_t::module_fs_t(vfs_node_t *node, stivale2_struct_modules_tag_t *modules) {
    node->name = "modules";

    for (auto i = 0; i < modules->count; i++) {
        auto module = &modules->modules[i];
        auto module_node = new module_fs_node_t(module->base, module->end);

        module_node->name = module->name;

        vfs::append_child(node, module_node);
    }

    vfs::mount(this, nullptr, node);
}

uint64_t module_fs_t::open(vfs_opened_file_t *file, const string_t &path) {
    return -1;
}

uint64_t module_fs_t::close(vfs_opened_file_t *file, uint64_t fd) {
    return -1;
}

uint64_t module_fs_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    return -1;
}

uint64_t module_fs_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    return -1;
}
