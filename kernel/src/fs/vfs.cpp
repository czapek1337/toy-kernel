#include "vfs.h"
#include "../lib/log.h"
#include "dev_fs.h"

static vfs_node_t *root_node;

static vector_t<string_t> split_path(const char *path) {
    vector_t<string_t> result;

    auto path_length = __builtin_strlen(path);
    auto component_start = 0;

    for (auto i = 0; i < path_length; i++) {
        if (path[i] == '/') {
            auto length = i - component_start;

            if (length != 0)
                result.push(string_t(path + component_start, length));

            component_start = i + 1;
        }
    }

    auto rest_length = path_length - component_start;

    if (rest_length > 0)
        result.push(string_t(path + component_start, rest_length));

    return result;
}

void vfs::init() {
    root_node = new vfs_node_t;

    mount(new dev_fs_t, nullptr, "/dev");
}

vfs_node_t *vfs::get(vfs_node_t *parent, const char *path) {
    if (!parent)
        parent = root_node;

    auto components = split_path(path);

    if (components.size() == 0)
        return parent;

    for (auto i = 0; i < components.size(); i++) {
        const auto &component = components[i];

        auto found_child = false;

        for (auto j = 0; j < !found_child && parent->children.size(); j++) {
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

vfs_node_t *vfs::append_child(vfs_node_t *parent, const char *name) {
    if (!parent)
        parent = root_node;

    return nullptr;
}

vfs_node_t *vfs::mount(vfs_file_system_t *fs, vfs_node_t *parent, const char *name) {
    if (!parent)
        parent = root_node;

    return nullptr;
}

void vfs::remove_child(vfs_node_t *parent, const char *name) {
    if (!parent)
        parent = root_node;
}

uint64_t vfs::open(const char *path) {
    return -1;
}

uint64_t vfs::close(uint64_t fd) {
    return -1;
}

uint64_t vfs::read(uint64_t fd, uint8_t *buffer, uint64_t size) {
    return -1;
}

uint64_t vfs::write(uint64_t fd, const uint8_t *buffer, uint64_t size) {
    return -1;
}
