#!/bin/bash

BOOT_FILE=boot
BOOT_LINK=bootsect.ld

as --32 -o $BOOT_FILE.o $BOOT_FILE.S
ld -melf_i386 -T $BOOT_LINK -o $BOOT_FILE.elf $BOOT_FILE.o
objcopy -O binary $BOOT_FILE.elf $BOOT_FILE.bin
