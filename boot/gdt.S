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
# File: gdt.S
# Desc: Home of the Global Descriptor Table.
#-------------------------------------------------------------------------------

#include <lyra/init.h>

.code16
.section .stage2, "ax", @progbits

.align 16
temp_gdt_base:
    .quad   0x0000000000000000
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # KERNEL_CS
    .quad   0x00CF92000000FFFF      # KERNEL_DS
    .quad   0x00CFFA000000FFFF      # USER_CS
    .quad   0x00CFF2000000FFFF      # USER_DS
    .quad   0x0000000000000000      # KERNEL_TSS
    .quad   0x0000000000000000      # KERNEL_LDT
temp_gdt_limit:

##
# 48-bit value for the GDTR describing the location and size of the GDT.
##
.align 4
gdt_ptr:
    .word   temp_gdt_limit - temp_gdt_base - 1
    .long   GDT_BASE
    .word   0

.globl setup_gdt
setup_gdt:
move_gdt:
    movl    $temp_gdt_base, %esi    # src
    movl    $GDT_BASE, %edi         # dest
    movzwl  gdt_ptr, %ecx           # count
rep movsb   (%esi), (%edi)
load_gdt:
    lgdtl   gdt_ptr
    ret
