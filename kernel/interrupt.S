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
# File: interrupt.S
# Desc:
#-------------------------------------------------------------------------------

#include <init.h>

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

GEN_EXCEPT_HANDLER(except_de, 0x00)     /* Divide Error */
GEN_EXCEPT_HANDLER(except_db, 0x01)     /* Debug */
GEN_EXCEPT_HANDLER(except_nmi, 0x02)    /* Non-Maskable Interrupts */
GEN_EXCEPT_HANDLER(except_bp, 0x03)     /* Breakpoint */
GEN_EXCEPT_HANDLER(except_of, 0x04)     /* Overflow */
GEN_EXCEPT_HANDLER(except_br, 0x05)     /* Bound Range Exceeded */
GEN_EXCEPT_HANDLER(except_ud, 0x06)     /* Invalid Opcode */
GEN_EXCEPT_HANDLER(except_nm, 0x07)     /* No Math Coprocessor */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_df, 0x08)     /* Double Fault */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_ts, 0x0A)     /* Invalid TSS */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_np, 0x0B)     /* Segment Not Present */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_ss, 0x0C)     /* Stack-Segment Fault */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_gp, 0x0D)     /* General Protection Fault */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_pf, 0x0E)     /* Page Fault */
GEN_EXCEPT_HANDLER(except_mf, 0x10)     /* Math Fault */
GEN_EXCEPT_HANDLER_WITH_ERRORCODE(except_ac, 0x11)     /* Alignment Check */
GEN_EXCEPT_HANDLER(except_mc, 0x12)     /* Machine Check */
GEN_EXCEPT_HANDLER(except_xf, 0x13)     /* SIMD Floating-Point Exception */

GEN_IRQ_HANDLER(irq_00, 0x00)

GEN_SYSCALL_HANDLER(system_call)
