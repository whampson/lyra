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
 * File: irq.c
 * Desc:
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <lyra/irq.h>
#include <lyra/io.h>

#define PIC0_CMD    0x20
#define PIC0_DATA   0x21
#define PIC1_CMD    0xA0
#define PIC1_DATA   0xA1

void irq_enable(int irq_num)
{
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = PIC0_DATA;
    }
    else {
        port = PIC1_DATA;
    }

    data = inb(port) & ~(1 << (irq_num & 0x07));
    outb(data, port);
}

void irq_disable(int irq_num)
{
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = PIC0_DATA;
    }
    else {
        port = PIC1_DATA;
    }

    data = inb(port) | (1 << (irq_num & 0x07));
    outb(data, port);
}
