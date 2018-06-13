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
 *   File: include/lyra/tty.h
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_TTY_H
#define __LYRA_TTY_H

#include <stdbool.h>
#include <stddef.h>

#define TTY_QUEUE_BUFLEN    128

struct tty_queue {
    unsigned char data[TTY_QUEUE_BUFLEN];
    int head;
    int tail;
    int len;
    bool empty;
    bool full;
};

struct tty {
    int console;
    struct tty_queue read_buf;
};

/* tty_queue functions */
void tty_queue_init(struct tty_queue *q);
unsigned char tty_queue_get(struct tty_queue *q);
void tty_queue_put(struct tty_queue *q, unsigned char c);


#endif /* __LYRA_TTY_H */