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
 * File: except.h
 * Desc:
 *----------------------------------------------------------------------------*/

#ifndef __EXCEPT_H__
#define __EXCEPT_H__

#define NUM_EXCEPT  32

#define EXCEPT_DE   0x00    /* Divide Error */
#define EXCEPT_DB   0x01    /* Debug */
#define EXCEPT_NMI  0x02    /* Non-Maskable Interrupt */
#define EXCEPT_BP   0x03    /* Breakpoint */
#define EXCEPT_OF   0x04    /* Overflow */
#define EXCEPT_BR   0x05    /* Bound Range Exceeded */
#define EXCEPT_UD   0x06    /* Invalid Opcode */
#define EXCEPT_NM   0x07    /* No Math Coprocessor */
#define EXCEPT_DF   0x08    /* Double Fault */
#define EXCEPT_TS   0x0A    /* Invalid TSS */
#define EXCEPT_NP   0x0B    /* Segment Not Present */
#define EXCEPT_SS   0x0C    /* Stack-Segment Fault */
#define EXCEPT_GP   0x0D    /* General Protection Fault */
#define EXCEPT_PF   0x0E    /* Page Fault */
#define EXCEPT_MF   0x10    /* Math Fault */
#define EXCEPT_AC   0x11    /* Alignment Check */
#define EXCEPT_MC   0x12    /* Machine Check */
#define EXCEPT_XF   0x13    /* SIMD Floating-Point Exception */



#endif /* __EXCEPT_H__ */
