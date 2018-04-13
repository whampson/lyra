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
# File: pm.S
# Desc: Contains code for switching the CPU into protected mode and calling the
#       kernel.
#       Also contains the Global Descriptor Table.
#-------------------------------------------------------------------------------

#include "boot.h"

.code16
.section .stage2, "ax", @progbits

.align 16
gdt_start:
    .quad   0x0000000000000000
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # 0x10 -- KERNEL_CS
    .quad   0x00CF92000000FFFF      # 0x18 -- KERNEL_DS
    .quad   0x00CFFA000000FFFF      # 0x23 -- USER_CS
    .quad   0x00CFF2000000FFFF      # 0x2b -- USER_DS
    .quad   0x0000000000000000
    .quad   0x0000000000000000
gdt_end:

gdt_desc:
    .word   gdt_end - gdt_start - 1
    .long   gdt_start
    .word   0

.globl pm_switch
pm_switch:
    lgdt    gdt_desc

    movl    %cr0, %eax
    orw     $0x01, %ax
    movl    %eax, %cr0

    # Here we go...!
    ljmpl   $0x10, $pm_entry

.code32
pm_entry:
    movw    $0x18, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs

    movl    $0x7C00, %ebp
    movl    %ebp, %esp

move_kernel:
    movl    $KERNEL_NUM_SECTORS, %ecx
    shll    $9, %ecx
    movl    $KERNEL_TEMP_BASE, %esi
    movl    $0x00400000, %edi
rep movsb   (%esi), (%edi)

    call    0x00400000

pm_halt:
    hlt
    jmp     pm_halt
