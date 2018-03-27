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

.align 16
gdt_start:
    .quad   0x0000000000000000
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # 0x10 -- KERNEL_CS
    .quad   0x00CF92000000FFFF      # 0x18 -- KERNEL_DS
    .quad   0x00CFFA000000FFFF      # 0x20 -- USER_CS
    .quad   0x00CFF2000000FFFF      # 0x28 -- USER_DS
    .quad   0x0000000000000000
    .quad   0x0000000000000000
gdt_end:

.align 4
gdt_desc:
    .word   gdt_end - gdt_start - 1
    .long   gdt_start
.align  4

nmi_disable:
    inb     $0x70, %al
    orb     $0x80, %al
    outb    %al, $0x70
    ret

.globl pm_switch
pm_switch:
    call    a20_enable

    call    nmi_disable
    lgdt    gdt_desc

    # Here we go...!
    leaw    s_to_pm, %bx
    call    print
    movl    %cr0, %eax
    orw     $0x01, %ax
    movl    %eax, %cr0

    ljmpl   $0x10, $pm_entry
