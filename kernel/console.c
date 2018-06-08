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

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <lyra/console.h>
#include <lyra/input.h>
#include <lyra/io.h>
#include <lyra/kernel.h>
#include <drivers/vga.h>
#include <drivers/ps2kbd.h>

#define PIT_CLK     1193182
#define BEL_FREQ    880     /* 880Hz = A5, if you're curious :) */

#define CSI_MAX_PARAMS  2
#define PARAM_DEFAULT   (-1)

enum state {
    S_NORMAL,
    S_ESC,
    S_CSI,
};

struct console {
    bool initialized;
    bool active;
    int state;
    short cursor_x;
    short cursor_y;
    char cursor_type;
    bool cursor_hidden;
    union vga_attr color;
    union vga_cell *vidmem;
    char tab_width;
    char bs_char;
    char csiparam[2];
    int paramidx;
};

static struct console cons[NUM_CONSOLES];
static int curr_cons;

/* Stole this idea from Linux, very convenient! */
#define m_initialized       (cons[curr_cons].initialized)
#define m_active            (cons[curr_cons].active)
#define m_state             (cons[curr_cons].state)
#define m_cursor_x          (cons[curr_cons].cursor_x)
#define m_cursor_y          (cons[curr_cons].cursor_y)
#define m_color             (cons[curr_cons].color)
#define m_cursor_type       (cons[curr_cons].cursor_type)
#define m_cursor_hidden     (cons[curr_cons].cursor_hidden)
#define m_vidmem            (cons[curr_cons].vidmem)
#define m_tab_width         (cons[curr_cons].tab_width)
#define m_bs_char           (cons[curr_cons].bs_char)
#define m_csiparam          (cons[curr_cons].csiparam)
#define m_paramidx          (cons[curr_cons].paramidx)

static void console_defaults(void);
static void switch_console(int old_cons, int new_cons);
static void handle_esc(char c);
static void handle_csi(char c);
static void scroll_up(int n);
static void scroll_down(int n);
static void backspace(void);
static void tab(void);
static void carriage_return(void);
static void linefeed(void);
static void bell(void);
static void cursor_up(int n);
static void cursor_down(int n);
static void cursor_right(int n);
static void cursor_left(int n);
static void csi_m(void);

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
    m_color.fg = VGA_WHT;
    m_vidmem = (union vga_cell *) VGA_FRAMEBUF;
    m_cursor_x = 0;
    m_cursor_y = 0;
    m_cursor_type = CURSOR_BLOCK;
    m_cursor_hidden = false;
    m_tab_width = 8;
    m_bs_char = ' ';
    m_state = S_NORMAL;
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

    if (iscntrl(c)) {
        goto ctrl_char;
    }

    switch (m_state)
    {
        case S_ESC:
            handle_esc(c);
            return;

        case S_CSI:
            handle_csi(c);
            goto move_cursor;

    ctrl_char:
        default:
            switch (c)
            {
                case ASCII_BEL:
                    bell();
                    return;
                case ASCII_BS:
                    backspace();
                    goto move_cursor;
                case ASCII_HT:
                    tab();
                    goto move_cursor;
                case ASCII_LF:
                case ASCII_VT:
                case ASCII_FF:
                    /* TODO: technically this should only go to the next line
                       and not do a cr */
                    linefeed();
                    goto move_cursor;
                case ASCII_CR:
                    carriage_return();
                    goto move_cursor;
                case ASCII_ESC:
                    m_state = S_ESC;
                    return;
                default:
                    if (iscntrl(c)) {
                        return;
                    }
                    break;
            }
    }

    pos = xy2pos(m_cursor_x, m_cursor_y);
    m_vidmem[pos].ch = c;
    m_vidmem[pos].attr = m_color;
    m_cursor_x++;
    if (m_cursor_x >= CON_COLS) {
        linefeed();
    }

move_cursor:
    pos = xy2pos(m_cursor_x, m_cursor_y);
    m_vidmem[pos].attr = m_color;               /* TODO: temp */
    set_cursor_pos(xy2pos(m_cursor_x, m_cursor_y));
}

static void handle_esc(char c)
{
    switch (c) {
        case '[':
            m_state = S_CSI;
            m_csiparam[0] = PARAM_DEFAULT;
            m_csiparam[1] = PARAM_DEFAULT;
            m_paramidx = 0;
            return;
        default:
            m_state = S_NORMAL;
            return;
    }
}

