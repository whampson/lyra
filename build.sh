#!/bin/bash

as --32 -o a20.o a20.S
as --32 -o boot.o boot.S
as --32 -o i8042.o i8042.S
ld -T bootloader.ld -o boot.elf a20.o boot.o i8042.o
objcopy -O binary boot.elf boot.bin
