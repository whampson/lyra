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
ASFLAGS     := -Wall -Werror -Wextra -Wpedantic -m32 -ffreestanding
CC          := gcc
CCFLAGS     := -Wall -Werror -Wextra -Wpedantic -m32 -ffreestanding
LD          := ld
LDFLAGS     :=

ASM_SOURCES     := $(wildcard *.S)
C_SOURCES       := $(wildcard *.c)
LDSCRIPTS       := $(wildcard *.ld)
LDSCRIPTS_GEN   := $(LDSCRIPTS:.ld=.ld.gen)
OBJECTS         := $(ASM_SOURCES:.S=_asm.o) $(C_SOURCES:.c=.o)

.PHONY: all debug clean remake install

all: boot.bin

debug: ASFLAGS += -g
debug: CCFLAGS += -g
debug: remake

boot.bin: boot.elf
	objcopy -O binary $< $@

boot.elf: $(OBJECTS) $(LDSCRIPTS_GEN)
	$(LD) $(LDFLAGS) -T $(LDSCRIPTS_GEN) -o $@ $(OBJECTS)

$(LDSCRIPTS_GEN): $(LDSCRIPTS)
	scripts/gen-lds.sh $(LDSCRIPTS) $(LDSCRIPTS_GEN)

%_asm.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	@rm -f *.o *.gen *.elf *.bin

remake: clean all

install: boot.bin
	dd if=boot.bin of=/dev/fd0
