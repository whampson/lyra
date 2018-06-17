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
#include <termios.h>

/* TTY channels */
#define TTY_CONSOLE         0
#define TTY_COM1            1
#define TTY_COM2            2
#define TTY_COM3            3
#define TTY_COM4            4

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
    struct termios termio;
    struct tty_queue rd_q;
    struct tty_queue wr_q;
    int (*write)(struct tty *tty);
    int column;
};

/**
 * Initialize TTY structs.
 */
void tty_init(void);

/**
 * Read data from a TTY's input buffer.
 */
int tty_read(int chan, char *buf, int n);

/**
 * Write data to a TTY's input buffer.
 */
int tty_write(int chan, const char *buf, int n);

void tty_recv(int chan, char c);

/**
 * Set up new a new tty_queue.
 */
void tty_queue_init(struct tty_queue *q);

/**
 * Get a character from a tty_queue.
 */
unsigned char tty_queue_get(struct tty_queue *q);

/**
 * Put a character in a tty_queue.
 */
void tty_queue_put(struct tty_queue *q, unsigned char c);

#endif /* __LYRA_TTY_H */
