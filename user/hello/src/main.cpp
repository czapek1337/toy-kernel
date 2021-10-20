#include <system/syscalls.h>

extern "C" int _start() {
    char limine_cfg[256];

    for (auto i = 0; i < sizeof(limine_cfg); i++) {
        limine_cfg[i] = 0;
    }

    auto fd = system::open("/modules/limine.cfg");
    auto size = system::read(fd, (uint8_t *) limine_cfg, sizeof(limine_cfg));

    system::trace("Hello, world!");
    system::trace(limine_cfg);

    system::close(fd);
    system::exit(0);

    while (true) {
        // Make sure we exit properly and dont return from this code
    }
}
