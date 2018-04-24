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
    xorw    %ax, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    $STACK_BASE, %bp
    movw    %bp, %sp

    leaw    s_hello, %bx
    call    print

load_stage2:
    movw    $STAGE2_START, %bx
    movb    $STAGE2_SECTOR, %cl
    movb    $STAGE2_NUM_SECTORS, %ch
    call    read_sectors
    cmpw    $0, %ax
    jnz     boot_err

load_kernel_early:
    movw    $KERNEL_START_EARLY, %bx
    movb    $KERNEL_SECTOR, %cl
    movb    KERNEL_NUM_SECTORS_PTR, %ch
    call    read_sectors
    cmpw    $0, %ax
    jnz     boot_err

go_to_pm:
    call    kill_interrupts
    call    a20_enable
    lgdt    gdt_ptr
    jmp     pm_switch

kill_interrupts:
    # Mask PIC interrupts
    movb    $0xff, %al          # mask all IRQs
    outb    %al, $0xa1          # send mask to slave PIC
    outb    %al, $0x21          # send mask to master PIC

    # Disable non-maskable interrupts
    inb     $0x70, %al          # read CMOS register
    orb     $0x80, %al          # set NMI disable bit
    outb    %al, $0x70          # write CMOS register
    ret

boot_err:
    leaw    s_boot_fail, %bx
    call    print
    jmp     halt

halt:
    hlt
    jmp     halt

s_hello:
    .ascii  "Booting Lyra..."
    .byte   13, 10, 0

s_boot_fail:
    .ascii  "Unable to boot."
    .byte   13, 10, 0
