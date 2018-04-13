#!/bin/bash

rm -f *.o *.bin *.elf
gcc -Wall -Werror -Wextra -Wpedantic -m32 -ffreestanding -c -o kernel.o entry.c
ld -Ttext=0x400000 -ekmain -melf_i386 -o kernel.elf kernel.o
objcopy -O binary kernel.elf kernel.bin
