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
 *   File: include/termios.c
 * Author: Wes Hampson
 *   Desc: Partial implementation of POSIX standard.
 *----------------------------------------------------------------------------*/

#ifndef __TERMIOS_H
#define __TERMIOS_H

#include <stdint.h>

#define NCCS    11      /* TODO: set NCCS */

typedef unsigned char cc_t;
typedef uint16_t speed_t;
typedef uint32_t tcflag_t;

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    // tcflag_t c_cflag;
    tcflag_t c_lflag;
    // cc_t cc_c[NCCS];
};

enum iflag {
    // BRKINT  = (1 << 0),
    ICRNL   = (1 << 1),
    // IGNBRK  = (1 << 2),
    IGNCR   = (1 << 3),
    INLCR   = (1 << 4)
    /* Incomplete... */
};

enum oflag {
    OPOST   = (1 << 0),
    ONLCR   = (1 << 1),
    OCRNL   = (1 << 2),
    ONLRET  = (1 << 3)
    /* Incomplete... */
};

enum lflag {
    ECHO    = (1 << 0),
    // ECHOE   = (1 << 1),
    // ECHOK   = (1 << 2),
    // ECHONL  = (1 << 3),
    // ICANON  = (1 << 4),
    // IEXTEN  = (1 << 5),
    // ISIG    = (1 << 6)
};


#endif /* __TERMIOS_H */
