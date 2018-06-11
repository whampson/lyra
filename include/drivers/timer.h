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
 *   File: include/drivers/timer.h
 * Author: Wes Hampson
 *   Desc: Programmable Interval Timer (PIT) driver interface.
 *----------------------------------------------------------------------------*/

#ifndef __DRIVERS_TIMER_H
#define __DRIVERS_TIMER_H

#define TIMER_MAX_FREQ  596591

/**
 * Sets the tick rate of the timer on the specified channel.
 *
 * Channel 0 is connected to IRQ0 and will generate interrupts at the specified
 * rate. Channel 1 is unused. Channel 2 is connected to the PC speaker.
 *
 * If the specified rate exceeds TIMER_MAX_FREQ, the rate will be set to
 * TIMER_MAX_FREQ.
 *
 * @param ch - timer channel (0 or 2, 1 is unused)
 * @param hz - the rate at which to generate timer interrupts in Hz
 */
void timer_set_rate(int ch, int hz);

/**
 * Timer tick interrupt handler.
 */
void timer_do_irq(void);

#endif /* __DRIVERS_TIMER_H */
