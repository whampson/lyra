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
#   File: Makefile
# Author: Wes Hampson
#-------------------------------------------------------------------------------

.PHONY: all img boot kernel kernel_build debug debug_echo clean remake floppy

# Enable/disable debug build
DEBUG           := 1
DEBUGFLAGS      := -D__DEBUG -g

# Enable/disable optimizations
OPTIMIZE        := 1
OPTIMIZEFLAGS   := -O2

GCC_WARNINGS    := -Wall -Wextra -Wpedantic -Wno-unused-function

# Build tools and flags
export AS       := gcc
export ASFLAGS  := $(GCC_WARNINGS) -D__ASM -m32
export CC       := gcc
export CFLAGS   := $(GCC_WARNINGS) -m32 -ffreestanding -fomit-frame-pointer \
                   -fno-unwind-tables -fno-asynchronous-unwind-tables       \
				   -fno-stack-protector -std=c11 -MD -fno-pic
export LD       := ld
export LDFLAGS  :=
export MAKEFLAGS:= --no-print-directory

# Build tools and header directories
export INCLUDE  := $(PWD)/include
export SCRIPTS  := $(PWD)/scripts

# Build output directories
export BIN      := $(PWD)/bin
export OBJ      := $(PWD)/obj
#export OBJ_BOOT := $(PWD)/obj_boot

# Linker script for kernel
LDSCRIPT        := lyra.ld

# Output binaries
BOOTIMG         := $(BIN)/bootldr
BOOTELF         := $(BIN)/bootldr.elf
KERNELIMG       := $(BIN)/kernel
KERNELELF       := $(BIN)/kernel.elf
OSIMG           := $(BIN)/lyra.img

# Code directories
BOOT_DIR        := boot
KERNEL_DIRS     := drivers kernel lib mem

# Object files for the kernel
KERNEL_OBJS     := $(foreach dir, $(KERNEL_DIRS),                       \
                       $(patsubst %.c, $(OBJ)/%.o,                      \
                           $(shell find $(dir) -iname '*.c' -type f)))  \
                    $(foreach dir, $(KERNEL_DIRS),                      \
                       $(patsubst %.S, $(OBJ)/%_asm.o,                  \
                           $(shell find $(dir) -iname '*.S' -type f)))

# Call Makefile in subdirectory
define submake
@$(MAKE) $(MAKEFLAGS) -C $1

endef

# Enable debug flags
ifeq ($(DEBUG), 1)
$(info [INFO]: Debug build)
ASFLAGS += $(DEBUGFLAGS)
CFLAGS  += $(DEBUGFLAGS)
OPTIMIZEFLAGS := -Og
endif

# Enable optimizations
ifeq ($(OPTIMIZE), 1)
$(info [INFO]: Optimizing build with $(OPTIMIZEFLAGS))
CFLAGS += $(OPTIMIZEFLAGS)
endif

all: boot kernel

#img: boot kernel
#	@$(SCRIPTS)/create-img.sh $(BOOTIMG) $(KERNELIMG) $(OSIMG)

dirs:
	@mkdir -p $(BIN)
	@mkdir -p $(OBJ)

boot: dirs
	$(call submake, $(BOOT_DIR))

kernel: kernel_build
	@$(SCRIPTS)/gen-lds.sh $(LDSCRIPT) $(LDSCRIPT).gen -I$(INCLUDE)
	@echo LD $(patsubst $(OBJ)/%, %, $(KERNEL_OBJS))
	@$(LD) $(LDFLAGS) -T $(LDSCRIPT).gen -o $(KERNELELF) $(KERNEL_OBJS)
	@objcopy -O binary $(KERNELELF) $(KERNELIMG)

kernel_build: dirs
	$(foreach dir, $(KERNEL_DIRS), $(call submake, $(dir)))

clean:
	@rm -rf $(BIN)
	@rm -rf $(OBJ)
	@rm -f *.gen

remake: clean all

floppy: img
	$(info [WARNING]: Overwriting floppy disk on /dev/fd0!)
	@sudo dd if=$(OSIMG) of=/dev/fd0 bs=512
