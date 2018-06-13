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
 *   File: include/lyra/kernel.h
 * Author: Wes Hampson
 *   Desc: Commonly-used and convenient function prototypes and macros.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_CONSOLE_H
#define __LYRA_CONSOLE_H

#include <lyra/tty.h>

#define NUM_CONSOLES    8
#define CON_COLS        80
#define CON_ROWS        25
#define CON_AREA        (CON_COLS * CON_ROWS)   /* total characters */
#define CON_SIZE        (CON_AREA * 2)          /* total bytes */

/**
 * Initializes keyboard and VGA drivers, creates virtual consoles,
 * and switches to console 0.
 */
void console_init(void);

int console_write(struct tty *tty);

void set_console(int num);

#endif /* __LYRA_CONSOLE_H */
