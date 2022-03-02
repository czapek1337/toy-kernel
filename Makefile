.PHONY: all run kernel clean

all: build/image.iso

run: build/image.iso
	@qemu-system-x86_64 -M q35 -m 2G -cdrom $< -boot d -serial stdio $(QEMUFLAGS)

run-debug: build/image.iso
	@qemu-system-x86_64 -M q35,smm=off -d int -m 2G -cdrom $< -boot d -serial stdio $(QEMUFLAGS)

limine:
	@git clone https://github.com/limine-bootloader/limine --branch=latest-binary --depth=1
	@make -C limine

kernel: build/kernel.elf

clean:
	@rm -rf build

C_FILES   = $(shell find kernel -type f -name '*.c')
ASM_FILES = $(shell find kernel -type f -name '*.asm')
OBJ_FILES = $(patsubst %,build/%.o,$(C_FILES) $(ASM_FILES))
DEP_FILES = $(patsubst %,build/%.d,$(C_FILES))

ASM ?= nasm
LD  ?= ld
CC  ?= cc

ASMFLAGS = -f elf64 -g
LDFLAGS  = -T misc/kernel.ld -nostdlib -z max-page-size=0x1000 -static
CFLAGS   = -O0 -g -Wall -Wextra -pipe \
	-ffreestanding \
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

build/%.c.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@ -MD -MF $(patsubst %,build/%.d,$<)

build/%.asm.o: %.asm
	@mkdir -p $(dir $@)
	@$(ASM) $(ASMFLAGS) -o $@ $<

build/image.iso: limine kernel
	@rm -rf build/iso_root
	@mkdir -p build/iso_root
	@cp build/kernel.elf misc/limine.cfg limine/limine.sys limine/limine-cd.bin build/iso_root
	@xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table build/iso_root -o $@ 2>/dev/null
	@rm -rf build/iso_root
