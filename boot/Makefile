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
#   File: boot/Makefile
# Author: Wes Hampson
#-------------------------------------------------------------------------------

##
# Common Makefile header
##
CUR_DIR         := $(notdir $(shell pwd))
OBJ             := $(OBJ)/$(CUR_DIR)
TREE            := $(CUR_DIR)


# Source files
SOURCES         := boot.S stage2.S a20.S    # boot.S MUST be first

# Object files
OBJECTS         := $(SOURCES:.S=_asm.o)
OBJECTS         := $(patsubst %.o, $(OBJ)/%.o, $(OBJECTS))

# Linker scripts
LDSCRIPTS       := $(wildcard *.ld)
LDSCRIPTS_GEN   := $(LDSCRIPTS:.ld=.ld.gen)

# Target binaries
BOOT_TARGET     := bootldr

.PHONY: all dirs

all: dirs $(BIN)/$(BOOT_TARGET)

dirs:
	@mkdir -p $(OBJ)

$(BIN)/$(BOOT_TARGET): $(BIN)/$(BOOT_TARGET).elf
	@objcopy -O binary $< $@

$(BIN)/$(BOOT_TARGET).elf: $(OBJECTS) $(OBJ)/$(LDSCRIPTS_GEN)
	@#echo LD $(patsubst $(OBJ)/%, $(CUR_DIR)/%, $(OBJECTS))
	@echo LD $(BOOT_TARGET)
	@$(LD) $(LDFLAGS) -T $(OBJ)/$(LDSCRIPTS_GEN) -o $@ $(OBJECTS)

$(OBJ)/$(LDSCRIPTS_GEN): $(LDSCRIPTS)
	@$(SCRIPTS)/gen-lds.sh $^ $@ -I$(INCLUDE)

$(OBJ)/%_asm.o: %.S
	@echo AS $(TREE)/$<
	@$(AS) $(ASFLAGS) -I$(INCLUDE) -c -o $@ $<

#ASM_SOURCES     := $(wildcard *.S)
#C_SOURCES       := $(wildcard *.c)
#LDSCRIPTS       := $(wildcard *.ld)
#LDSCRIPTS_GEN   := $(LDSCRIPTS:.ld=.ld.gen)
#OBJECTS         := $(ASM_SOURCES:.S=_asm.o) $(C_SOURCES:.c=.o)
#OBJECTS         := $(filter-out boot_asm.o, $(OBJECTS))
#OBJECTS         := $(patsubst %.o, $(OBJ_BOOT)/%.o, $(OBJECTS))
#
#.PHONY: all
#
#
#
#all: $(BIN)/boot.bin
#
#$(BIN)/boot.bin: $(BIN)/boot.elf
#	@objcopy -O binary $< $@
#
#$(BIN)/boot.elf: $(OBJ_BOOT)/boot_asm.o $(OBJECTS) $(OBJ_BOOT)/$(LDSCRIPTS_GEN)
#	@echo LD $(patsubst $(OBJ_BOOT)/%, $(CUR_DIR)/%, $< $(OBJECTS))
#	@$(LD) $(LDFLAGS) -T $(OBJ_BOOT)/$(LDSCRIPTS_GEN) -o $@ $< $(OBJECTS)
#
#$(OBJ_BOOT)/$(LDSCRIPTS_GEN): $(LDSCRIPTS)
#	@$(SCRIPTS)/gen-lds.sh $^ $@ -I$(INCLUDE)
#
#$(OBJ_BOOT)/%_asm.o: %.S
#	@echo AS $(TREE)/$<
#	@$(AS) $(ASFLAGS) -I$(INCLUDE) -c -o $@ $<
#
#$(OBJ_BOOT)/%.o: %.c
#	@echo CC $(TREE)/$<
#	@$(CC) $(CFLAGS) -I$(INCLUDE) -c -o $@ $<

