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
#   File: drivers/Makefile
# Author: Wes Hampson
#-------------------------------------------------------------------------------

CUR_DIR         := $(notdir $(shell pwd))
export OBJ      := $(OBJ)/$(CUR_DIR)
export TREE     := $(CUR_DIR)

# TODO: automate this somehow; generate list of directories
DRIVERS         := i8259 input

.PHONY: all dirs $(DRIVERS)

all: dirs $(DRIVERS)

dirs:
	@mkdir -p $(OBJ)

i8259:
	@$(MAKE) $(MAKEFLAGS) -C i8259

input:
	@$(MAKE) $(MAKEFLAGS) -C input