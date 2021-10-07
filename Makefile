MKDIR = mkdir -p $(dir $@)

ifndef ECHO
	N := x
	C = $(words $N)$(eval N := x $N)
	T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory -nrRf $(firstword $(MAKEFILE_LIST)) ECHO="%CT%" | grep -c "%CT%")
	ECHO = @echo "\e[1m[$C/$T]\e[m"
endif

ifndef FIRMWARE
	FIRMWARE = bios
endif

SOURCES = $(shell find kernel -type f -name '*.cpp' -or -name '*.asm')
HEADERS = $(shell find kernel -type f -name '*.h')
OBJECTS = $(patsubst %, build/%.o, $(SOURCES))

ASM  = nasm
CXX  = clang-12
LD   = ld.lld
QEMU = qemu-system-x86_64

ASM_FLAGS  = -f elf64
CXX_FLAGS  = -target x86_64-none-elf -fno-exceptions -fno-rtti -ffreestanding -mcmodel=kernel -std=c++2b -g -O0 -mno-sse -mno-sse2
LD_FLAGS   = -m elf_x86_64 -z max-page-size=0x1000 -T misc/kernel.ld
QEMU_FLAGS = -serial stdio -drive format=raw,file=build/disk.img -m 4G

ifeq ($(FIRMWARE), uefi)
	QEMU_FLAGS += \
		-drive if=pflash,format=raw,unit=0,file=misc/ovmf-code-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=misc/ovmf-vars-pure-efi.fd
endif

all: help

help:
	@echo "Please choose one of the following commands:"
	@echo "- help:\t\tdisplays this information"
	@echo "- clean:\tcleans the output build directory"
	@echo "- kernel:\tbuilds the kernel binary"
	@echo "- image-bios:\tbuilds a BIOS disk image"
	@echo "- image-uefi:\tbuilds a UEFI disk image"
	@echo "- run:\t\truns the built image in default configuration in QEMU"
	@echo "- run-kvm:\truns the built image in QEMU using KVM"
	@echo "- run-debug:\truns the built image in QEMU with additional debug information"
	@echo "- run-gdb:\truns the built image in QEMU with a GDB server"

clean:
	@rm -rf build

kernel: build/kernel.elf

image-bios: kernel
	@./misc/create-disk-image.sh bios 2>/dev/null

image-uefi: kernel
	@./misc/create-disk-image.sh uefi 2>/dev/null

run: image-$(FIRMWARE)
	@$(QEMU) $(QEMU_FLAGS)

run-kvm: image-$(FIRMWARE)
	@$(QEMU) $(QEMU_FLAGS) -enable-kvm

run-debug: image-$(FIRMWARE)
	@$(QEMU) $(QEMU_FLAGS) -machine smm=off -no-reboot -d int -no-reboot

run-gdb: image-$(FIRMWARE)
	@$(QEMU) $(QEMU_FLAGS) -machine smm=off -no-reboot -d int -no-reboot -s -S

#################

build/kernel/%.asm.o: kernel/%.asm
	@$(MKDIR)
	@$(ECHO) "\e[37mkernel \e[92mASM\e[m" $<
	@$(ASM) $(ASM_FLAGS) -o $@ $<

build/kernel/%.cpp.o: kernel/%.cpp $(HEADERS)
	@$(MKDIR)
	@$(ECHO) "\e[37mkernel \e[92mCXX\e[m" $<
	@$(CXX) $(CXX_FLAGS) -c -o $@ $<

build/kernel.elf: $(OBJECTS)
	@$(MKDIR)
	@$(ECHO) "\e[37mkernel \e[92mLD\e[m" $@
	@$(LD) $(LD_FLAGS) -o $@ $^
