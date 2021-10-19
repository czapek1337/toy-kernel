#include <system/syscalls.h>

extern "C" int _start() {
    system::trace("Hello, world!");
    system::exit(0);

    while (true) {
        // Make sure we exit properly and dont return from this code
    }
}
