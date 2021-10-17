# toy-kernel

a unix inspired, non posix compliant micro kernel (more of a monolithic kernel for now though) that i am working on in my spare time

## prerequisites

to build the kernel you will need:

- clang (version 12 or newer),
- lld (i don't think the version matters here),
- nasm (i don't think it matters here either)

to build the disk image you will need `parted`, and to run the kernel you will need `qemu`

## building

make sure you checked out the `limine` submodule as it is required to build the disk image

to build the kernel itself you can run `make kernel` in the root directory

to build the disk image you can run one of these following commands:

- `make image-bios` which will build a legacy disk image with limine installed in its boot sector,
- `make image-uefi` which will build an uefi compatible disk image

to run the kernel you can run one of these following commands:

- `make run` which will run the legacy disk image in default configuration without kvm,
- `make run-kvm` which will run the kernel in default configuration with kvm,
- `make run-debug` which will run the kernel with interrupt logging and without kvm,
- `make run-gdb` which will run the kernel in default configuration without kvm and with a gdb server set up on port 1234

if you want to use any of these command with a uefi compatible image, add `FIRMWARE=uefi` to the command, i strongly
recommend to use the legacy bios images for testing due to the uefi firmware being terribly slow for some reaosn and
only use the uefi images if you want to make sure everything is working for both legacy and uefi modes

## license

this project is licensed under the terms of the gnu general public license v3.0, the text of the
license can be found in the [LICENSE.txt file](LICENSE.txt) in the root of this repository
