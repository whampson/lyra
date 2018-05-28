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

#include <ctype.h>
#include <stdio.h>
#include <lyra/input.h>

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


static int handle_numpad(scancode_t k);
static int handle_special(scancode_t k);

void sendkey(keystroke_t k)
{
    /* TODO:
        - translate keystroke into character(s)
            - the mapping of character(s) sent for keys pressed will happen here
        - send characters to terminal input line
            - tty line discipline will hold input buffer
    */

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

    if (handle_numpad(sc) || handle_special(sc)) {
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

    /* Handle ctrl chars (C0) */
    if (is_ctrl_down(k)) {
        /* See, the ASCII table was actually very well-crafted: all control
           characters fall between 0x00 and 0x1F, and all "hat notation" control
           characters (i.e. the keys you actually press to enter the control
           character) fall between 0x40 and 0x5F. This means that you simply
           have to subtract 0x40 from the typed character to get the actual
           control character! For instance, ESC is typed as '^[', the '[' is
           0x5B in ASCII. Subtract 0x40 from 0x5B and we get 0x1B, which is the
           code for the ESC control charater! :) */

        /* First though, we must convert all alphabetic characters to uppercase
           since the virtual scancodes for alphabetic characters map to
           lowercase letters. */
        ch = (char) toupper(ch);

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
    putchar(ch);
}

static int handle_numpad(scancode_t sc)
{
    if (!is_numpad_key(sc)) {
        return 0;
    }

    if (sc == KB_RENTER) {
        putchar('\n');
    }
    else {
        putchar(sc - NUMPAD_OFFSET);
    }

    return 1;
}

static int handle_special(scancode_t sc)
{
    if (!is_special_key(sc) && !is_func_key(sc)) {
        return 0;
    }

    //putchar((char) sc);
    return 1;
}
