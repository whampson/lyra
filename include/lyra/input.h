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
 * File: input.h
 * Desc: Keyboard virtual scancode definitons.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_INPUT_H__
#define __LYRA_INPUT_H__

#include <stdint.h>

/**
 * Keyboard virtual scancodes. Each entry corresponds to a physical key on the
 * traditional 101-key US keyboard (IBM Model M).
 *
 * Scancodes for alphabetic keys are equivalent to the ASCII codes for lowercase
 * letters. Similarly, scancodes for symbolic keys are equivalent to the ASCII
 * codes for the symbol typed when shift is not pressed (e.g. the scancode for
 * the '/?' key is 0x2F, the ASCII code for the '/' character). All keys on the
 * numeric keypad have their own scancodes.
 *
 * Virtual key codes derived from the Windows Virtual-Key Codes found here:
 * https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
 */
enum kb_keys {
    KB_BACKSPACE    = 0x08,     /* '\b' */
    KB_TAB          = 0x09,     /* '\t' */
    KB_ENTER        = 0x0D,     /* '\n' */
    KB_ESCAPE       = 0x1B,     /* '\e' */
    KB_SPACE        = 0x20,
    KB_DELETE       = 0x7F,
    KB_INSERT       = 0x80,
    KB_HOME         = 0x81,
    KB_END          = 0x82,
    KB_PAGEUP       = 0x83,
    KB_PAGEDN       = 0x84,
    KB_PRTSC        = 0x85,
    KB_PAUSE        = 0x86,
    KB_UP           = 0x87,
    KB_DOWN         = 0x88,
    KB_LEFT         = 0x89,
    KB_RIGHT        = 0x8A,
    KB_NUM0         = 0x90,
    KB_NUM1         = 0x91,
    KB_NUM2         = 0x92,
    KB_NUM3         = 0x93,
    KB_NUM4         = 0x94,
    KB_NUM5         = 0x95,
    KB_NUM6         = 0x96,
    KB_NUM7         = 0x97,
    KB_NUM8         = 0x98,
    KB_NUM9         = 0x99,
    KB_RENTER       = 0x9A,
    KB_ADD          = 0x9B,
    KB_SUBTRACT     = 0x9C,
    KB_MULTIPLY     = 0x9D,
    KB_DIVIDE       = 0x9E,
    KB_DECIMAL      = 0x9F,
    KB_NUMLK        = 0xA0,
    KB_CAPLK        = 0xA1,
    KB_SCRLK        = 0xA2,
    KB_LCTRL        = 0xA3,
    KB_RCTRL        = 0xA4,
    KB_LSHIFT       = 0xA5,
    KB_RSHIFT       = 0xA6,
    KB_LALT         = 0xA7,
    KB_RALT         = 0xA8,
    KB_F1           = 0xB0,
    KB_F2           = 0xB1,
    KB_F3           = 0xB2,
    KB_F4           = 0xB3,
    KB_F5           = 0xB4,
    KB_F6           = 0xB5,
    KB_F7           = 0xB6,
    KB_F8           = 0xB7,
    KB_F9           = 0xB8,
    KB_F10          = 0xB9,
    KB_F11          = 0xBA,
    KB_F12          = 0xBB
};

/* Useful offsets for converting scancodes to ASCII */
#define NUMPAD_OFFSET   0x60
#define C0CHAR_OFFSET   0x40
#define CAPS_OFFSET     0x20

/**
 * Represents a keyboard virtual scancode.
 */
typedef uint16_t scancode_t;

/**
 * Represents a keyboard event.
 */
struct keystroke {
    scancode_t key_id;              /* virtual key id */
    uint16_t flag_keypress  : 1;    /* key is pressed */
    uint16_t flag_ctrl      : 1;    /* ctrl is pressed */
    uint16_t flag_shift     : 1;    /* shift is pressed */
    uint16_t flag_alt       : 1;    /* alt is pressed */
    uint16_t flag_numlk     : 1;    /* num lock is on */
    uint16_t flag_capslk    : 1;    /* caps lock is on */
    uint16_t flag_scrlk     : 1;    /* scroll lock is on */
};

/**
 * Represents a keyboard event.
 * The same data is represented as in 'struct keypress', but encoded as an
 * aggregate value.
 */
typedef uint32_t keystroke_t;

/* Masks for accessing fields in 'keystroke_t' */
#define KEY_ID          0xFFFF
#define FLAG_KEYPRESS   (1 << 16)
#define FLAG_CTRL       (1 << 17)
#define FLAG_SHIFT      (1 << 18)
#define FLAG_ALT        (1 << 19)
#define FLAG_NUMLK      (1 << 20)
#define FLAG_CAPSLK     (1 << 21)
#define FLAG_SCRLK      (1 << 21)

#endif /* __LYRA_INPUT_H__ */
