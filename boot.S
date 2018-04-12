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
# File: boot.S
# Desc: Stage 1 boot code (a.k.a. the bootsector). The primary purpose of this
#       code is to load the second stage and kernel code. This file also
#       contains miscellaneous routines used throughout the stage 1 and stage 2
#       boot phases.
#-------------------------------------------------------------------------------

#include "boot.h"

.code16
.section .stage1, "ax", @progbits

##
# Bootloader entry point -- where it all begins!
##
.globl entry
entry:
    cli
    movw    %cs, %ax
    movw    %ax, %ds
    xorw    %ax, %ax
    movw    %ax, %ss
    movw    $BOOT_STACK_BASE, %bp
    movw    %bp, %sp

    leaw    s_boot, %bx
    call    print

load_stage2:
    movw    $STAGE2_BASE, %bx
    movb    $STAGE2_START_SECTOR, %cl
    movb    $STAGE2_NUM_SECTORS, %ch
    call    read_sectors
    cmpb    $0, %ah
    jnz     disk_err

load_kernel:
    movw    $KERNEL_TEMP_BASE, %bx
    movb    $KERNEL_START_SECTOR, %cl
    movb    $KERNEL_NUM_SECTORS, %ch
    call    read_sectors
    cmpb    $0, %ah
    jnz     disk_err

continue_boot:
    call    intr_disable
    call    a20_enable
    jmp     pm_switch

##
# Loads sectors of contiguous data from the boot disk to the address specified
# by 0000:bx.
# Each sector is 512 bytes.
#
# LIMITATIONS:
#   As of now, this routine only loads sectors from the first track of the
#   first head of the floppy. Therefore, only 18 sectors can be loaded at a
#   time.
#
#   Inputs: bx - destination address
#           cl - starting sector
#           ch - number of sectors
#  Outputs: ah - 0 for success, nonzero if an error occurred
# Clobbers: ax, ch, dx, es, si
##
read_sectors:
    xorw    %ax, %ax
    movw    %ax, %es
    movw    $FLOPPY_NUM_RETRIES, %si
    movb    %ch, %al

_read_sectors_loop:
    decw    %si
    movb    $0x02, %ah                  # func: read floppy
    movb    $0, %ch                     # cylinder number
    movb    $0, %dh                     # head number
    movb    $0, %dl                     # drive number
    int     $0x13
    jc      _read_sectors_retry
    cmpb    $0, %ah
    jnz     _read_sectors_retry
    ret

_read_sectors_retry:
    cmpw    $0, %si
    je      _read_sectors_error
    jmp     _read_sectors_loop

_read_sectors_error:
    ret

##
# Prints a disk read error message and halts.
##
disk_err:
    leaw    s_disk_err, %bx
    call    print
    jmp     halt

s_disk_err:
    .ascii  "Disk read error! Unable to boot."
    .byte   10, 13, 0


##
# Masks all PIC interrupts and disables non-maskable interrupts (NMIs).
##
intr_disable:
    # Mask PIC interrupts
    movb    $0xff, %al          # mask all IRQs
    outb    %al, $0xa1          # send mask to slave PIC
    outb    %al, $0x21          # send mask to master PIC

    # Disable non-maskable interrupts
    inb     $0x70, %al          # read CMOS register
    orb     $0x80, %al          # set NMI disable bit
    outb    %al, $0x70          # write CMOS register

    ret

##
# Prints a zero-terminated string to the screen at the current cursor position.
#
#   Inputs: bx - address of string to print
#  Outputs: (none)
# Clobbers: ax, bx
##
.globl print
print:
    movb    $0x0e, %ah          # 0x0e - TTY mode

_print_loop:
    movb    (%bx), %al
    cmpb    $0, %al
    je      _print_done
    int     $0x10
    incw    %bx
    jmp     _print_loop

_print_done:
    ret


##
# Spin forever.
##
.globl halt
halt:
    hlt
    jmp     halt

.globl s_boot
s_boot:
    .ascii  "Booting Lyra..."
    .byte   13, 10, 0
