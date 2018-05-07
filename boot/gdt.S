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

.code16
.section .stage2, "ax", @progbits

.align 16
gdt_start:
    .quad   0x0000000000000000
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # KERNEL_CS     - 0x10
    .quad   0x00CF92000000FFFF      # KERNEL_DS     - 0x18
    .quad   0x00CFFA000000FFFF      # USER_CS       - 0x23
    .quad   0x00CFF2000000FFFF      # USER_DS       - 0x2B
    .quad   0x0000000000000000      # KERNEL_TSS    - 0x30
    .quad   0x0000000000000000      # KERNEL_LDT    - 0x38
gdt_end:

.globl gdt_ptr
gdt_ptr:
    .word   gdt_end - gdt_start - 1
    .long   gdt_start
    .word   0
