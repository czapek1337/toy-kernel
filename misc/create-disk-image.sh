#!/bin/bash

CMD=$1

BUILD_DIR=$(pwd)
LIMINE_DIR=$(pwd)/../limine
MISC_DIR=$(pwd)/../misc

DISK_IMAGE=$BUILD_DIR/disk-$1.img
DISK_ROOT=$BUILD_DIR/disk_root
LOOPBACK=$BUILD_DIR/loopback_dev

if [ -f $DISK_IMAGE ]; then
    rm -rf $DISK_IMAGE
fi

if [ -d $DISK_ROOT ]; then
    rm -rf $DISK_ROOT
fi

# Create the disk image and image root
dd if=/dev/zero of=$DISK_IMAGE bs=1M count=64
mkdir $DISK_ROOT

# Create a primary partition
parted -s $DISK_IMAGE mklabel gpt
parted -s $DISK_IMAGE mkpart primary fat32 2048s 100%

# Mount the disk image
sudo losetup -Pf --show $DISK_IMAGE > $LOOPBACK
sudo partprobe `cat $LOOPBACK` >/dev/null 2>&1
sudo mkfs.fat -F 32 `cat $LOOPBACK`p1 >/dev/null 2>&1
sudo mount `cat $LOOPBACK`p1 $DISK_ROOT >/dev/null 2>&1

# Copy the files needed to boot over
sudo cp $BUILD_DIR/kernel/kernel $DISK_ROOT/kernel.elf
sudo cp $BUILD_DIR/user/hello/hello $DISK_ROOT/hello.elf
sudo cp $MISC_DIR/limine.cfg $DISK_ROOT

if [ "$CMD" = "bios" ]; then
    sudo cp $LIMINE_DIR/limine.sys $DISK_ROOT
    sudo $LIMINE_DIR/limine-install-linux-x86_64 `cat $LOOPBACK` >/dev/null 2>&1
fi

if [ "$CMD" = "uefi" ]; then
    sudo mkdir -p $DISK_ROOT/EFI/BOOT
    sudo cp $LIMINE_DIR/BOOTX64.EFI $DISK_ROOT/EFI/BOOT
fi

# Finish building the image
sudo sync
sudo umount $DISK_ROOT
sudo losetup -d `cat $LOOPBACK`
sudo rm -rf $DISK_ROOT
sudo rm -rf $LOOPBACK
