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

#define VGA_FRAMEBUF    0xB8000

enum vga_color {
    VGA_BLK,
    VGA_BLU,
    VGA_GRN,
    VGA_CYN,
    VGA_RED,
    VGA_MGT,
    VGA_BRN,
    VGA_WHT,
    VGA_GRY,
    VGA_BBLU,
    VGA_BGRN,
    VGA_BCYN,
    VGA_BRED,
    VGA_BMGT,
    VGA_YLW,
    VGA_BWHT
};

enum cursor_type {
    CURSOR_UNDERBAR,
    CURSOR_BLOCK
};

union vga_attr {
    struct {
        uint8_t fg : 4;     /* foreground color */
        uint8_t bg : 4;     /* background color; high bit is blink field */
    };
    uint8_t value;
};

union vga_cell {
    struct {
        uint8_t ch;
        union vga_attr attr;
    };
    uint16_t value;
};

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
void blink_disable(void);

/**
 * Enables character blinking.
 * When disabled, bit 7 of the attribute byte is used to select the
 * background color.
 */
void blink_enable(void);

#endif /* __DRIVERS_VGA_H */
