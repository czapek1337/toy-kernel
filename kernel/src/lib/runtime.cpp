#include "log.h"

extern "C" void atexit() {
    assert(false);
}

extern "C" void __cxa_atexit() {
    assert(false);
}

extern "C" void __cxa_finalize() {
    assert(false);
}

extern "C" void __cxa_pure_virtual() {
    assert(false);
}
