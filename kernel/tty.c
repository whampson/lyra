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
 *   File: kernel/tty.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#include <string.h>
#include <lyra/tty.h>
#include <lyra/input.h>
#include <lyra/interrupt.h>

static inline void putch(struct tty *tty, char c)
{
    tty_queue_put(&tty->write_buf, c);
}

struct tty sys_tty = {
    .console = 0,
    .termio = {
        .c_iflag = ICRNL,
        .c_oflag = OPOST | ONLCR,
        .c_lflag = ECHO
    }
};

void tty_init(void)
{
    tty_queue_init(&sys_tty.read_buf);
    tty_queue_init(&sys_tty.write_buf);
    sys_tty.write = console_write;
}

int tty_read(struct tty *tty, char *buf, int n)
{
    int count;
    char c;
    tcflag_t c_iflag;
    tcflag_t c_lflag;

    if (tty == NULL || buf == NULL || n < 0) {
        return -1;
    }

    c_iflag = tty->termio.c_iflag;
    c_lflag = tty->termio.c_lflag;

    /* Block until a char is input */
    while (tty->read_buf.empty);

    count = 0;
    while (!tty->read_buf.empty /*&& count < n*/) {
        c = tty_queue_get(&tty->read_buf);

        /* CR/LF translation */
        if (!flag_set(c_iflag, IGNCR)) {
            if (c == ASCII_LF && flag_set(c_iflag, INLCR)) {
                c = ASCII_CR;
            }
            else if (c == ASCII_CR && flag_set(c_iflag, ICRNL)) {
                c = ASCII_LF;
            }
        }

        /* character echoing */
        if (flag_set(c_lflag, ECHO)) {
            tty_write(tty, &c, 1);
            tty_flush(tty);
        }

        buf[count++] = c;
    }

    return count;
}

int tty_write(struct tty *tty, const char *buf, int n)
{
    int i;
    char c;
    tcflag_t c_oflag;

    if (tty == NULL || buf == NULL || n < 0) {
        return -1;
    }

    c_oflag = tty->termio.c_oflag;

    i = 0;
    while (i < n) {
        if (tty->write_buf.full) {
            tty_flush(tty);
        }
        c = buf[i++];

        if (!flag_set(c_oflag, OPOST)) {
            putch(tty, c);
            continue;
        }

        if (flag_set(c_oflag, ONLCR) && c == ASCII_LF) {
            putch(tty, ASCII_CR);
        }
        if (flag_set(c_oflag, OCRNL) && c == ASCII_CR) {
            c = ASCII_LF;
        }

        putch(tty, c);
    }

    return i;
}

void tty_flush(struct tty *tty)
{
    if (tty == NULL) {
        return;
    }

    (void) tty->write(tty);
}

void tty_queue_init(struct tty_queue *q)
{
    memset(q->data, 0, TTY_QUEUE_BUFLEN);
    q->head = 0;
    q->tail = 0;
    q->len = 0;
    q->empty = true;
    q->full = false;
}

unsigned char tty_queue_get(struct tty_queue *q)
{
    unsigned char c;

    /* NOTE: always check if the queue is empty first.
       You will get back the wrong value if you don't. */

    if (q == NULL || q->empty) {
        return 0;
    }

    c = q->data[q->tail++];
    q->full = false;
    q->len--;

    if (q->tail >= TTY_QUEUE_BUFLEN) {
        q->tail = 0;
    }

    if (q->tail == q->head) {
        q->empty = true;
    }

    return c;
}

void tty_queue_put(struct tty_queue *q, unsigned char c)
{
    if (q == NULL || q->full) {
        return;
    }

    q->data[q->head++] = c;
    q->empty = false;
    q->len++;

    if (q->head >= TTY_QUEUE_BUFLEN) {
        q->head = 0;
    }

    if (q->head == q->tail) {
        q->full = true;
    }
}
