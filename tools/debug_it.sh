#!/bin/sh

IMAGE_DIR=../image

killall qemu-system-i386
qemu-system-i386 $IMAGE_DIR/alta_kernel.bin -s -S -vnc none &
gdb
