# toy-kernel

A UNIX like, non POSIX compliant (for now at least) monolithic
kernel (with aspirations to be a micro kernel) that I am working on in my spare time

## Prerequisites

To build the kernel you will need:

- CMake (with GNU Make or Ninja as project file generator)
- LLVM or GNU toolchain (it is developed with LLVM in mind though);
- NASM

To build the disk image you will need `parted`, and to run the kernel you will need `qemu-system-x86_64`

## Building

Make sure you have checked out all the submodules, as they are required for building the kernel and
the disk image.

First, you will want to generate project files using the project generator of your choice.
Following commands assume the source directory is above current directory in the directory tree:

- for GNU Make: `cmake -G "Unix Makefiles" ..`
- for Ninja: `cmake -G "Ninja" ..`

To build the kernel you can build the `all` target, to build the disk image you can build the `image` target and
to run the kernel you can build one of the following targets: `run`, `run-gdb`, `run-debug`.

The first one simply runs the kernel with serial output, the second one will start the internal QEMU GDB
server to allow for debugging and the third one works like the first one, but will additionally output
some debugging information like interrupt logs:

## Contributing

Contributions are very welcome, if you plan on making a big change to the project please open an issue first.

For any contributions please make sure to follow these naming conventions:

- for types (classes, structs, type definitions) use CamelCase;
- for variables and methods use snake_case;
- - locals and statics don't have any prefixes;
- - member variables use the `m_` prefix;

Also, please make sure the code is formatted according to the [clang-format file](.clang-format) file
in the root of the the repository.

I reserve the right to request any changes to the code style of your contribution if you decide to
make a pull request :^)

## License

This project is licensed under the terms of the GNU General Public License v3.0, the text of the
license can be found in the [LICENSE.txt file](LICENSE.txt) file in the root of the repository.
