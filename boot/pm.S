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

.globl pm_switch
pm_switch:
    movl    %cr0, %eax
    orw     $0x01, %ax
    movl    %eax, %cr0

    # Here we go...!
    ljmpl   $BOOT_CS, $pm_entry

.code32
pm_entry:
    movw    $BOOT_DS, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs

move_kernel:
    movl    $KERNEL_START_EARLY, %esi       # src
    movl    $KERNEL_START, %edi             # dest
    movzwl  KERNEL_NUM_SECTORS_PTR, %ecx    # count
    shll    $9, %ecx                        #   multiply by 512 (sector size)
rep movsb   (%esi), (%edi)

invoke_kernel:
    call    *KERNEL_START

idle:
    hlt
    jmp     idle
