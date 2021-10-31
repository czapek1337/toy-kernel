#include "fd.h"
#include "../proc/sched.h"

static uint64_t fd_counter = 0;

uint64_t vfs::fd::allocate(VfsOpenedFile *file) {
    auto current_task = task::get_current_task();
    auto fd = current_task->fd_counter++;

    current_task->open_fds.insert(fd, file);

    return fd;
}

uint64_t vfs::fd::duplicate(uint64_t old_fd) {
    auto current_task = task::get_current_task();
    auto file = current_task->open_fds.lookup(old_fd);
    auto fd = current_task->fd_counter++;

    if (file == nullptr)
        return -1;

    current_task->open_fds.insert(fd, file);

    return fd;
}

void vfs::fd::destroy(uint64_t fd) {
    auto current_task = task::get_current_task();

    current_task->open_fds.remove(fd);
}
