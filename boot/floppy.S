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
# File: floppy.S
# Desc: Basic floppy disk driver.
#-------------------------------------------------------------------------------

#include "floppy.h"

.section .stage1, "ax", @progbits
.code16

##
# Loads sectors of contiguous data from the boot disk to the address specified
# by 0000:bx.
# Each sector is 512 bytes.
#
# LIMITATIONS:
#   As of now, this routine only loads sectors from the first track of the
#   first head of the floppy. Therefore, a maximum of 18 sectors can be loaded.
#
#   Inputs: bx - destination address
#           cl - starting sector
#           ch - number of sectors to read
#           al - starting cylinder
#  Outputs: ax - 0 for success, -1 if an error occurred
# Clobbers: ax, ch, dx, es, si
##
.globl read_sectors
read_sectors:
    pushw   %ax
    xorw    %ax, %ax
    movw    %ax, %es
    movw    $RETRY_COUNT, %si

_bounds_check:
    movb    %cl, %al
    addb    %ch, %al        # TODO: test overflow
    subw    $1, %ax
    cmpw    $SECTOR_COUNT, %ax
    ja      _support_err
    popw    %ax             # TEMP FIX: allow reading from more than just cyl 0
    xchgb   %ch, %al        # TODO: rewrite this routine to allow read of
    #movb    %ch, %al       # arbitrary length from arbitrary position

_read_loop:
    decw    %si
    movb    $BIOS_READ_FLOPPY, %ah
    movb    $0, %ch         # cylinder number
    movb    $0, %dh         # head number
    movb    $0, %dl         # drive number
    int     $0x13
    jc      _read_retry
    cmpb    $0, %ah
    jnz     _read_retry
    jmp     _read_done

_read_retry:
    cmpw    $0, %si
    je      _read_error
    jmp     _read_loop

_read_done:
    movw    $0, %ax
    ret

_read_error:
    leaw    s_disk_err, %bx
    call    print
    movw    $-1, %ax
    ret

_support_err:
    leaw    s_cyl_support, %bx
    call    print
    movw    $-1, %ax
    ret

s_disk_err:
    .ascii  "Disk read error!"
    .byte   10, 13, 0

s_cyl_support:
    .ascii "Reading more than one cylinder is not supported yet!"
    .byte   10, 13, 0
