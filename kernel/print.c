#include <kernel.h>

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
 * File: print.c
 * Desc: Basic screen-printing routines.
 *----------------------------------------------------------------------------*/

/* VGA colors */
#define BG_BLACK        0x00
#define FG_LIGHT_GRAY   0x07

/* VGA text mode properties */
#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25
#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

#define XY2POS(x, y)    (y * SCREEN_WIDTH + x)

struct coord2d {
    short x;
    short y;
};

static char * const VIDMEM = (char *) 0xb8000;
static struct coord2d cursor = { 0 };

void putc(char c)
{
    char do_newline = 0;

    switch (c) {
        case '\n':
            do_newline = 1;
            break;
        case '\r':
            cursor.x = 0;
            break;
        default:
            VIDMEM[XY2POS(cursor.x, cursor.y) * 2] = c;
            cursor.x++;
            if (cursor.x >= SCREEN_WIDTH) {
                do_newline = 1;
            }
            break;
    }

    if (do_newline) {
        cursor.x = 0;
        cursor.y++;
    }
    if (cursor.y >= SCREEN_HEIGHT) {
        cursor.y = 0;
    }
}

void puts(const char *s)
{
    while (*s != '\0') {
        putc(*s);
        s++;
    }
}

void clear(void)
{
    int i;
    for (i = 0; i < SCREEN_SIZE; i++) {
        VIDMEM[i * 2] = ' ';
        VIDMEM[i * 2 + 1] = BG_BLACK | FG_LIGHT_GRAY;
    }
}
