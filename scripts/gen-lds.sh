#!/bin/bash

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
# File: gen-lds.sh
# Desc: "Preprocessor" for GNU linker scripts.
#-------------------------------------------------------------------------------

if [ $# -lt 1 ]; then
    echo "$0: usage: input_ld output_ld cflags"
    exit 1
fi

gcc $3 -E -x c $1 | grep -v '^#' > $2
