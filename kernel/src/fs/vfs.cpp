#include "vfs.h"
#include "../lib/log.h"
#include "device_fs.h"
#include "module_fs.h"

static VfsNode *root_node;

static core::Vector<core::String> split_path(const core::String &path) {
    core::Vector<core::String> result;

    auto component_start = 0;

    for (auto i = 0; i < path.size(); i++) {
        if (path.data()[i] == '/') {
            auto length = i - component_start;

            if (length != 0)
                result.push(core::String(path.data() + component_start, length));

            component_start = i + 1;
        }
    }

    auto rest_length = path.size() - component_start;

    if (rest_length > 0)
        result.push(core::String(path.data() + component_start, rest_length));

    return result;
}

static void iterate_node(VfsNode *node, int depth) {
    char buffer[128];

    __builtin_memset(buffer, 0, sizeof(buffer));
    __builtin_memset(buffer, '-', depth);

    log_debug("{} {}", buffer, node->name);

    for (auto i = 0; i < node->children.size(); i++) {
        iterate_node(node->children[i], depth + 1);
    }
}

void vfs::init(Stivale2Struct *boot_info) {
    auto modules = (Stivale2StructModulesTag *) boot_info->query_tag(STIVALE2_STRUCT_MODULES_TAG);

    root_node = new VfsNode;
    root_node->name = "/";

    new DeviceFs(new VfsNode);
    new ModuleFs(new VfsNode, modules);
}

VfsNode *vfs::get(VfsNode *parent, const core::String &path) {
    parent = parent ?: root_node;

    auto components = split_path(path);

    if (components.size() == 0)
        return parent;

    for (auto i = 0; i < components.size(); i++) {
        const auto &component = components[i];

        auto found_child = false;

        for (auto j = 0; !found_child && j < parent->children.size(); j++) {
            auto child = parent->children[j];

            if (child->name == component) {
                parent = child;
                found_child = true;

                break;
            }
        }

        if (!found_child)
            return nullptr;
    }

    return parent;
}

void vfs::mount(VfsFileSystem *fs, VfsNode *parent, VfsNode *node) {
    append_child(parent ?: root_node, node);

    node->file_system = fs;
}

void vfs::append_child(VfsNode *parent, VfsNode *node) {
    parent = parent ?: root_node;

    node->parent = parent;
    node->file_system = parent->file_system;

    parent->children.push(node);
}

void vfs::remove_child(VfsNode *parent, VfsNode *node) {
    parent = parent ?: root_node;

    for (auto i = 0; i < parent->children.size(); i++) {
        auto child = parent->children[i];

        if (child != node)
            continue;

        parent->children.remove(child);

        delete child;

        return;
    }

    log_warn("Could not delete a VFS node '{}' in parent '{}'", node->name, parent->name);
}

uint64_t vfs::open(const core::String &fs_path, const core::String &path) {
    auto parent = get(nullptr, fs_path);

    if (!parent)
        return -1;

    auto node = get(parent, path);

    if (!node)
        return -1;

    auto file = new VfsOpenedFile;

    file->seek = 0;
    file->file_size = 0;
    file->node = node;

    return node->file_system->open(file, path);
}

uint64_t vfs::close(uint64_t fd) {
    auto current_task = sched::get_current_thread();
    auto file = current_task->process->file_descriptors.lookup(fd);

    if (!file)
        return -1;

    return file->node->file_system->close(file, fd);
}

uint64_t vfs::read(uint64_t fd, uint8_t *buffer, uint64_t size) {
    auto current_task = sched::get_current_thread();
    auto file = current_task->process->file_descriptors.lookup(fd);

    if (!file)
        return -1;

    return file->node->file_system->read(file, buffer, size);
}

uint64_t vfs::write(uint64_t fd, const uint8_t *buffer, uint64_t size) {
    auto current_task = sched::get_current_thread();
    auto file = current_task->process->file_descriptors.lookup(fd);

    if (!file)
        return -1;

    return file->node->file_system->write(file, buffer, size);
}
