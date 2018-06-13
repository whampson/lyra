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
 *   File: drivers/input/input.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

/* TODO: send typed char to input buffer before console */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <lyra/input.h>
#include <lyra/console.h>
#include <lyra/kernel.h>

/* Use this to convert control character "caret notation" to the actual
   control character. */
#define C0CHAR_OFFSET   0x40

/* Use these to correctly index the 'ESC_SEQUENCES' array. */
#define SPECIAL1_OFFSET 0x7F
#define SPECIAL2_OFFSET 0xA0
#define FUNC_OFFSET     0xA5

/**
 * Mapping of "unshifted" ASCII characters to "shifted" ASCII characters.
 */
const char SHIFT_MAP[256] =
{
/*00-0F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*10-1F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*20-2F*/  0,0,0,0,0,0,0,'"',0,0,0,0,'<','_','>','?',
/*30-3F*/  ')','!','@','#','$','%','^','&','*','(',0,':',0,'+',0,0,
/*40-4F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*50-5F*/  0,0,0,0,0,0,0,0,0,0,0,'{','|','}',0,0,
/*60-6F*/  '~','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
/*70-7F*/  'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
/*80-8F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*90-9F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*A0-AF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*B0-BF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*C0-CF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*D0-DF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*E0-EF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*F0-FF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/**
 * ANSI escape sequences for non-character keys.
 */
const char * const ESC_SEQUENCES[24] =
{
    /* "Special" keys (part 1) */
    "\x1B[3~",      /* Del */
    "\x1B[2~",      /* Ins */
    "\x1B[1~",      /* Home */
    "\x1B[4~",      /* End */
    "\x1B[5~",      /* PgUp */
    "\x1B[6~",      /* PgDn */
    "\x1B[A",       /* Up */
    "\x1B[B",       /* Down */
    "\x1B[C",       /* Right  */
    "\x1B[D",       /* Left */

    /* "Special" keys (part 2) */
    NULL,           /* PrntSc (does this have an esc seq?) */
    "\x1B[P",       /* Pause */ /* TODO: rethink this seq, translates to "delete char" */

    /* Function keys */
    "\x1B[11~",     /* F1 */
    "\x1B[12~",     /* F2 */
    "\x1B[13~",     /* F3 */
    "\x1B[14~",     /* F4 */
    "\x1B[15~",     /* F5 */
    "\x1B[17~",     /* F6 */
    "\x1B[18~",     /* F7 */
    "\x1B[19~",     /* F8 */
    "\x1B[20~",     /* F9 */
    "\x1B[21~",     /* F10 */
    "\x1B[23~",     /* F11 */
    "\x1B[24~",     /* F12 */
};

static bool handle_numpad(scancode_t k);
static bool handle_nonchar(scancode_t k);

void sendkey(keystroke_t k)
{
    struct keystroke keystroke;
    char ch, tmp;
    scancode_t sc;

    keystroke = decode_keystroke(k);
    sc = keystroke.key_id;

    if (!keystroke.flag_keypress) {
        /* Ignore key release events for now... */
        return;
    }

    /* Ignore direct presses of ctrl, shift, alt, numlk, capslk, scrlk */
    if (is_modifier_key(sc) || is_toggle_key(sc)) {
        return;
    }

    /* Send the correct ASCII value for the numpad keys. */
    if (handle_numpad(sc)) {
        return;
    }

    /* Handle keys that do not represent characters (ins., home, end, etc.) */
    if (handle_nonchar(sc)) {
        return;
    }

    ch = (char) sc;

    /* Handle shift */
    if (is_shift_down(k)) {
        tmp = SHIFT_MAP[sc];
        if (tmp != 0) {
            ch = tmp;
        }
    }

    /* Handle ctrl chars (C0 set) */
    if (is_ctrl_down(k)) {
        /* See, the ASCII table was actually very well-crafted: all control
           characters fall between 0x00 and 0x1F, and all "caret notation"
           control characters (i.e. the keys you actually press to enter the
           control character) fall between 0x40 and 0x5F. This means that you
           simply have to subtract 0x40 from the typed character to get the
           actual control character! For instance, ESC is typed as '^[', the '['
           is 0x5B in ASCII. Subtract 0x40 from 0x5B and we get 0x1B, which is
           the code for the ESC control charater! :) */

        /* First though, we must convert all alphabetic characters to uppercase
           since the virtual scancodes for alphabetic characters map to
           lowercase letters. */
        ch = (char) toupper(ch);

        /* Special case for ^? */
        if (ch == '?') {
            ch = ASCII_DEL;
            goto sendchar;
        }

        /* Do nothing for non-control characters. */
        if (ch < '@' || ch > '_') {
            return;
        }

        /* Ok, now we can subtract the magic offset and transmit! */
        ch -= C0CHAR_OFFSET;
        goto sendchar;
    }

    /* Handle caps lock */
    if (is_capslk_on(k)) {
        ch = is_shift_down(k)
           ? (char) tolower(ch) : (char) toupper(ch);
    }

sendchar:
    tty_write(&sys_tty, &ch, 1);
}

static bool handle_numpad(scancode_t sc)
{
    char ch;

    if (!is_numpad_key(sc)) {
        return false;
    }

    if (sc == KB_RENTER) {
        ch = ASCII_LF;
    }
    else {
        /* Another example of the niceties of the ASCII table.
           Use our special offset to get the ASCII value. */
        ch = sc - NUMPAD_OFFSET;
    }

    tty_write(&sys_tty, &ch, 1);

    return true;
}

static bool handle_nonchar(scancode_t sc)
{
    int isfunc, isspecial, isspecial_set2;
    char *seq;

    isfunc = is_func_key(sc);
    isspecial = is_special_key(sc);
    isspecial_set2 = (sc == KB_PRTSC || sc == KB_PAUSE);

    if (!isfunc && !isspecial) {
        return false;
    }

    if (isspecial) {
        sc -= (isspecial_set2) ? SPECIAL2_OFFSET : SPECIAL1_OFFSET;
    }
    else {
        sc -= FUNC_OFFSET;
    }

    seq = ESC_SEQUENCES[sc];
    tty_write(&sys_tty, seq, strlen(seq));  /* TODO: put into read buf */

    return true;
}
