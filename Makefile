#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Copyright (C) 2018 Wes Hampson. All Rights Reserved.                         #
#                                                                              #
# This file is part of the Lyra operating system.                              #
#                                                                              #
# Lyra is free software: you can redistribute it and/or modify                 #
# it under the terms of version 2 of the GNU General Public License            #
# as published by the Free Software Foundation.                                #
#                                                                              #
# See LICENSE in the top-level directory for a copy of the license.            #
# You may also visit <https://www.gnu.org/licenses/gpl-2.0.txt>.               #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#-------------------------------------------------------------------------------
# File: Makefile
# Desc: Bootloader build script.
#-------------------------------------------------------------------------------

AS          := gcc
ASFLAGS     := -m32 -ffreestanding
CC          := gcc
CCFLAGS     := -m32 -ffreestanding
LD          := ld
LDSCRIPT    := bootloader.ld
LDFLAGS     := -T $(LDSCRIPT)

ASM_SOURCES := $(wildcard *.S)
C_SOURCES   := $(wildcard *.c)
OBJECTS     := $(ASM_SOURCES:.S=_asm.o) $(C_SOURCES:.c=.o)

ELFTARGET   := boot.elf
BINTARGET   := boot.bin

.PHONY: all debug clean remake

all: $(BINTARGET)

$(BINTARGET): $(ELFTARGET)
	objcopy -O binary $< $@

$(ELFTARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%_asm.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

debug: ASFLAGS += -g
debug: remake

clean:
	@rm -f *.o *.elf *.bin

remake: clean all
