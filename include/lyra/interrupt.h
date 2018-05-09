/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * Copyright (C) 2018 Wes Hampson. All Rights Reserved.                       *
 *                                                                            *
 * This file is part of the Lyra operating system.                            *
 *                                                                            *
 * Lyra is free software: you can redistribute it and/or modify               *
 * it under the terms of version 2 of the GNU General Public License          *
 * as published by the Free Software Foundation.                              *
 *                                                                            *
 * See LICENSE in the top-level directory for a copy of the license.          *
 * You may also visit <https://www.gnu.org/licenses/gpl-2.0.txt>.             *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-----------------------------------------------------------------------------
 * File: interrupt.h
 * Desc: Structures and prototypes for generic interrupt handling.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_INTERRUPT_H__
#define __LYRA_INTERRUPT_H__

/* Number of entries in the IDT. */
#define NUM_VEC     256

/* The interrupt vector used for system calls. */
#define SYSCALL_VEC 0x80

#ifndef __ASM__
#include <types.h>

/* Structure for storing registers during an interrupt or system call. */
struct interrupt_frame {
    /* Process context.
       Pushed by interrupt handler.
       (Mirrors 'struct proc_ctx' defined in proc.h) */
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    /* Exception number when an exception raised.
       One's compliment of IRQ number when IRQ raised.
       0x80 when executing system call. */
    uint32_t vec_num;

    /* Error code when an exception raised.
       Should be set to -1 when error code does not apply. */
    uint32_t err_code;

    /* Hardware context.
       Pushed automatically by CPU when interrupt raised. */
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;   /* ESP and SS only present when... */
    uint32_t ss;    /* ...interrupt causes privilege level change. */
};

/* Sets up and loads the IDT. */
void idt_init(void);

#endif /* __ASM __ */

#endif /* __LYRA_INTERRUPT_H__ */
