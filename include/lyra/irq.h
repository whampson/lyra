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
 *   File: include/lyra/irq.h
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_IRQ_H
#define __LYRA_IRQ_H

#include <lyra/interrupt.h>
#include <drivers/i8259.h>

/* The IDT vector of IRQ0. */
#define IRQ_BASE_VEC    0x20

/* The total number of IRQs.
   We have 2 PICs (master & slave). */
#define NUM_IRQ         (PIC_NUM_IRQ * 2)

/* IRQ numbers for devices connected to the Intel 8259 PICs. */
#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1
#define IRQ_SLAVE_PIC   2
#define IRQ_RTC         8

#ifndef __ASM__

/**
 * Initialize device interrupts.
 */
void irq_init(void);

/**
 * Enable (unmask) an IRQ line.
 *
 * @param irq_num - the IRQ line to enable
 * @return  0 if the line was successfully enabled
 *         -1 if the IRQ number is invalid
 */
int irq_enable(unsigned int irq_num);

/**
 * Disable (mask) an IRQ line.
 *
 * @param irq_num - the IRQ line to disable
 * @return  0 if the line was successfully disabled
 *         -1 if the IRQ number is invalid
 */
int irq_disable(unsigned int irq_num);

/**
 * Generic IRQ handler. All IRQs are routed through this handler before being
 * dispatched to a device-specific handler.
 *
 * @param regs - the interrupt frame which contains the process and CPU context,
 *               as well as the IRQ number
 */
__attribute__((fastcall))
void do_irq(struct interrupt_frame *regs);

#endif /* __ASM__ */

#endif /* __LYRA_IRQ_H */
