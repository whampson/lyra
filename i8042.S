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
# Wait for PS/2 controller to be ready before reading from data port.
#
#   Inputs: (none)
#  Outputs: al - PS/2 status register
# Clobbers: al
##
.globl i8042_in_p
i8042_in_p:
    call    delay
    inb     $0x64, %al
    testb   $0x01, %al          # check output buffer status (1 = ready)
    jz      i8042_in_p
    ret

##
# Wait for PS/2 controller to be ready before writing to command or data port.
#
#   Inputs: (none)
#  Outputs: al - PS/2 status register
# Clobbers: al
##
.globl i8042_out_p
i8042_out_p:
    call    delay
    inb     $0x64, %al
    testb   $0x02, %al          # check input buffer status (0 = ready)
    jnz     i8042_out_p
    ret
