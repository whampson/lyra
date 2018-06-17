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
#include <lyra/console.h>
#include <lyra/interrupt.h>

#define NUM_TTY 5

static struct tty tty_table[NUM_TTY] = {
    {
        /* console */
        .termio = {
            .c_iflag = ICRNL,
            .c_oflag = OPOST | ONLCR,
            .c_lflag = ECHO
        },
        .column = 0
    },
    {
        /* COM1 */
        .termio = { 0 },
        .column = 0
    },
    {
        /* COM2 */
        .termio = { 0 },
        .column = 0
    },
    {
        /* COM3 */
        .termio = { 0 },
        .column = 0
    },
    {
        /* COM4 */
        .termio = { 0 },
        .column = 0
    },
};

/**
 * Get next character from TTY's input queue.
 */
static inline char tty_getch(struct tty *tty)
{
    return (char) tty_queue_get(&tty->rd_q);
}

/**
 * Write character to TTY's output queue.
 */
static inline void tty_putch(struct tty *tty, char c)
{
    tty_queue_put(&tty->wr_q, (unsigned char) c);
}

void tty_init(void)
{
    int i;

    for (i = 0; i < NUM_TTY; i++) {
        tty_queue_init(&tty_table[i].rd_q);
        tty_queue_init(&tty_table[i].wr_q);
    }

    tty_table[TTY_CONSOLE].write = console_write;
}

int tty_read(int chan, char *buf, int n)
{
    int count;
    struct tty *tty;

    if (chan < 0 || chan >= NUM_TTY || buf == NULL || n < 0) {
        return -1;
    }

    tty = tty_table + chan;

    count = 0;
    while (!tty->rd_q.empty && count < n) {
        buf[count++] = tty_getch(tty);
    }

    return count;
}

int tty_write(int chan, const char *buf, int n)
{
    int i;
    char c_in;
    char c_out;
    tcflag_t c_oflag;
    struct tty *tty;

    if (chan < 0 || chan >= NUM_TTY || buf == NULL || n < 0) {
        return -1;
    }

    tty = tty_table + chan;
    c_oflag = tty->termio.c_oflag;

    i = 0;
    while (i < n) {
        c_in = buf[i++];
        c_out = c_in;

        if (!flag_set(c_oflag, OPOST)) {
            goto do_write;
        }

        /* CR/LF translation */
        switch (c_in) {
            case ASCII_CR:
                if (flag_set(c_oflag, OCRNL)) {
                    c_out = ASCII_LF;
                    if (flag_set(c_oflag, ONLRET)) {
                        tty->column = 0;
                    }
                }
                break;
            case ASCII_LF:
                if (flag_set(c_oflag, ONLCR)) {
                    tty_putch(tty, ASCII_CR);
                    tty->column = 0;
                }
                if (flag_set(c_oflag, ONLRET)) {
                    tty->column = 0;
                }
                break;
        }

    do_write:
        tty_putch(tty, c_out);
        tty->write(tty);
    }

    return i;
}

void tty_recv(int chan, char c)
{
    struct tty *tty;
    tcflag_t c_iflag;
    tcflag_t c_lflag;
    char c_out;

    if (chan < 0 || chan >= NUM_TTY) {
        return;
    }

    tty = tty_table + chan;
    c_iflag = tty->termio.c_iflag;
    c_lflag = tty->termio.c_lflag;

    if (tty->rd_q.full) {
        return;
    }

    c_out = c;

    /* CR/LF translation */
    switch (c) {
        case ASCII_CR:
            if (flag_set(c_iflag, IGNCR)) {
                return;
            }
            if (flag_set(c_iflag, ICRNL)) {
                c_out = ASCII_LF;
            }
            break;
        case ASCII_LF:
            if (flag_set(c_iflag, INLCR)) {
                c_out = ASCII_CR;
            }
            break;
    }

    /* character echoing */
    if (flag_set(c_lflag, ECHO)) {
        tty_write(chan, &c_out, 1);
    }

    tty_queue_put(&tty->rd_q, c_out);
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
