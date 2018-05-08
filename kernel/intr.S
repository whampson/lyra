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
# File: intr.S
# Desc:
#-------------------------------------------------------------------------------

#include <intr.h>
#include <except.h>

#define GEN_EXCEPT_HANDLER(handl_name, vec_num)                 \
.globl handl_name;                                              \
handl_name:                                                     \
    pushl   $0;                                                 \
    pushl   $vec_num;                                           \
    jmp     common_interrupt;

#define GEN_EXCEPT_HANDLER_WITH_ERRORCODE(handl_name, vec_num)  \
.globl handl_name;                                              \
handl_name:                                                     \
    pushl   $vec_num;                                           \
    jmp     common_interrupt;

#define GEN_IRQ_HANDLER(handl_name, irq_num)                    \
.globl handl_name;                                              \
handl_name:                                                     \
    pushl   $0;                                                 \
    pushl   $~irq_num;                                          \
    jmp     common_interrupt

#define GEN_SYSCALL_HANDLER(handl_name)                         \
.globl handl_name;                                              \
handl_name:                                                     \
    pushl   $0;                                                 \
    pushl   $0x80;                                              \
    jmp     common_interrupt

common_interrupt:
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %esi
    pushl   %edi
    pushl   %ebp
    pushl   %esp

    movl    28(%esp), %eax
    # do stuff with eax
    cmpl    $0x80, %eax
    je      do_system_call

return_from_interrupt:
    popl    %esp
    popl    %ebp
    popl    %edi
    popl    %esi
    popl    %edx
    popl    %ecx
    popl    %ebx
    addl    $8, %esp
    iret


do_system_call:
    leal    s_syscall, %ebx
    pushl   %ebx
    call    puts
    addl    $4, %esp
    jmp     return_from_interrupt


s_syscall:
    .ascii  "Handling system call..."
    .byte   10, 0

GEN_EXCEPT_HANDLER(except_de, EXCEPT_DE)
GEN_EXCEPT_HANDLER(except_db, EXCEPT_DB)
GEN_EXCEPT_HANDLER(except_nmi, EXCEPT_NMI)
GEN_EXCEPT_HANDLER(except_bp, EXCEPT_BP)
GEN_EXCEPT_HANDLER(except_of, EXCEPT_OF)
GEN_EXCEPT_HANDLER(except_br, EXCEPT_BR)
GEN_EXCEPT_HANDLER(except_ud, EXCEPT_UD)
GEN_EXCEPT_HANDLER(except_nm, EXCEPT_NM)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_df, EXCEPT_DF)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_ts, EXCEPT_TS)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_np, EXCEPT_NP)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_ss, EXCEPT_SS)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_gp, EXCEPT_GP)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_pf, EXCEPT_PF)
GEN_EXCEPT_HANDLER(except_mf, EXCEPT_MF)
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_ac, EXCEPT_AC)
GEN_EXCEPT_HANDLER(except_mc, EXCEPT_MC)
GEN_EXCEPT_HANDLER(except_xf, EXCEPT_XF)

GEN_IRQ_HANDLER(irq_00, 0x00)

GEN_SYSCALL_HANDLER(system_call)
