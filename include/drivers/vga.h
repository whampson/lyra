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
 *   File: include/drivers/vga.h
 * Author: Wes Hampson
 *   Desc: VGA driver interface.
 *----------------------------------------------------------------------------*/

#ifndef __DRIVERS_VGA_H
#define __DRIVERS_VGA_H

#include <stdint.h>

/* Screen dimensions (characters) */
#define SCREEN_X    80
#define SCREEN_Y    25
#define SCREEN_SIZE (SCREEN_X * SCREEN_Y)

enum cursor_type {
    CURSOR_UNDERBAR,
    CURSOR_BLOCK
};

static inline void pos2xy(int pos, int *x, int *y)
{
    *x = pos % SCREEN_X;
    *y = pos / SCREEN_X;
}

static inline int xy2pos(int x, int y)
{
    return y * SCREEN_X + x;
}

void vga_init(void);

void hide_cursor(void);
void show_cursor(void);

void set_cursor_type(int type);

uint16_t get_cursor_pos(void);
void set_cursor_pos(uint16_t pos);

void vga_blink_disable(void);
void vga_blink_enable(void);

#endif /* __DRIVERS_VGA_H */
