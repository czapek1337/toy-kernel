cmake_minimum_required(VERSION 3.16)

project(nucon LANGUAGES ASM_NASM CXX)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-none-elf -mcmodel=kernel -fno-exceptions -fno-use-cxa-atexit -fno-rtti -nostdlib -ffreestanding")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-sse4.1 -mno-sse4.2 -mno-sse4a -mno-3dnow -mno-3dnowa")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m elf_x86_64 -nostdlib -z max-page-size=0x1000")

add_subdirectory(kernel)
add_subdirectory(libs)
add_subdirectory(user)

set(OUTPUT_IMAGE_AHCI "${CMAKE_BINARY_DIR}/disk-ahci.img")
set(OUTPUT_IMAGE_NVME "${CMAKE_BINARY_DIR}/disk-nvme.img")
set(OUTPUT_IMAGE_BIOS "${CMAKE_BINARY_DIR}/disk-bios.img")
set(OUTPUT_IMAGE_UEFI "${CMAKE_BINARY_DIR}/disk-uefi.img")

set(QEMU_ARGS -smp 4 -m 2G -serial stdio
    -drive format=raw,file=${OUTPUT_IMAGE_AHCI},if=none,id=ahci -device ahci,id=ahci -device ide-hd,drive=ahci,bus=ahci.0
    -drive format=raw,file=${OUTPUT_IMAGE_NVME},if=none,id=nvm  -device nvme,serial=deadbeef,drive=nvm
    -trace hda* -trace pci_nvme*)

set(QEMU_DEBUG_ARGS -d int -M q35,smm=off -no-reboot)
set(QEMU_GDB_ARGS -d int -M q35,smm=off -s -S)

set(QEMU_BIOS_ARGS -drive format=raw,file=${OUTPUT_IMAGE_BIOS})
set(QEMU_UEFI_ARGS -drive format=raw,file=${OUTPUT_IMAGE_UEFI}
    -drive if=pflash,format=raw,unit=0,file=${CMAKE_SOURCE_DIR}/misc/ovmf-code-pure-efi.fd,readonly=on
    -drive if=pflash,format=raw,unit=1,file=${CMAKE_SOURCE_DIR}/misc/ovmf-vars-pure-efi.fd)

add_custom_command(OUTPUT ${OUTPUT_IMAGE_AHCI} DEPENDS kernel hello USES_TERMINAL COMMAND ${CMAKE_SOURCE_DIR}/misc/create-disk-image.sh ahci)
add_custom_command(OUTPUT ${OUTPUT_IMAGE_NVME} DEPENDS kernel hello USES_TERMINAL COMMAND ${CMAKE_SOURCE_DIR}/misc/create-disk-image.sh nvme)

add_custom_command(OUTPUT ${OUTPUT_IMAGE_BIOS} DEPENDS ${OUTPUT_IMAGE_AHCI} ${OUTPUT_IMAGE_NVME} USES_TERMINAL COMMAND ${CMAKE_SOURCE_DIR}/misc/create-disk-image.sh bios)
add_custom_command(OUTPUT ${OUTPUT_IMAGE_UEFI} DEPENDS ${OUTPUT_IMAGE_AHCI} ${OUTPUT_IMAGE_NVME} USES_TERMINAL COMMAND ${CMAKE_SOURCE_DIR}/misc/create-disk-image.sh uefi)

add_custom_target(image DEPENDS ${OUTPUT_IMAGE_BIOS})
add_custom_target(image-uefi DEPENDS ${OUTPUT_IMAGE_UEFI})

add_custom_target(run USES_TERMINAL DEPENDS ${OUTPUT_IMAGE_BIOS} COMMAND qemu-system-x86_64 ${QEMU_ARGS} ${QEMU_BIOS_ARGS})
add_custom_target(run-gdb USES_TERMINAL DEPENDS ${OUTPUT_IMAGE_BIOS} COMMAND qemu-system-x86_64 ${QEMU_ARGS} ${QEMU_BIOS_ARGS} ${QEMU_GDB_ARGS})
add_custom_target(run-debug USES_TERMINAL DEPENDS ${OUTPUT_IMAGE_BIOS} COMMAND qemu-system-x86_64 ${QEMU_ARGS} ${QEMU_BIOS_ARGS} ${QEMU_DEBUG_ARGS})

add_custom_target(run-uefi USES_TERMINAL DEPENDS ${OUTPUT_IMAGE_UEFI} COMMAND qemu-system-x86_64 ${QEMU_ARGS} ${QEMU_UEFI_ARGS})
add_custom_target(run-uefi-gdb USES_TERMINAL DEPENDS ${OUTPUT_IMAGE_UEFI} COMMAND qemu-system-x86_64 ${QEMU_ARGS} ${QEMU_UEFI_ARGS} ${QEMU_GDB_ARGS})
add_custom_target(run-uefi-debug USES_TERMINAL DEPENDS ${OUTPUT_IMAGE_UEFI} COMMAND qemu-system-x86_64 ${QEMU_ARGS} ${QEMU_DEBUG_ARGS} ${QEMU_UEFI_ARGS})
