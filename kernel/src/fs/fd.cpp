#include "fd.h"
#include "../sched/sched.h"

uint64_t vfs::fd::allocate(VfsOpenedFile *file) {
    auto current_task = sched::get_current_thread();
    auto fd = current_task->process->fd_counter++;

    current_task->process->file_descriptors.insert(fd, file);

    return fd;
}

uint64_t vfs::fd::duplicate(uint64_t old_fd) {
    auto current_task = sched::get_current_thread();
    auto file = current_task->process->file_descriptors.lookup(old_fd);
    auto fd = current_task->process->fd_counter++;

    if (file == nullptr)
        return -1;

    current_task->process->file_descriptors.insert(fd, file);

    return fd;
}

void vfs::fd::destroy(uint64_t fd) {
    auto current_task = sched::get_current_thread();
    auto file = current_task->process->file_descriptors.lookup(fd);

    current_task->process->file_descriptors.remove(fd);
}
