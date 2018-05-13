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
 * File: kbd.h
 * Desc:
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_KBD_H__
#define __LYRA_KBD_H__

#include <stdint.h>

/* Key IDs for non-ASCII keys. */
#define KB_DELETE   0x7F
#define KB_LCTRL    0x80
#define KB_RCTRL    0x81
#define KB_LSHIFT   0x82
#define KB_RSHIFT   0x83
#define KB_LALT     0x84
#define KB_RALT     0x85
#define KB_CAPS     0x86
#define KB_NUM      0x87
#define KB_SCROLL   0x88
#define KB_PAUSE    0x89
#define KB_PRNTSC   0x8A
#define KB_F1       0x91
#define KB_F2       0x92
#define KB_F3       0x93
#define KB_F4       0x94
#define KB_F5       0x95
#define KB_F6       0x96
#define KB_F7       0x97
#define KB_F8       0x98
#define KB_F9       0x99
#define KB_F10      0x9A
#define KB_F11      0x9B
#define KB_F12      0x9C
#define KB_KP_0     0xA0
#define KB_KP_1     0xA1
#define KB_KP_2     0xA2
#define KB_KP_3     0xA3
#define KB_KP_4     0xA4
#define KB_KP_5     0xA5
#define KB_KP_6     0xA6
#define KB_KP_7     0xA7
#define KB_KP_8     0xA8
#define KB_KP_9     0xA9
#define KB_KP_STAR  0xAA    /* '*' */
#define KB_KP_SLASH 0xAB    /* '/' */
#define KB_KP_PLUS  0xAC    /* '+' */
#define KB_KP_MINUS 0xAD    /* '-' */
#define KB_KP_DOT   0xAE    /* '.' */
#define KB_KP_ENTER 0xAF
#define KB_INSERT   0xB0
#define KB_END      0xB1
#define KB_CUR_D    0xB2
#define KB_PGDN     0xB3
#define KB_CUR_L    0xB4
#define KB_CUR_R    0xB6
#define KB_HOME     0xB7
#define KB_CUR_U    0xB8
#define KB_PGUP     0xB9

typedef struct keycode {
    uint8_t key_id;
    struct keyflags {
        uint8_t pressed : 1;
        uint8_t ctrl    : 1;
        uint8_t shift   : 1;
        uint8_t alt     : 1;
        uint8_t num     : 1;
        uint8_t caps    : 1;
        uint8_t scroll  : 1;
    } flags;
} keycode_t;

void kbd_init(void);
void kbd_enable(void);
void kbd_disable(void);
void kbd_set_scancode(int sc);
void kbd_handle_interrupt(void);

#endif /* __LYRA_KBD_H__ */
