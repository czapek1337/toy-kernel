.PHONY: all run kernel initrd clean

all: build/image.iso

run: build/image.iso
	@qemu-system-x86_64 -smp 4 -M q35 -m 2G -cdrom $< -boot d -serial stdio $(QEMUFLAGS)

run-debug: build/image.iso
	@qemu-system-x86_64 -smp 4 -M q35,smm=off -d int -m 2G -cdrom $< -boot d -serial stdio $(QEMUFLAGS)

limine:
	@git clone https://github.com/limine-bootloader/limine --branch=latest-binary --depth=1
	@make -C limine

kernel: build/kernel.elf

initrd: build/initrd.tar

clean:
	@rm -rf build

CPP_FILES   = $(shell find kernel -type f -name '*.cpp')
ASM_FILES = $(shell find kernel -type f -name '*.asm')
OBJ_FILES = $(patsubst %,build/%.o,$(CPP_FILES) $(ASM_FILES))
DEP_FILES = $(patsubst %,build/%.d,$(CPP_FILES))

ASM ?= nasm
LD  ?= ld
CXX ?= c++

ASMFLAGS = -f elf64 -g
LDFLAGS  = -T misc/kernel.ld -nostdlib -z max-page-size=0x1000 -static
CXXFLAGS = -I kernel -O0 -g -Wall -Wextra -pipe -std=gnu++20 \
	-ffreestanding \
	-fno-exceptions \
	-fno-rtti \
	-fno-omit-frame-pointer \
	-fno-stack-protector \
	-fno-pic \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel

-include $(DEP_FILES)

build/kernel.elf: $(OBJ_FILES)
	@mkdir -p $(dir $@)
	@$(LD) $^ $(LDFLAGS) -o $@

build/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -MD -MF $(patsubst %,build/%.d,$<)

build/%.asm.o: %.asm
	@mkdir -p $(dir $@)
	@$(ASM) $(ASMFLAGS) -o $@ $<

build/initrd.tar:
	@misc/build-initrd.sh $@

build/image.iso: limine kernel initrd
	@rm -rf build/iso_root
	@mkdir -p build/iso_root
	@cp build/kernel.elf build/initrd.tar misc/limine.cfg limine/limine.sys limine/limine-eltorito-efi.bin limine/limine-cd.bin build/iso_root
	@xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-eltorito-efi.bin build/iso_root -o $@ 2>/dev/null
	@rm -rf build/iso_root
