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
#   File: drivers/video/Makefile
# Author: Wes Hampson
#-------------------------------------------------------------------------------

CUR_DIR         := $(notdir $(shell pwd))
OBJ             := $(OBJ)/$(CUR_DIR)
TREE            := $(TREE)/$(CUR_DIR)

ASM_SOURCES     := $(wildcard *.S)
C_SOURCES       := $(wildcard *.c)
OBJECTS         := $(ASM_SOURCES:.S=_asm.o) $(C_SOURCES:.c=.o)
OBJECTS         := $(patsubst %.o, $(OBJ)/%.o, $(OBJECTS))

.PHONY: all dirs

all: dirs $(OBJECTS)

dirs:
	@mkdir -p $(OBJ)

$(OBJ)/%_asm.o: %.S
	@echo AS $(TREE)/$<
	@$(AS) $(ASFLAGS) -I$(INCLUDE) -c -o $@ $<

$(OBJ)/%.o: %.c
	@echo CC $(TREE)/$<
	@$(CC) $(CFLAGS) -I$(INCLUDE) -c -o $@ $<
