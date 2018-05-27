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
#   File: boot/boot.S
# Author: Wes Hampson
#   Desc: Stage 1 boot code (a.k.a. the bootsector). Responsible for loading the
#         second stage bootloader and kernel code.
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
    cld
    xorw    %ax, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    $BOOT_STACK_BASE, %bp
    movw    %bp, %sp

    leaw    s_hello, %bx
    call    print

load_stage2:
    movw    $STAGE2_START, %bx
    movb    STAGE2_SECTOR, %cl
    movb    STAGE2_NUM_SECTORS, %ch
    call    read_sectors
    cmpw    $0, %ax
    jnz     boot_err

load_kernel_early:
    movw    $KERNEL_START_EARLY, %bx
    movb    KERNEL_SECTOR, %cl
    movb    KERNEL_NUM_SECTORS, %ch
    call    read_sectors
    cmpw    $0, %ax
    jnz     boot_err

go_to_pm:
    call    kill_interrupts
    call    a20_enable
    call    setup_gdt
    jmp     pm_switch

kill_interrupts:
    movb    $0xFF, %al          # mask all IRQs
    outb    %al, $0xA1          # send mask to slave PIC
    outb    %al, $0x21          # send mask to master PIC
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
