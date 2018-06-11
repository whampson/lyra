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
 *   File: include/drivers/pcspk.h
 * Author: Wes Hampson
 *   Desc: PC Speaker driver interface.
 *----------------------------------------------------------------------------*/

#ifndef __DRIVERS_PCSPK_H
#define __DRIVERS_PCSPK_H

/**
 * Sets the PC speaker frequency.
 *
 * @param hz - the PC speaker frequency in Hz.
 */
void pcspk_set_freq(int hz);

/**
 * Causes the PC speaker to emit a tone at the set rate.
 */
void pcspk_on(void);

/**
 * Causes the PC speaker to stop playing.
 */
void pcspk_off(void);

/**
 * Causes the PC speaker to emit a tone for the specified number of ticks.
 * The amount of time between ticks varies depending on the current
 * configuration of PIT channel 0.
 */
void pcspk_beep(unsigned int ticks);

#endif /* __DRIVERS_PCSPK_H */
