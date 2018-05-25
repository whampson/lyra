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
    KB_NUM0         = 0x80,
    KB_NUM1         = 0x81,
    KB_NUM2         = 0x82,
    KB_NUM3         = 0x83,
    KB_NUM4         = 0x84,
    KB_NUM5         = 0x85,
    KB_NUM6         = 0x86,
    KB_NUM7         = 0x87,
    KB_NUM8         = 0x88,
    KB_NUM9         = 0x89,
    KB_RENTER       = 0x8A,
    KB_ADD          = 0x8B,
    KB_SUBTRACT     = 0x8C,
    KB_MULTIPLY     = 0x8D,
    KB_DIVIDE       = 0x8E,
    KB_DECIMAL      = 0x8F,
    KB_NUMLK        = 0x90,
    KB_CAPLK        = 0x91,
    KB_SCRLK        = 0x92,
    KB_LCTRL        = 0x93,
    KB_RCTRL        = 0x94,
    KB_LSHIFT       = 0x95,
    KB_RSHIFT       = 0x96,
    KB_LALT         = 0x97,
    KB_RALT         = 0x98,
    KB_INSERT       = 0x99,
    KB_HOME         = 0x9A,
    KB_END          = 0x9B,
    KB_PAGEUP       = 0x9C,
    KB_PAGEDN       = 0x9D,
    KB_PRTSC        = 0x9E,
    KB_PAUSE        = 0x9F,
    KB_F1           = 0xA0,
    KB_F2           = 0xA1,
    KB_F3           = 0xA2,
    KB_F4           = 0xA3,
    KB_F5           = 0xA4,
    KB_F6           = 0xA5,
    KB_F7           = 0xA6,
    KB_F8           = 0xA7,
    KB_F9           = 0xA8,
    KB_F10          = 0xA9,
    KB_F11          = 0xAA,
    KB_F12          = 0xAB,
    KB_UP           = 0xAC,
    KB_DOWN         = 0xAD,
    KB_LEFT         = 0xAE,
    KB_RIGHT        = 0xAF
};

#define NUMPAD_OFFSET   0x50

/**
 * Represents a keyboard virtual scancode.
 */
typedef uint8_t scancode_t;

#endif /* __LYRA_INPUT_H__ */
