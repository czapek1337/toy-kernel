#include <system/syscalls.h>

extern "C" int user_start() {
    system::trace("Hello, world!");

    system::exit(0);

    while (true) {
        // Make sure we exit properly and dont return from this code
    }
}
