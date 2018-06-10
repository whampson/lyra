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
 *   File: drivers/video/vga.c
 * Author: Wes Hampson
 *   Desc: VGA driver.
 *----------------------------------------------------------------------------*/

#include <drivers/vga.h>
#include <lyra/kernel.h>
#include <lyra/io.h>

/* VGA ports */
#define PORT_AC_ADDR        0x3C0
#define PORT_AC_DATA_R      0x3C1
#define PORT_AC_DATA_W      0x3C0
#define PORT_INPUTSTATUS1   0x3DA
#define PORT_MISCOUTPUT_R   0x3CC
#define PORT_MISCOUTPUT_W   0x3C2
#define PORT_CRTC_ADDR      0x3D4
#define PORT_CRTC_DATA      0x3D5

/* VGA CRTC (CRT controller) registers */
#define REG_CUR_SCANSTART   0x0A    /* top scan line of cursor */
#define REG_CUR_SCANEND     0x0B    /* bottom scan line of cursor */
#define REG_CUR_POSHI       0x0E    /* upper 8-bits of cursor pos */
#define REG_CUR_POSLO       0x0F    /* lower 8-bits of cursor pos */

/* VGA AC (attribute controller) registers */
#define REG_ATTR_MODECTL    0x10

/* Cursor size parameters
   'START' and 'END' refer to scan lines on the current row. Each row has 16
   scan lines, thus these parameters can range from 0 to 15. */
#define CUR_BLOCK_START     0
#define CUR_BLOCK_END       15
#define CUR_UNDERBAR_START  13
#define CUR_UNDERBAR_END    15

/* Cursor scanline register fields */
#define CUR_SCANLINE        0x1F    /* start/end scanline field mask */
#define CUR_DISABLE         0x20    /* field located in scanstart reg */

/* Misc. output register fields. */
#define IOAS                0x01    /* I/O address select */

/* Attribute address register fields. */
#define ATTR_ADDRESS        0x1F    /* AC register index mask */
#define PAS                 0x20    /* pallette address source (keep set) */

/* Attribute mode control register fields. */
#define BLINK               0x08    /* enable character blink */

void vga_init(void)
{
    /* Set I/O address select field to ensure CRTC ports are 0x3D4 and 0x3D5 */
    outb(inb(PORT_MISCOUTPUT_R) | IOAS, PORT_MISCOUTPUT_W);

    set_cursor_type(CURSOR_UNDERBAR);;
    blink_enable();
}

void hide_cursor(void)
{
    uint8_t data;

    outb(REG_CUR_SCANSTART, PORT_CRTC_ADDR);
    data = inb(PORT_CRTC_DATA);

    data |= CUR_DISABLE;

    outb(REG_CUR_SCANSTART, PORT_CRTC_ADDR);
    outb(data, PORT_CRTC_DATA);
}

void show_cursor(void)
{
    uint8_t data;

    outb(REG_CUR_SCANSTART, PORT_CRTC_ADDR);
    data = inb(PORT_CRTC_DATA);

    data &= ~CUR_DISABLE;

    outb(REG_CUR_SCANSTART, PORT_CRTC_ADDR);
    outb(data, PORT_CRTC_DATA);
}

void set_cursor_type(int type)
{
    uint8_t start;
    uint8_t end;

    switch (type) {
        case CURSOR_UNDERBAR:
            start = CUR_UNDERBAR_START;
            end = CUR_UNDERBAR_END;
            break;

        case CURSOR_BLOCK:
            start = CUR_BLOCK_START;
            end = CUR_BLOCK_END;
            break;

        default:
            return;
    }

    outb(REG_CUR_SCANSTART, PORT_CRTC_ADDR);
    outb(start & CUR_SCANLINE, PORT_CRTC_DATA);
    outb(REG_CUR_SCANEND, PORT_CRTC_ADDR);
    outb(end & CUR_SCANLINE, PORT_CRTC_DATA);
}

uint16_t get_cursor_pos(void)
{
    uint16_t pos;

    outb(REG_CUR_POSHI, PORT_CRTC_ADDR);
    pos = inb(PORT_CRTC_DATA) << 8;

    outb(REG_CUR_POSLO, PORT_CRTC_ADDR);
    pos = (pos & 0xFF00) | inb(PORT_CRTC_DATA);

    return pos;
}

void set_cursor_pos(uint16_t pos)
{
    outb(REG_CUR_POSHI, PORT_CRTC_ADDR);
    outb(pos >> 8, PORT_CRTC_DATA);

    outb(REG_CUR_POSLO, PORT_CRTC_ADDR);
    outb(pos & 0xFF, PORT_CRTC_DATA);
}

void blink_disable(void)
{
    uint8_t data;

    /* Reading this register ensures that the attribute controller interprets
       the next write to PORT_AC_ADDR as a write to the attribute address
       register. */
    (void)inb(PORT_INPUTSTATUS1);

    /* Write modectl register index, keep PAS set; then read modectl register */
    outb(PAS | (REG_ATTR_MODECTL & ATTR_ADDRESS), PORT_AC_ADDR);
    data = inb(PORT_AC_DATA_R);

    data &= ~BLINK;

    /* Same deal as above, except now we're writing the modectl register. */
    (void)inb(PORT_INPUTSTATUS1);
    outb(PAS | (REG_ATTR_MODECTL & ATTR_ADDRESS), PORT_AC_ADDR);
    outb(data, PORT_AC_DATA_W);
}

void blink_enable(void)
{
    uint8_t data;

    /* Reading this register ensures that the attribute controller interprets
       the next write to PORT_AC_ADDR as a write to the attribute address
       register. */
    (void)inb(PORT_INPUTSTATUS1);

    /* Write modectl register index, keep PAS set; then read modectl register */
    outb(PAS | (REG_ATTR_MODECTL & ATTR_ADDRESS), PORT_AC_ADDR);
    data = inb(PORT_AC_DATA_R);

    data |= BLINK;

    /* Same deal as above, except now we're writing the modectl register. */
    (void)inb(PORT_INPUTSTATUS1);
    outb(PAS | (REG_ATTR_MODECTL & ATTR_ADDRESS), PORT_AC_ADDR);
    outb(data, PORT_AC_DATA_W);
}
