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
 * File: i8269.c
 * Desc: Code for the Intel 8259 PIC driver.
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <lyra/io.h>
#include <lyra/irq.h>
#include <drivers/i8259.h>

/* ICW1 bit fields.
   Parentheses indicate the mode used when the bit is zero. */
#define ICW1_INIT   0x10    /* always needed */
#define ICW1_LTIM   0x08    /* level-triggered mode (edge-triggered mode) */
#define ICW1_ADI4   0x04    /* call address interval 4 (8) */
#define ICW1_SNGL   0x02    /* single (cascade) mode */
#define ICW1_ICW4   0x01    /* ICW4 (not) present */

/* ICW4 bit fields.
   Parentheses indicate the mode used when the bit is zero. */
#define ICW4_SNFM   0x10    /* (not) special fully nested mode */
#define ICW4_BUF    0x08    /* (not) buffered mode */
#define ICW4_MS     0x04    /* buffered mode master (slave) */
#define ICW4_AEOI   0X02    /* auto (normal) EOI */
#define ICW4_8086   0x01    /* 8086 (MCS-80/85) mode */

/* PIC configuration.
   Initialization Command Words for the master and slave PICs. ICW1 and ICW4
   specify operating modes for the PIC. ICW2 specifies the IDT base vector for
   each IRQ line (must be a multiple of 8). ICW3 specifies which IRQ line the
   slave PIC is connected to on the master PIC (only needed when operating in
   cascade mode). For the master PIC, this is value uses a one-hot encoding
   (bit0=IRQ0..bit7=IRQ7); for the slave PIC, the value uses the standard binary
   encoding.
   */
#define ICW1    (ICW1_INIT | ICW1_ICW4)
#define ICW2_M  (IRQ_BASE_VEC)
#define ICW2_S  (IRQ_BASE_VEC + 8)
#define ICW3_M  0x04    /* IRQ 2 (one-hot, 0000 0100 = 0x04) */
#define ICW3_S  (IRQ_SLAVE_PIC)
#define ICW4    (ICW4_8086)

/* End-Of-Interrupt command word. */
#define OCW_EOI 0x60

void i8259_init(void)
{
    uint8_t mask0, mask1;

    /* Save masks */
    mask0 = inb(PORT_PIC0_DATA);
    mask1 = inb(PORT_PIC1_DATA);

    /* Configure master PIC */
    outb_p(ICW1, PORT_PIC0_CMD);
    outb_p(ICW2_M, PORT_PIC0_DATA);
    outb_p(ICW3_M, PORT_PIC0_DATA);
    outb_p(ICW4, PORT_PIC0_DATA);

    /* Configure slave PIC */
    outb_p(ICW1, PORT_PIC1_CMD);
    outb_p(ICW2_S, PORT_PIC1_DATA);
    outb_p(ICW3_S, PORT_PIC1_DATA);
    outb_p(ICW4, PORT_PIC1_DATA);

    /* Restore masks */
    outb(mask0, PORT_PIC0_DATA);
    outb(mask1, PORT_PIC1_DATA);
}

void i8259_mask(unsigned int irq_num)
{
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = PORT_PIC0_DATA;
    }
    else {
        port = PORT_PIC1_DATA;
    }

    data = inb(port) | (1 << (irq_num & 0x07));
    outb(data, port);
}

void i8259_unmask(unsigned int irq_num)
{
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = PORT_PIC0_DATA;
    }
    else {
        port = PORT_PIC1_DATA;
    }

    data = inb(port) & ~(1 << (irq_num & 0x07));
    outb(data, port);
}

void i8259_eoi(int irq_num)
{
    if (irq_num < 8) {
        /* Send EOI to master only */
        outb(OCW_EOI | irq_num, PORT_PIC0_CMD);
    }
    else {
        /* Send EOI to slave then master */
        outb(OCW_EOI | (irq_num & 0x07), PORT_PIC1_CMD);
        outb(OCW_EOI | IRQ_SLAVE_PIC, PORT_PIC0_CMD);
    }
}
