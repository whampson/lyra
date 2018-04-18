#!/bin/bash

rm -f *.o *.bin *.elf
gcc -Wall -Werror -Wextra -Wpedantic -m32 -ffreestanding -c -o kernel.o entry.c
ld -T kernel.ld -o kernel.elf kernel.o
objcopy -O binary kernel.elf kernel.bin
