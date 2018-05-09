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
# Desc: Lyra OS build script.
#-------------------------------------------------------------------------------

.PHONY: all img boot kernel debug debug_echo clean remake floppy

export BIN      := $(PWD)/bin
export OBJ      := $(PWD)/obj
export OBJ_BOOT := $(PWD)/obj_boot
export INCLUDE  := $(PWD)/include
export SCRIPTS  := $(PWD)/scripts

WARNFLAGS       := -Wall -Wextra -Wpedantic

export AS       := gcc
export ASFLAGS  := $(WARNFLAGS) -D__ASM__ -m32
export CC       := gcc
export CCFLAGS  := $(WARNFLAGS) -m32 -ffreestanding -fomit-frame-pointer \
                   -fno-unwind-tables -fno-asynchronous-unwind-tables
export LD       := ld
export LDFLAGS  :=
export MAKEFLAGS:= --no-print-directory

BOOT            := boot
KERNEL          := kernel
MEM             := mem
KERNEL_DIRS     := $(KERNEL) $(MEM)
KERNEL_OBJS     := $(foreach dir, $(KERNEL_DIRS), \
                       $(patsubst %.c, $(OBJ)/%.o, $(wildcard $(dir)/*.c))) \
                   $(foreach dir, $(KERNEL_DIRS), \
                       $(patsubst %.S, $(OBJ)/%_asm.o, $(wildcard $(dir)/*.S)))

BOOTIMG         := $(BIN)/boot.bin
KERNELIMG       := $(BIN)/kernel.bin
OSIMG           := $(BIN)/lyra.img

all: img

img: boot kernel
	@$(SCRIPTS)/create-img.sh $(BOOTIMG) $(KERNELIMG) $(OSIMG)

dirs:
	@mkdir -p $(BIN)
	@mkdir -p $(OBJ)
	@mkdir -p $(OBJ_BOOT)

debug: export ASFLAGS += -g
debug: export CCFLAGS += -g
debug: debug_echo $(filter-out debug, $(MAKECMDGOALS))
debug_echo:
	@echo [INFO] Compiling with debugging symbols.

boot: dirs
	@$(MAKE) $(MAKEFLAGS) -C $(BOOT)

kernel: dirs
	@$(MAKE) $(MAKEFLAGS) -C $(KERNEL)
	@$(MAKE) $(MAKEFLAGS) -C $(MEM)
	@$(SCRIPTS)/gen-lds.sh kernel.ld kernel.ld.gen -I$(INCLUDE)
	@echo LD $(patsubst $(OBJ)/%, %, $(KERNEL_OBJS))
	@$(LD) $(LDFLAGS) -T kernel.ld.gen -o $(BIN)/kernel.elf $(KERNEL_OBJS)
	@objcopy -O binary $(BIN)/kernel.elf $(BIN)/kernel.bin

clean:
	@rm -rf $(BIN)
	@rm -rf $(OBJ)
	@rm -rf $(OBJ_BOOT)
	@rm -f *.gen

remake: clean all

floppy: img
	@# WARNING: check yer floppies!
	@sudo dd if=$(OSIMG) of=/dev/fd0 bs=512
