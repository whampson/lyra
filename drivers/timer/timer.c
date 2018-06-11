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
 *   File: drivers/timer/timer.c
 * Author: Wes Hampson
 *   Desc: Programmable Interval Timer (PIT) driver.
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <drivers/timer.h>
#include <lyra/io.h>

/* PIT clock rate */
#define PIT_CLK         1193182

/* I/O ports */
#define PORT_PIT_CH0    0x40
#define PORT_PIT_CH1    0x41
#define PORT_PIT_CH2    0x42
#define PORT_PIT_CMD    0x43

/* Channel masks */
#define CH0             0x00
#define CH1             0x40
#define CH2             0x80

/* Access mode masks */
#define LATCH           0x00
#define LOBYTE          0x10
#define HIBYTE          0x20
#define LOHIBYTE        0x30

/* Operating mode masks */
#define SQUARE_WAVE     0x06

int beep_ticks = 0;

void timer_set_rate(int ch, int hz)
{
    uint16_t timer_port;
    uint16_t divisor;
    uint8_t ch_mask;
    uint8_t cfg;

    switch (ch) {
        case 0:
            timer_port = PORT_PIT_CH0;
            ch_mask = CH0;
            break;
        case 2:
            timer_port = PORT_PIT_CH2;
            ch_mask = CH2;
            break;
        default:
            return;
    }

    if (hz > TIMER_MAX_FREQ) {
        hz = TIMER_MAX_FREQ;
    }
    divisor = PIT_CLK / hz;

    /* Configure channel */
    cfg = ch_mask | LOHIBYTE | SQUARE_WAVE;
    outb(cfg, PORT_PIT_CMD);

    /* Set tick frequency */
    outb((uint8_t) divisor, timer_port);
    outb((uint8_t) (divisor >> 8), timer_port);
}

/**
 * Timer tick interrupt handler.
 */
void timer_do_irq(void)
{

}