static void handle_csi(char c)
{
    switch (c) {
        case 'A':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_up(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'B':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_down(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'C':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_right(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'D':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_left(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'E':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_down(m_csiparam[0]);
            m_cursor_x = 0;
            m_state = S_NORMAL;
            break;
        case 'F':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_up(m_csiparam[0]);
            m_cursor_x = 0;
            m_state = S_NORMAL;
            break;
        case 'G':
            if (m_csiparam[0] < 1) {
                m_csiparam[0] = 1;
            }
            else if (m_csiparam[0] > CON_COLS) {
                m_csiparam[0] = CON_COLS;
            }
            m_cursor_x = m_csiparam[0] - 1;
            m_state = S_NORMAL;
            break;
        case 'H':
        case 'f':
            if (m_csiparam[0] < 1) {
                m_csiparam[0] = 1;
            }
            else if (m_csiparam[0] > CON_ROWS) {
                m_csiparam[0] = CON_ROWS;
            }
            if (m_csiparam[1] < 1) {
                m_csiparam[1] = 1;
            }
            else if (m_csiparam[1] > CON_COLS) {
                m_csiparam[1] = CON_COLS;
            }
            m_cursor_y = m_csiparam[0] - 1;
            m_cursor_x = m_csiparam[1] - 1;
            m_state = S_NORMAL;
            break;
        // case 'J':
        // case 'K':
        case 'S':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            scroll_up(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'T':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            scroll_down(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'm':
            csi_m();
            m_state = S_NORMAL;
            break;
        case ';':
            if (++m_paramidx >= CSI_MAX_PARAMS) {
                m_state = S_NORMAL;
            }
            break;
        default:
            if (isdigit(c)) {
                if (m_csiparam[m_paramidx] == PARAM_DEFAULT) {
                    m_csiparam[m_paramidx] = 0;
                }
                m_csiparam[m_paramidx] *= 10;
                m_csiparam[m_paramidx] += (c - '0');
            }
            else if (c >= 0x40 && c <= 0x7E) {
                m_state = S_NORMAL;
            }
            break;
    }
}

static void scroll_up(int n)
{
    if (n <= 0) {
        return;
    }

    int n_cells;            /* number of screen cells to move off-screen */
    int n_bytes;            /* number of bytes to move back in buffer */
    int blank_start;        /* start offset of cells to blank */
    union vga_cell cell;
    int i;

    /* Limit n to the total number of rows */
    if (n >= CON_ROWS) {
        n = CON_ROWS;
    }

    /* Compute number of cells to erase from the top of the buffer.
       This is also the number of blank spaces at the end of the buffer. */
    n_cells = n * CON_COLS;

    /* Compute the start offset of the cells to blank */
    blank_start = CON_AREA - n_cells;

    /* Compute the number of bytes to move in the buffer (2 bytes per cell) */
    n_bytes = blank_start * sizeof(uint16_t);

    /* Give the impression of scrolling by moving characters back by n_cells */
    memmove(m_vidmem, &(m_vidmem[n_cells]), n_bytes);

    /* Blank-out the rest of the terminal, preserve the attribute */
    cell.attr = m_color;
    cell.ch = m_bs_char;
    for (i = 0; i < n_cells; i++) {
        m_vidmem[blank_start + i] = cell;
    }
}

static void scroll_down(int n)
{
    if (n <= 0) {
        return;
    }

    int n_cells;
    int n_bytes;
    union vga_cell cell;
    int i;

    if (n >= CON_ROWS) {
        n = CON_ROWS;
    }

    n_cells = n * CON_COLS;
    n_bytes = (CON_AREA - n_cells) * sizeof(uint16_t);
    memmove(&(m_vidmem[n_cells]), m_vidmem, n_bytes);

    cell.attr = m_color;
    cell.ch = m_bs_char;
    for (i = 0; i < n_cells; i++) {
        m_vidmem[i] = cell;
    }
}

static void backspace(void)
{
    int pos;

    pos = xy2pos(m_cursor_x, m_cursor_y);
    if (pos == 0) {
        return;
    }

    m_vidmem[--pos].ch = m_bs_char;
    pos2xy(pos, &m_cursor_x, &m_cursor_y);
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

static void linefeed(void)
{
    m_cursor_x = 0;
    m_cursor_y++;

    if (m_cursor_y >= CON_ROWS) {
        scroll_up(1);
        m_cursor_y--;
        m_cursor_x = 0;
    }
}

static void carriage_return(void)
{
    m_cursor_x = 0;
}

static void bell(void)
{
    /* TODO: */
}

static void cursor_up(int n)
{
    m_cursor_y -= n;
    if (m_cursor_y < 0) {
        m_cursor_y = 0;
    }
}

static void cursor_down(int n)
{
    m_cursor_y += n;
    if (m_cursor_y >= CON_ROWS) {
        m_cursor_y = CON_ROWS - 1;
    }
}

static void cursor_right(int n)
{
    m_cursor_x += n;
    if (m_cursor_x >= CON_COLS) {
        m_cursor_x = CON_COLS - 1;
    }
}

static void cursor_left(int n)
{
    m_cursor_x -= n;
    if (m_cursor_x < 0) {
        m_cursor_x = 0;
    }
}

static void csi_m(void)
{
    int i;
    int p;

    i = 0;
    while (i < CSI_MAX_PARAMS) {
        p = m_csiparam[i++];
        if (p == 7) {
            m_color.fg ^= m_color.bg;
            m_color.bg ^= m_color.fg;
            m_color.fg ^= m_color.bg;
        }
        if (p >= 30 && p <= 37) {
            m_color.fg = (p - 30);
        }
        else if (p >= 40 && p <= 47) {
            m_color.bg = (p - 40);
        }
    }
}