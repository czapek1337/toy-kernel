#include <system/syscalls.h>

extern "C" int _start() {
    char limine_cfg[256];

    for (auto i = 0; i < sizeof(limine_cfg); i++) {
        limine_cfg[i] = 0;
    }

    auto limine_cfg_fd = system::open("/modules/limine.cfg");
    auto framebuffer_fd = system::open("/dev/st2fb");

    system::trace("Contents of limine.cfg:");

    auto size = system::read(limine_cfg_fd, (uint8_t *) limine_cfg, sizeof(limine_cfg));

    system::trace(limine_cfg);

    system::trace("Filling the screen with red");

    while (true) {
        auto red = 0x00ff0000;
        auto written = system::write(framebuffer_fd, (uint8_t *) &red, sizeof(red));

        if (written < 4)
            break;
    }

    system::trace("Done");
    system::trace("Exiting now, good bye!");

    system::close(limine_cfg_fd);
    system::close(framebuffer_fd);

    system::exit(0);

    while (true) {
        // Make sure we exit properly and dont return from this code
    }
}
