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

/* Screen dimensions (in terms of characters) */
#define SCREEN_X    80
#define SCREEN_Y    25
#define SCREEN_SIZE (SCREEN_X * SCREEN_Y)

/**
 * Cursor shapes.
 */
enum cursor_type {
    CURSOR_UNDERBAR,
    CURSOR_BLOCK
};

/**
 * Converts a 1-D screen corrdinate to a 2-D screen coordinate.
 *
 * @param pos - 1-D coordinate to convert
 * @param x   - pointer to store x-component
 * @param y   - pointer to store y-component
 */
static inline void pos2xy(int pos, int *x, int *y)
{
    *x = pos % SCREEN_X;
    *y = pos / SCREEN_X;
}

/**
 * Converts a 2-D screen corrdinate to a 1-D screen coordinate.
 *
 * @param x - the x-component of the coordinate to convert
 * @param y - the y-component of the coordinate to convert
 * @return the 1-D coordinate
 */
static inline int xy2pos(int x, int y)
{
    return y * SCREEN_X + x;
}

/**
 * Set VGA driver to defaults.
 */
void vga_init(void);

/**
 * Hides the blinking cursor.
 */
void hide_cursor(void);

/**
 * Shows the blinking cursor.
 */
void show_cursor(void);

/**
 * Sets the cursor shape.
 *
 * Shapes:
 *   CURSOR_UNDERBAR (default)
 *   CURSOR_BLOCK
 *
 * @param type - the cursor shape
 */
void set_cursor_type(int type);

/**
 * Gets the current cursor position as a 1-D coordinate.
 * The top-left of the screen corresponds to 0.
 *
 * @return current cursor position
 */
uint16_t get_cursor_pos(void);

/**
 * Sets the current cursor position using a 1-D coordinate.
 * The top-left of the screen corresponds to 0.
 *
 * @param pos - new cursor position
 */
void set_cursor_pos(uint16_t pos);

/**
 * Enables character blinking.
 * When enabled, bit 7 of the attribute byte toggles blinking.
 */
void vga_blink_disable(void);

/**
 * Enables character blinking.
 * When disabled, bit 7 of the attribute byte is used to select the
 * background color.
 */
void vga_blink_enable(void);

#endif /* __DRIVERS_VGA_H */
