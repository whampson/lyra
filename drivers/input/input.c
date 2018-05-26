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

void sendkey(keystroke_t k)
{
    /* TODO:
        - translate keystroke into character(s)
            - the mapping of character(s) sent for keys pressed will happen here
        - send characters to terminal input line
            - tty line discipline will hold input buffer
    */

   /* Carried over from ps2kbd.c... */
    // ch = (char) sc;

    // /* Handle numpad */
    // /* TODO: check/handle numlock */
    // if (handle_numpad(sc)) {
    //     goto irq_done;
    // }

    // /* Handle special keys (home, end, insert, etc.) */
    // if (handle_special(sc)) {
    //     goto irq_done;
    // }

    // /* Handle shift */
    // if (flag_shift) {
    //     tmp = SHIFT_MAP[(int) ch];
    //     if (tmp != 0) {
    //         ch = tmp;
    //     }
    // }

    // /* Handle ctrl chars (C0) */
    // if (flag_ctrl) {
    //     if (ch >= 'a' && ch <= 'z') {
    //         ch -= CAPS_OFFSET;
    //     }
    //     if (ch >= '@' && ch <= '_') {   /* superset of the above */
    //         ch -= C0CHAR_OFFSET;
    //         goto sendchar;
    //     }
    //     goto irq_done;
    // }

    // /* Handle caps lock */
    // if (flag_caps && ch >= 'a' && ch <= 'z') {
    //     ch += (flag_shift) ? CAPS_OFFSET : -CAPS_OFFSET;
    // }
}
