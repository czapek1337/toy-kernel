extern "C" int _start() {
    asm("syscall" : : "a"(1), "b"("Hello, world!"));
    asm("syscall" : : "a"(0), "b"(0));

    while (true) {
    }
}
