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
#   File: boot/pm.S
# Author: Wes Hampson
#   Desc: Responsible for witching the CPU into protected mode and calling the
#         kernel.
#-------------------------------------------------------------------------------

#include "boot.h"
#include <lyra/descriptor.h>

.code16
.section .stage2, "ax", @progbits

.globl pm_switch
pm_switch:
    # Enable Protected Mode
    movl    %cr0, %eax
    orw     $PE_BIT, %ax
    movl    %eax, %cr0

    # Reload the CS register... here we go!
    ljmpl   $KERNEL_CS, $pm_entry

.code32
pm_entry:
    # We made it!
    movw    $KERNEL_DS, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs

    movl    $KERNEL_STACK_BASE, %ebp
    movl    %ebp, %esp

move_kernel:
    movl    $KERNEL_START_EARLY, %esi   # src
    movl    $KERNEL_START, %edi         # dest
    movzwl  KERNEL_NUM_SECTORS, %ecx    # count
    shll    $9, %ecx                    #   multiply by 512 (sector size)
rep movsb   (%esi), (%edi)

invoke_kernel:
    jmp     *KERNEL_START
