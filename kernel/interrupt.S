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
#   File: kernel/interrupt.S
# Author: Wes Hampson
#   Desc: Assembly linkage for interrupt handling. All interrupts generated on
#         the system are routed through the common interrupt handler defined
#         here.
#
# Interrupt handling overview:
#         Each time an interrupt is raised, the CPU will call the cooresponding
#         stub function, which pushes the interrupt vector onto the stack and
#         calls the common interrupt dispatcher. From there, the dispatcher will
#         call the correct handler function based on whether the interrupt is an
#         exception, device IRQ (rasied via the PIC), or system call.
#         For exceptions, the exception number (IDT entry index) is pushed onto
#         the stack. This is always a positive number. For device IRQs, the
#         one's compliment of the IRQ number is pushed onto the stack. This is
#         always a negative number, which allows the dispatcher to distinguish
#         between exceptions and device IRQs. Exceptions are interrupts
#         0x00 - 0x1F, device IRQs are interrupts 0x20 - 0x2F, and system calls
#         use interrupt 0x80.
#-------------------------------------------------------------------------------

#include <lyra/interrupt.h>
#include <lyra/exception.h>
#include <lyra/irq.h>

/* 'struct proc_ctx' field offsets and size. */
#define PROC_CTX_EDI        0
#define PROC_CTX_ESI        4
#define PROC_CTX_EBP        8
#define PROC_CTX_EBX        12
#define PROC_CTX_EDX        16
#define PROC_CTX_ECX        20
#define PROC_CTX_EAX        24
#define SIZEOF_PROC_CTX     28

/* 'struct intr_frame' field offsets. */
#define INTR_FRAME_VEC_NUM  28
#define INTR_FRAME_ERR_CODE 32

common_interrupt_handler:
    # Store process state
    subl    $SIZEOF_PROC_CTX, %esp
    movl    %edi, PROC_CTX_EDI(%esp)
    movl    %esi, PROC_CTX_ESI(%esp)
    movl    %ebp, PROC_CTX_EBP(%esp)
    movl    %ebx, PROC_CTX_EBX(%esp)
    movl    %edx, PROC_CTX_EDX(%esp)
    movl    %ecx, PROC_CTX_ECX(%esp)
    movl    %eax, PROC_CTX_EAX(%esp)

    # Get interrupt vector number
    movl    INTR_FRAME_VEC_NUM(%esp), %eax

    # Pass interrupt frame pointer as first argument
    # (ECX is the first parameter in 'fastcall' calling convention)
    movl    %esp, %ecx

    # Call handler based on interrupt type
    cmpl    $NUM_EXCEPT, %eax
    jb      interrupt_exception
    cmpl    $SYSCALL_VEC, %eax
    je      interrupt_syscall
    jmp     interrupt_device_irq

interrupt_exception:
    call    do_exception
    jmp     interrupt_return

interrupt_device_irq:
    call    do_irq
    jmp     interrupt_return

interrupt_syscall:
    #call    do_syscall
    leal    s_syscall, %ebx
    pushl   %ebx
    call    printf
    addl    $4, %esp
    xorl    %eax, %eax
    jmp     syscall_return

interrupt_return:
    movl    PROC_CTX_EAX(%esp), %eax
syscall_return:
    movl    PROC_CTX_ECX(%esp), %ecx
    movl    PROC_CTX_EDX(%esp), %edx
    movl    PROC_CTX_EBX(%esp), %ebx
    movl    PROC_CTX_EBP(%esp), %ebp
    movl    PROC_CTX_ESI(%esp), %esi
    movl    PROC_CTX_EDI(%esp), %edi
    addl    $SIZEOF_PROC_CTX + 8, %esp
    iret

s_irq:
    .ascii  "Servicing device interrupt..."
    .byte   10, 0

s_syscall:
    .ascii  "Handling system call..."
    .byte   10, 0


/* Below are small 'stub' functions for linking entries in the IDT to the common
   interrupt handler defined above. There exists one of these small stub
   functions for every active entry in the IDT. */

#define GEN_EXCEPT_STUB(stub_name, vec_num)                 \
.globl stub_name;                                           \
stub_name:                                                  \
    pushl   $-1;                                            \
    pushl   $vec_num;                                       \
    jmp     common_interrupt_handler

#define GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_name, vec_num)  \
.globl stub_name;                                           \
stub_name:                                                  \
    pushl   $vec_num;                                       \
    jmp     common_interrupt_handler

#define GEN_IRQ_STUB(stub_name, irq_num)                    \
.globl stub_name;                                           \
stub_name:                                                  \
    pushl   $-1;                                            \
    pushl   $~irq_num;                                      \
    jmp     common_interrupt_handler

#define GEN_SYSCALL_STUB(stub_name)                         \
.globl stub_name;                                           \
stub_name:                                                  \
    pushl   $-1;                                            \
    pushl   $SYSCALL_VEC;                                   \
    jmp     common_interrupt_handler

/* Exception stubs */
GEN_EXCEPT_STUB(stub_except_de, EXCEPT_DE)
GEN_EXCEPT_STUB(stub_except_db, EXCEPT_DB)
GEN_EXCEPT_STUB(stub_except_nmi, EXCEPT_NMI)
GEN_EXCEPT_STUB(stub_except_bp, EXCEPT_BP)
GEN_EXCEPT_STUB(stub_except_of, EXCEPT_OF)
GEN_EXCEPT_STUB(stub_except_br, EXCEPT_BR)
GEN_EXCEPT_STUB(stub_except_ud, EXCEPT_UD)
GEN_EXCEPT_STUB(stub_except_nm, EXCEPT_NM)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_df, EXCEPT_DF)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_ts, EXCEPT_TS)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_np, EXCEPT_NP)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_ss, EXCEPT_SS)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_gp, EXCEPT_GP)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_pf, EXCEPT_PF)
GEN_EXCEPT_STUB(stub_except_mf, EXCEPT_MF)
GEN_EXCEPT_STUB_WITH_ERRORCODE(stub_except_ac, EXCEPT_AC)
GEN_EXCEPT_STUB(stub_except_mc, EXCEPT_MC)
GEN_EXCEPT_STUB(stub_except_xf, EXCEPT_XF)

/* Device IRQ stubs */
GEN_IRQ_STUB(stub_irq_00, IRQ_TIMER)
GEN_IRQ_STUB(stub_irq_01, IRQ_KEYBOARD)
GEN_IRQ_STUB(stub_irq_02, 0x02)
GEN_IRQ_STUB(stub_irq_03, 0x03)
GEN_IRQ_STUB(stub_irq_04, 0x04)
GEN_IRQ_STUB(stub_irq_05, 0x05)
GEN_IRQ_STUB(stub_irq_06, 0x06)
GEN_IRQ_STUB(stub_irq_07, 0x07)
GEN_IRQ_STUB(stub_irq_08, IRQ_RTC)
GEN_IRQ_STUB(stub_irq_09, 0x09)
GEN_IRQ_STUB(stub_irq_10, 0x0A)
GEN_IRQ_STUB(stub_irq_11, 0x0B)
GEN_IRQ_STUB(stub_irq_12, 0x0C)
GEN_IRQ_STUB(stub_irq_13, 0x0D)
GEN_IRQ_STUB(stub_irq_14, 0x0E)
GEN_IRQ_STUB(stub_irq_15, 0x0F)

/* System call stub */
GEN_SYSCALL_STUB(stub_syscall)
