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
 * File: i8269.h
 * Desc: Header file for the Intel 8259 PIC driver.
 *----------------------------------------------------------------------------*/

#ifndef __DRIVERS_i8259_H__
#define __DRIVERS_i8259_H__

/* I/O ports for each 8259 PIC. */
#define PORT_PIC0_CMD   0x20
#define PORT_PIC0_DATA  (PORT_PIC0_CMD + 1)
#define PORT_PIC1_CMD   0xA0
#define PORT_PIC1_DATA  (PORT_PIC1_CMD + 1)

/* Number of IRQ lines per PIC. */
#define PIC_NUM_IRQ     8

#ifndef __ASM__

/**
 * Initialize the Intel 8259 PICs.
 * (TODO: what parameters are used for initialization?)
 */
void i8259_init(void);

/**
 * Masks an IRQ line on either the master or slave PIC.
 * The PIC on which the line is masked is chosen based on the IRQ number.
 *
 * @param irq_num - the IRQ line to mask
 */
void i8259_mask(unsigned int irq_num);

/**
 * Unmasks an IRQ line on either the master or slave PIC.
 * The PIC on which the line is unmasked is chosen based on the IRQ number.
 *
 * @param irq_num - the IRQ line to unmask
 */
void i8259_unmask(unsigned int irq_num);

/**
 * Send the End-Of-Interrupt command.
 *
 * @param the IRQ line that originally triggered the interrupt
 */
void i8259_eoi(int irq_num);

#endif /* __ASM__ */

#endif /* __DRIVERS_i8259_H__ */
