#include "dev_fs.h"

uint64_t dev_fs_t::open(vfs_opened_file_t *file, const string_t &path) {
    return -1;
}

uint64_t dev_fs_t::close(vfs_opened_file_t *file, uint64_t fd) {
    return -1;
}

uint64_t dev_fs_t::read(vfs_opened_file_t *file, uint8_t *buffer, uint64_t size) {
    return -1;
}

uint64_t dev_fs_t::write(vfs_opened_file_t *file, const uint8_t *buffer, uint64_t size) {
    return -1;
}