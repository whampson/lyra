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
# Desc: Contains code for switching the CPU into protected mode.
#       Also contains the Global Descriptor Table.
#-------------------------------------------------------------------------------

#include "boot.h"

.code16
.section .stage2, "ax", @progbits

.globl pm_switch
pm_switch:
    call    a20_enable
    #call    nmi_disable
    lgdt    gdt_desc

    # Here we go...!
    movl    %cr0, %eax
    orw     $0x01, %ax
    movl    %eax, %cr0
    ljmp    $0x08, $pm_entry

.code32
pm_entry:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
#    jmp     kmain

move_kernel:
    movl    $KERNEL_NUM_SECTORS, %ecx
    shll    $9, %ecx
    movl    $KERNEL_TEMP_BASE, %esi
    movl    $0x00400000, %edi
rep movsb   (%esi), (%edi)

    jmp     0x00400000


.section .text
kmain:
    # Put a green 'a' on the screen to prove it worked!
    movw    $0x0261, 0xB8800

pm_halt:
    hlt
    jmp     pm_halt


.section .stage2, "ax", @progbits
gdt_start:
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # 0x08 -- KERNEL_CS
    .quad   0x00CF92000000FFFF      # 0x10 -- KERNEL_DS
    .quad   0x00CFFA000000FFFF      # 0x18 -- USER_CS
    .quad   0x00CFF2000000FFFF      # 0x20 -- USER_DS
    .quad   0x0000000000000000
    .quad   0x0000000000000000
gdt_end:

gdt_desc:
    .word   gdt_end - gdt_start - 1
    .long   gdt_start
