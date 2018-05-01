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
# File: create-img.sh
# Desc: Creates floppy disk image for Lyra.
#-------------------------------------------------------------------------------

SECTOR_SIZE=512
MAX_SECTOR_COUNT=18
KERNEL_NUM_SECTORS_ADDR=502

if [ $# -lt 3 ]; then
    echo "$0: usage: boot_img kernel_img out_img"
    exit 1
fi

boot_img=$1
kernel_img=$2
out_img=$3

# Compute number of sectors needed to hold kernel image
kernel_size=$(wc -c $kernel_img)
kernel_size=${kernel_size/%\ */}    # separate size and filename
num_sectors=$(awk -v s=$SECTOR_SIZE -v k=$kernel_size \
    'BEGIN { print k / s + (s - (k % s)) / s }')

# Combine boot and kernel images
cat $boot_img $kernel_img > $out_img
if [ $? -ne 0 ]; then
    exit 1
fi

# Update KERNEL_NUM_SECTORS value in disk image
printf "\x$(printf %02x $num_sectors)" |\
    dd of=$out_img bs=1 seek=$KERNEL_NUM_SECTORS_ADDR conv=notrunc status=none
if [ $? -ne 0 ]; then
    exit 1
fi
