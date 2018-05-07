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
# Desc: Lyra bootloader build script.
#-------------------------------------------------------------------------------

CUR_DIR         := $(notdir $(shell pwd))
OBJ             := $(OBJ)/$(CUR_DIR)

ASM_SOURCES     := $(wildcard *.S)
C_SOURCES       := $(wildcard *.c)
LDSCRIPTS       := $(wildcard *.ld)
LDSCRIPTS_GEN   := $(LDSCRIPTS:.ld=.ld.gen)
OBJECTS         := $(ASM_SOURCES:.S=_asm.o) $(C_SOURCES:.c=.o)
OBJECTS         := $(filter-out boot_asm.o, $(OBJECTS))
OBJECTS         := $(patsubst %.o, $(OBJ_BOOT)/%.o, $(OBJECTS))

.PHONY: all

all: $(BIN)/boot.bin

$(BIN)/boot.bin: $(BIN)/boot.elf
	objcopy -O binary $< $@

$(BIN)/boot.elf: $(OBJ_BOOT)/boot_asm.o $(OBJECTS) $(OBJ_BOOT)/$(LDSCRIPTS_GEN)
	$(LD) $(LDFLAGS) -T $(OBJ_BOOT)/$(LDSCRIPTS_GEN) -o $@ $< $(OBJECTS)

$(OBJ_BOOT)/$(LDSCRIPTS_GEN): $(LDSCRIPTS)
	$(SCRIPTS)/gen-lds.sh $^ $@ -I$(INCLUDE)

$(OBJ_BOOT)/%_asm.o: %.S
	@echo $(AS) $(ASFLAGS) $(CUR_DIR)/$<
	@$(AS) $(ASFLAGS) -I$(INCLUDE) -c -o $@ $<

$(OBJ_BOOT)/%.o: %.c
	@echo $(CC) $(CCFLAGS) $(CUR_DIR)/$<
	@$(CC) $(CCFLAGS) -I$(INCLUDE) -c -o $@ $<
