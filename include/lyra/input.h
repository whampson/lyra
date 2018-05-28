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
 *   File: include/lyra/input.h
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_INPUT_H
#define __LYRA_INPUT_H

#include <stdbool.h>
#include <stdint.h>

/**
 * ASCII control characters.
 */
enum ascii {
    ASCII_NUL,  /* '\0' */
    ASCII_SOH,
    ASCII_STX,
    ASCII_ETX,
    ASCII_EOT,
    ASCII_ENQ,
    ASCII_ACK,
    ASCII_BEL,  /* '\a' */
    ASCII_BS,   /* '\b' */
    ASCII_TAB,  /* '\t' */
    ASCII_LF,   /* '\n' */
    ASCII_VT,   /* '\v' */
    ASCII_FF,   /* '\f' */
    ASCII_CR,   /* '\r' */
    ASCII_SO,
    ASCII_SI,
    ASCII_DLE,
    ASCII_DC1,
    ASCII_DC2,
    ASCII_DC3,
    ASCII_DC4,
    ASCII_NAK,
    ASCII_SYN,
    ASCII_ETB,
    ASCII_CAN,
    ASCII_EM,
    ASCII_SUB,
    ASCII_ESC,  /* '\e' */
    ASCII_FS,
    ASCII_GS,
    ASCII_RS,
    ASCII_US,
    ASCII_DEL = 0x7F
};

/**
 * Keyboard virtual scancodes. Each entry corresponds to a physical key on the
 * traditional 101-key US keyboard (IBM Model M).
 *
 * Scancodes for alphabetic keys are equivalent to the ASCII codes for lowercase
 * letters. Similarly, scancodes for symbolic keys are equivalent to the ASCII
 * codes for the symbol typed when shift is not pressed (e.g. the scancode for
 * the '/?' key is 0x2F, the ASCII code for the '/' character). All keys on the
 * numeric keypad have their own scancodes.
 */
enum kb_keys {
    /* Control characters */
    KB_BACKSPACE    = ASCII_BS,
    KB_TAB          = ASCII_TAB,
    KB_ENTER        = ASCII_LF,
    KB_ESCAPE       = ASCII_ESC,

    /* Alphabetic, numeric, and symbolic keys */
    /* (matches ASCII values, see above) */
    /* 0x20 - 0x7E */

    /* "Special" keys (part 1) */
    KB_DELETE       = 0x7F,
    KB_INSERT       = 0x80,
    KB_HOME         = 0x81,
    KB_END          = 0x82,
    KB_PAGEUP       = 0x83,
    KB_PAGEDN       = 0x84,
    KB_UP           = 0x85,
    KB_DOWN         = 0x86,
    KB_RIGHT        = 0x87,
    KB_LEFT         = 0x88,

    /* Number pad */
    /* Subtract NUMPAD_OFFSET to get ASCII values. */
    KB_MULTIPLY     = 0x8A,
    KB_ADD          = 0x8B,
    KB_RENTER       = 0x8C,
    KB_SUBTRACT     = 0x8D,
    KB_DECIMAL      = 0x8E,
    KB_DIVIDE       = 0x8F,
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

    /* Toggle and modifier keys */
    KB_NUMLK        = 0xA0,
    KB_CAPLK        = 0xA1,
    KB_SCRLK        = 0xA2,
    KB_LCTRL        = 0xA3,
    KB_RCTRL        = 0xA4,
    KB_LSHIFT       = 0xA5,
    KB_RSHIFT       = 0xA6,
    KB_LALT         = 0xA7,
    KB_RALT         = 0xA8,

    /* "Special" keys (part 2) */
    KB_PRTSC        = 0xAA,
    KB_PAUSE        = 0xAB,

    /* Function keys */
    KB_F1           = 0xB1,
    KB_F2           = 0xB2,
    KB_F3           = 0xB3,
    KB_F4           = 0xB4,
    KB_F5           = 0xB5,
    KB_F6           = 0xB6,
    KB_F7           = 0xB7,
    KB_F8           = 0xB8,
    KB_F9           = 0xB9,
    KB_F10          = 0xBA,
    KB_F11          = 0xBB,
    KB_F12          = 0xBC
};

/* Useful offsets for converting scancodes to ASCII. */
#define NUMPAD_OFFSET   0x60

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

/**
 * Convert 'struct keystroke' to 'keystroke_t'.
 */
#define encode_keystroke(keystroke_struct)   \
*((keystroke_t *) &keystroke_struct)

/**
 * Convert 'keystroke_t' to 'struct keystroke'.
 */
#define decode_keystroke(keystroke_val)     \
*((struct keystroke *) &keystroke_val)

static inline bool is_ctrl_down(keystroke_t k)
{
    return k & FLAG_CTRL;
}

static inline bool is_shift_down(keystroke_t k)
{
    return k & FLAG_SHIFT;
}

static inline bool is_alt_down(keystroke_t k)
{
    return k & FLAG_ALT;
}

static inline bool is_numlk_on(keystroke_t k)
{
    return k & FLAG_NUMLK;
}

static inline bool is_capslk_on(keystroke_t k)
{
    return k & FLAG_CAPSLK;
}

static inline bool is_scrlk_on(keystroke_t k)
{
    return k & FLAG_SCRLK;
}

static inline bool is_toggle_key(keystroke_t k)
{
    return (k & KEY_ID) >= KB_NUMLK &&
           (k & KEY_ID) <= KB_SCRLK;
}

static inline bool is_modifier_key(keystroke_t k)
{
    return (k & KEY_ID) >= KB_LCTRL &&
           (k & KEY_ID) <= KB_RALT;
}

static inline bool is_func_key(keystroke_t k)
{
    return (k & KEY_ID) >= KB_F1 &&
           (k & KEY_ID) <= KB_F12;
}

static inline bool is_numpad_key(keystroke_t k)
{
    return (k & KEY_ID) >= KB_MULTIPLY &&
           (k & KEY_ID) <= KB_NUM9;
}

static inline bool is_special_key(keystroke_t k)
{
    return ((k & KEY_ID) >= KB_DELETE && (k & KEY_ID) <= KB_LEFT)
        || (k & KEY_ID) == KB_PRTSC
        || (k & KEY_ID) == KB_PAUSE;
}

/**
 * Send a keystroke to the terminal.
 */
void sendkey(keystroke_t k);

#endif /* __LYRA_INPUT_H */
