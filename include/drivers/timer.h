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

#define PORT_PIT_CH0    0x40
#define PORT_PIT_CH1    0x41
#define PORT_PIT_CH2    0x42
#define PORT_PIT_CMD    0x43

#define TIMER_MAX_FREQ  1193182

/**
 * Sets the tick rate of the timer.
 * If the specified rate exceeds TIMER_MAX_FREQ, the rate will be set to
 * TIMER_MAX_FREQ.
 *
 * @param hz - the rate at which to generate timer interrupts in Hz
 */
void timer_set_rate(int hz);

/**
 * Timer tick interrupt handler.
 */
void timer_do_irq(void);

#endif /* __DRIVERS_TIMER_H */
