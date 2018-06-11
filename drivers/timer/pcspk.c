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
 *   File: drivers/timer/pcspk.c
 * Author: Wes Hampson
 *   Desc: PC Speaker driver, controlled by the PIT.
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <lyra/interrupt.h>
#include <lyra/io.h>
#include <drivers/timer.h>

#define PCSPK_ENABLE    0x03
#define PORT_PCSPK      0x61

int beep_ticks;

void pcspk_set_freq(int hz)
{
    timer_set_rate(TIMER_CH_PCSPK, hz);
}

void pcspk_on(void)
{
    uint8_t data;

    data = inb(PORT_PCSPK);
    data |= PCSPK_ENABLE;
    outb(data, PORT_PCSPK);
}

void pcspk_off(void)
{
    uint8_t data;

    data = inb(PORT_PCSPK);
    data &= ~PCSPK_ENABLE;
    outb(data, PORT_PCSPK);
}

void pcspk_beep(int ticks)
{
    uint32_t flags;

    cli_save(flags);
    beep_ticks = ticks;
    pcspk_on();
    restore_flags(flags);
}
