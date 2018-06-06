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
 *   File: kernel/console.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#include <stdbool.h>
#include <string.h>
#include <lyra/console.h>
#include <lyra/input.h>
#include <drivers/vga.h>
#include <drivers/ps2kbd.h>

struct console {
    bool initialized;
    bool active;
    short cursor_x;
    short cursor_y;
    char cursor_type;
    bool cursor_hidden;
    struct vga_attr color;
    char *framebuf;
    char tab_width;
    char bs_char;
};

static struct console cons[NUM_CONSOLES];
static int curr_cons;

/* Stole this idea from Linux, very convenient! */
#define m_initialized       (cons[curr_cons].initialized)
#define m_active            (cons[curr_cons].active)
#define m_cursor_x          (cons[curr_cons].cursor_x)
#define m_cursor_y          (cons[curr_cons].cursor_y)
#define m_color             (cons[curr_cons].color)
#define m_cursor_type       (cons[curr_cons].cursor_type)
#define m_cursor_hidden     (cons[curr_cons].cursor_hidden)
#define m_framebuf          (cons[curr_cons].framebuf)
#define m_tab_width         (cons[curr_cons].tab_width)
#define m_bs_char           (cons[curr_cons].bs_char)

static void console_defaults(void);
static void switch_console(int old_cons, int new_cons);
static void backspace(void);
static void tab(void);
static void carriage_return(void);
static void newline(void);

/**
 * Converts a 1-D screen corrdinate to a 2-D screen coordinate.
 *
 * @param pos - 1-D coordinate to convert
 * @param x   - pointer to store x-component
 * @param y   - pointer to store y-component
 */
static inline void pos2xy(int pos, short *x, short *y)
{
    *x = pos % CON_COLS;
    *y = pos / CON_COLS;
}

/**
 * Converts a 2-D screen corrdinate to a 1-D screen coordinate.
 *
 * @param x - the x-component of the coordinate to convert
 * @param y - the y-component of the coordinate to convert
 * @return the 1-D coordinate
 */
static inline int xy2pos(short x, short y)
{
    return y * CON_COLS + x;
}

void console_init(void)
{
    uint16_t pos;

    ps2kbd_init();
    vga_init();

    memset(cons, 0, sizeof(cons));

    curr_cons = 0;
    console_defaults();
    pos = get_cursor_pos();
    pos2xy(pos, &m_cursor_x, &m_cursor_y);
    m_initialized = 1;
    m_active = 1;
}

int console_puts(const char *s)
{
    int i;
    char c;

    i = 0;
    while ((c = s[i]) != '\0') {
        console_putchar(c);
        i++;
    }

    return i;
}

void console_putchar(char c)
{
    int pos;

    switch (c) {
        case ASCII_LF:
            newline();
            goto move_cursor;
        case ASCII_CR:
            carriage_return();
            goto move_cursor;
        case ASCII_TAB:
            tab();
            goto move_cursor;
        case ASCII_BS:
            backspace();
            goto move_cursor;
    }

    pos = xy2pos(m_cursor_x, m_cursor_y);
    m_framebuf[pos * 2] = c;    /* TODO: color */
    m_cursor_x++;
    if (m_cursor_x >= CON_COLS) {
        newline();
    }

move_cursor:
    set_cursor_pos(xy2pos(m_cursor_x, m_cursor_y));
}

static void backspace(void)
{
    int pos;

    m_cursor_x--;
    if (m_cursor_x < 0) {
        m_cursor_x = CON_COLS - 1;
        m_cursor_y--;
    }
    if (m_cursor_y < 0) {
        m_cursor_x = 0;
        m_cursor_y = 0;
    }

    pos = xy2pos(m_cursor_x, m_cursor_y);
    m_framebuf[pos * 2] = m_bs_char;
}

static void tab(void)
{
    char tmp;

    tmp = m_cursor_x % m_tab_width;
    if (tmp == 0) {
        m_cursor_x += m_tab_width;
    }
    else {
        m_cursor_x += (m_tab_width - tmp);
    }
    if (m_cursor_x >= CON_COLS) {
        m_cursor_x = CON_COLS - 1;
    }
}

static void newline(void)
{
    m_cursor_x = 0;
    m_cursor_y++;
    if (m_cursor_y >= CON_ROWS) {
        m_cursor_y = 0;
    }
}

static void carriage_return(void)
{
    m_cursor_x = 0;
}

void set_console(int num)
{
    int old_cons;

    if (num < 0 || num >= NUM_CONSOLES) {
        return;
    }

    old_cons = curr_cons;
    curr_cons = num;

    if (!m_initialized) {
        console_defaults();
        m_initialized = 1;
    }

    if (!m_active) {
        switch_console(old_cons, curr_cons);
    }
}

static void console_defaults(void)
{
    m_color.bg = VGA_BLK;
    m_color.fg = VGA_GRY;
    m_framebuf = (char *) VGA_FRAMEBUF;
    m_cursor_x = 0;
    m_cursor_y = 0;
    m_cursor_type = CURSOR_BLOCK;
    m_cursor_hidden = false;
    m_tab_width = 8;
    m_bs_char = ' ';
}

static void switch_console(int old_cons, int new_cons)
{
    /* TODO switch video memory paging*/

    set_cursor_pos(xy2pos(m_cursor_x, m_cursor_y));
    set_cursor_type(m_cursor_type);
    if (m_cursor_hidden) {
        hide_cursor();
    }
    else {
        show_cursor();
    }
    cons[old_cons].active = 0;
    cons[new_cons].active = 1;
}