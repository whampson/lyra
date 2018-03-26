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
# File: i8042.S
# Desc: Misc. routines for communicating with the Intel 8042 PS/2 Controller.
#-------------------------------------------------------------------------------

.code16
.section .stage2, "ax", @progbits

##
# Disables the keyboard.
#
#   Inputs: (none)
#  Outputs: (none)
# Clobbers: al
##
.globl i8042_disable
i8042_disable:
    call    i8042_io_p
    movb    $0xAD, %al
    outb    %al, $0x64
    ret

##
# Enables the keyboard.
#
#   Inputs: (none)
#  Outputs: (none)
# Clobbers: al
##
.globl i8042_enable
i8042_enable:
    call    i8042_io_p
    movb    $0xAE, %al
    outb    %al, $0x64
    ret

##
# Pause until PS/2 control is ready for another command or data read/write.
#
#   Inputs: (none)
#  Outputs: (none)
# Clobbers: al
##
.globl i8042_io_p
i8042_io_p:
    call    delay
    inb     $0x64, %al
    testb   $0x03, %al
    jnz     i8042_io_p
    ret
