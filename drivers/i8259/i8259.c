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
#include <drivers/i8259.h>

/* Initialization Command Words for the master and slave PICs. */
/* TODO: break these down or build dynamically using parameters. */
#define ICW1        0x11
#define ICW2_MASTER 0x20
#define ICW2_SLAVE  0x28
#define ICW3_MASTER 0x04
#define ICW3_SLAVE  0x02
#define ICW4        0x01

/* End-Of-Interrupt command word. */
#define OCW_EOI     0x60

void i8259_init(void)
{
    uint8_t mask0, mask1;

    /* Save masks */
    mask0 = inb(PORT_PIC0_DATA);
    mask1 = inb(PORT_PIC1_DATA);

    /* Configure master PIC */
    outb_p(ICW1, PORT_PIC0_CMD);
    outb_p(ICW2_MASTER, PORT_PIC0_DATA);
    outb_p(ICW3_MASTER, PORT_PIC0_DATA);
    outb_p(ICW4, PORT_PIC0_DATA);

    /* Configure slave PIC */
    outb_p(ICW1, PORT_PIC1_CMD);
    outb_p(ICW2_SLAVE, PORT_PIC1_DATA);
    outb_p(ICW3_SLAVE, PORT_PIC1_DATA);
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
        outb(OCW_EOI | 0x02, PORT_PIC0_CMD);
    }
}
