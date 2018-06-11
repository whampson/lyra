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
 *   Desc: Console driver. Much of the information on escape sequences was
 *         gathered from http://www.real-world-systems.com/docs/ANSIcode.html
 *         and http://man7.org/linux/man-pages/man4/console_codes.4.html.
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
#include <drivers/pcspk.h>

#define BEL_TICKS   150
#define BEL_FREQ    880

#define CSI_MAX_PARAMS  8
#define PARAM_DEFAULT   (-1)

#define DEFAULT_FG  (VGA_WHT)
#define DEFAULT_BG  (VGA_BLK)
#define BRIGHT      (1 << 3)
#define BLINK       (1 << 3)

struct gfx_attr {
    unsigned char bold      : 1;
    unsigned char faint     : 1;
    unsigned char underline : 1;
    unsigned char blink     : 1;
    unsigned char invert    : 1;
    unsigned char conceal   : 1;
    char fg;
    char bg;
};

struct cursor {
    short x;
    short y;
    char type;
    bool hidden;
};

enum state {
    S_NORMAL,
    S_ESC,
    S_CSI,
};

struct console {
    bool initialized;
    bool active;
    int state;
    struct gfx_attr gfxattr;
    struct cursor cursor;
    union vga_cell *vidmem;
    char tab_width;
    char bs_char;
    char csiparam[CSI_MAX_PARAMS];
    int paramidx;
};

/* Stole this idea from Linux, very convenient! */
#define m_initialized       (cons[curr_cons].initialized)
#define m_active            (cons[curr_cons].active)
#define m_state             (cons[curr_cons].state)
#define m_gfxattr           (cons[curr_cons].gfxattr)
#define m_cursor            (cons[curr_cons].cursor)
#define m_vidmem            (cons[curr_cons].vidmem)
#define m_tab_width         (cons[curr_cons].tab_width)
#define m_bs_char           (cons[curr_cons].bs_char)
#define m_csiparam          (cons[curr_cons].csiparam)
#define m_paramidx          (cons[curr_cons].paramidx)

static struct console cons[NUM_CONSOLES];
static int curr_cons;

static const struct gfx_attr default_gfx = {
    .bold = 0,
    .faint = 0,
    .underline = 0,
    .blink = 0,
    .invert = 0,
    .fg = DEFAULT_FG,
    .bg = DEFAULT_BG
};

static const char COLOR_TABLE[8] = {
    VGA_BLK,
    VGA_RED,
    VGA_GRN,
    VGA_BRN,
    VGA_BLU,
    VGA_MGT,
    VGA_CYN,
    VGA_WHT
};

/* TODO: many of these need to be virtualized to support multiple consoles */
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
static void reverse_linefeed(void);
static void bell(void);
static void cursor_up(int n);
static void cursor_down(int n);
static void cursor_right(int n);
static void cursor_left(int n);
static void erase_display(int cmd);
static void erase_line(int cmd);
static void csi_m(int param);
static void set_cell_attr(union vga_attr *a);
static void do_cursor_update(void);

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
    pos2xy(pos, &m_cursor.x, &m_cursor.y);

    switch_console(curr_cons, curr_cons);
    m_initialized = 1;
}

/* TODO: consider rename */
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
    m_gfxattr = default_gfx;
    m_vidmem = (union vga_cell *) VGA_FRAMEBUF;
    m_cursor.x = 0;
    m_cursor.y = 0;
    m_cursor.type = CURSOR_UNDERBAR;
    m_cursor.hidden = false;
    m_tab_width = 8;
    m_bs_char = ' ';
    m_state = S_NORMAL;
}

static void switch_console(int old_cons, int new_cons)
{
    /* TODO switch video memory paging*/

    do_cursor_update();
    set_cursor_type(m_cursor.type);
    if (m_cursor.hidden) {
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
    bool update_char;
    bool update_attr;
    bool update_cursor;
    bool needs_newline;

    pos = xy2pos(m_cursor.x, m_cursor.y);
    update_char = false;
    update_attr = false;
    update_cursor = true;
    needs_newline = false;

    if (iscntrl(c)) {
        goto ctrl_char;
    }

    switch (m_state)
    {
        case S_ESC:
            handle_esc(c);
            goto move_cursor;

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
                    if (m_cursor.x > 0) {
                        pos--;
                    }
                    backspace();
                    c = m_bs_char;
                    update_char = true;
                    update_attr = true;
                    goto move_cursor;
                case ASCII_HT:
                    tab();
                    goto move_cursor;
                case ASCII_LF:
                case ASCII_VT:
                case ASCII_FF:
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
                    if (++m_cursor.x >= CON_COLS) {
                        needs_newline = true;
                    }
                    update_char = true;
                    update_attr = true;
            }
    }

move_cursor:
    if (update_char) {
        m_vidmem[pos].ch = c;
    }
    if (update_attr) {
        set_cell_attr(&(m_vidmem[pos].attr));
    }
    if (needs_newline) {
        carriage_return();
        linefeed();
    }
    if (update_cursor) {
        do_cursor_update();
    }
}

static void handle_esc(char c)
{
    int i;

    switch (c) {
        // case 'c':    /* reset console */
        case 'D':       /* linefeed */
            linefeed();
            break;
        case 'E':       /* newline */
            carriage_return();
            linefeed();
            break;
        case 'M':       /* reverse linefeed */
            reverse_linefeed();
            break;
        // case '7':    /* save console state */
        // case '8':    /* restore console state */
        case '[':       /* (control sequence introducer) */
            for (i = 0; i < CSI_MAX_PARAMS; i++) {
                m_csiparam[i] = PARAM_DEFAULT;
            }
            m_paramidx = 0;
            m_state = S_CSI;
            return;
        default:
            m_state = S_NORMAL;
            return;
    }
}

static void handle_csi(char c)
{
    int i;

    switch (c) {
        // case '@':    /* insert character */
        case 'A':       /* move cursor up */
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_up(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'B':       /* move cursor down */
        case 'e':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_down(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'C':       /* move cursor right */
        case 'a':
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_right(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'D':       /* move cursor left */
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_left(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'E':       /* move cursor to next line */
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_down(m_csiparam[0]);
            m_cursor.x = 0;
            m_state = S_NORMAL;
            break;
        case 'F':       /* move cursor to previous line */
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            cursor_up(m_csiparam[0]);
            m_cursor.x = 0;
            m_state = S_NORMAL;
            break;
        case 'G':       /* move cursor to column */
        case '`':
            if (m_csiparam[0] < 1) {
                m_csiparam[0] = 1;
            }
            else if (m_csiparam[0] > CON_COLS) {
                m_csiparam[0] = CON_COLS;
            }
            m_cursor.x = m_csiparam[0] - 1;
            m_state = S_NORMAL;
            break;
        case 'd':       /* move cursor to row */
            if (m_csiparam[0] < 1) {
                m_csiparam[0] = 1;
            }
            else if (m_csiparam[0] > CON_ROWS) {
                m_csiparam[0] = CON_ROWS;
            }
            m_cursor.y = m_csiparam[0] - 1;
            m_state = S_NORMAL;
            break;
        case 'H':       /* move cursor to row, column */
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
            m_cursor.y = m_csiparam[0] - 1;
            m_cursor.x = m_csiparam[1] - 1;
            m_state = S_NORMAL;
            break;
        case 'J':       /* erase display */
            erase_display(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'K':       /* erase line */
            erase_line(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        // case 'L':    /* insert line */
        // case 'M':    /* delete line */
        // case 'P':    /* delete character */
        case 'S':       /* scroll up */
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            scroll_up(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        case 'T':       /* scroll down */
            if (m_csiparam[0] == PARAM_DEFAULT) {
                m_csiparam[0] = 1;
            }
            scroll_down(m_csiparam[0]);
            m_state = S_NORMAL;
            break;
        // case 'X':    /* erase character */
        // case 'Z':    /* backtab */
        case 'm':       /* set attributes */
            i = 0;
            while (i <= m_paramidx) {
                csi_m(m_csiparam[i++]);
            }
            m_state = S_NORMAL;
            break;
        // case 'n':    /* report cursor pos (send to tty input buf) */
        // case 's':    /* save cursor pos */
        // case 'u':    /* restore saved cursor pos */
        case ';':       /* (parameter separator) */
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
    int n_cells;
    int n_bytes;
    int blank_start;
    union vga_cell cell;
    int i;

    if (n <= 0) {
        return;
    }

    if (n >= CON_ROWS) {
        n = CON_ROWS;
    }

    n_cells = n * CON_COLS;
    blank_start = CON_AREA - n_cells;
    n_bytes = blank_start * sizeof(uint16_t);
    memmove(m_vidmem, &(m_vidmem[n_cells]), n_bytes);

    cell.ch = m_bs_char;
    set_cell_attr(&cell.attr);
    for (i = 0; i < n_cells; i++) {
        m_vidmem[blank_start + i] = cell;
    }
}

static void scroll_down(int n)
{
    int n_cells;
    int n_bytes;
    union vga_cell cell;
    int i;

    if (n <= 0) {
        return;
    }

    if (n >= CON_ROWS) {
        n = CON_ROWS;
    }

    n_cells = n * CON_COLS;
    n_bytes = (CON_AREA - n_cells) * sizeof(uint16_t);
    memmove(&(m_vidmem[n_cells]), m_vidmem, n_bytes);

    cell.ch = m_bs_char;
    set_cell_attr(&cell.attr);
    for (i = 0; i < n_cells; i++) {
        m_vidmem[i] = cell;
    }
}

static void backspace(void)
{
    if (--m_cursor.x < 0) {
        m_cursor.x = 0;
    }
}

static void tab(void)
{
    char tmp;

    tmp = m_cursor.x % m_tab_width;
    if (tmp == 0) {
        m_cursor.x += m_tab_width;
    }
    else {
        m_cursor.x += (m_tab_width - tmp);
    }
    if (m_cursor.x >= CON_COLS) {
        m_cursor.x = CON_COLS - 1;
    }
}

static void linefeed(void)
{
    if (++m_cursor.y >= CON_ROWS) {
        scroll_up(1);
        m_cursor.y--;
    }
}

static void reverse_linefeed(void)
{
    if (--m_cursor.y < 0) {
        scroll_down(1);
        m_cursor.y++;
    }
}

static void carriage_return(void)
{
    m_cursor.x = 0;
}

static void bell(void)
{
    pcspk_set_freq(BEL_FREQ);
    pcspk_beep(BEL_TICKS);
}

static void cursor_up(int n)
{
    m_cursor.y -= n;
    if (m_cursor.y < 0) {
        m_cursor.y = 0;
    }
}

static void cursor_down(int n)
{
    m_cursor.y += n;
    if (m_cursor.y >= CON_ROWS) {
        m_cursor.y = CON_ROWS - 1;
    }
}

static void cursor_right(int n)
{
    m_cursor.x += n;
    if (m_cursor.x >= CON_COLS) {
        m_cursor.x = CON_COLS - 1;
    }
}

static void cursor_left(int n)
{
    m_cursor.x -= n;
    if (m_cursor.x < 0) {
        m_cursor.x = 0;
    }
}

static void erase_display(int cmd)
{
    int pos;
    union vga_attr attr;

    set_cell_attr(&attr);

    switch (cmd) {
        case PARAM_DEFAULT:
        case 0:
            pos = xy2pos(m_cursor.x, m_cursor.y);
            for (; pos < CON_AREA; pos++) {
                m_vidmem[pos].ch = m_bs_char;
                m_vidmem[pos].attr = attr;
            }
            break;
        case 1:
            pos = xy2pos(m_cursor.x, m_cursor.y);
            for (; pos > -1; pos--) {
                m_vidmem[pos].ch = m_bs_char;
                m_vidmem[pos].attr = attr;
            }
            break;
        case 2:
            for (pos = 0; pos < CON_AREA; pos++) {
                m_vidmem[pos].ch = m_bs_char;
                m_vidmem[pos].attr = attr;
            }
            break;
    }
}

static void erase_line(int cmd)
{
    int i;
    int pos;
    union vga_attr attr;

    set_cell_attr(&attr);

    switch (cmd) {
        case PARAM_DEFAULT:
        case 0:
            pos = xy2pos(m_cursor.x, m_cursor.y);
            for (i = m_cursor.x; i < CON_COLS; i++, pos++) {
                m_vidmem[pos].ch = m_bs_char;
                m_vidmem[pos].attr = attr;
            }
            break;
        case 1:
            pos = xy2pos(m_cursor.x, m_cursor.y);
            for (i = m_cursor.x; i > -1; i--, pos--) {
                m_vidmem[pos].ch = m_bs_char;
                m_vidmem[pos].attr = attr;
            }
            break;
        case 2:
            pos = xy2pos(0, m_cursor.y);
            for (i = 0; i < CON_COLS; i++, pos++) {
                m_vidmem[pos].ch = m_bs_char;
                m_vidmem[pos].attr = attr;
            }
            break;
    }
}

static void csi_m(int param)
{
    switch (param) {
        case 0:
            m_gfxattr = default_gfx;
            break;
        case 1:
            m_gfxattr.bold = 1;
            break;
        case 2:
            m_gfxattr.faint = 1;
            break;
        case 4:
            m_gfxattr.underline = 1;
            break;
        case 5:
            m_gfxattr.blink = 1;
            break;
        case 7:
            m_gfxattr.invert = 1;
            break;
        case 8:
            m_gfxattr.conceal = 1;
            break;
        case 21:
            m_gfxattr.bold = 0;
            break;
        case 22:
            m_gfxattr.faint = 0;
            break;
        case 24:
            m_gfxattr.underline = 0;
            break;
        case 25:
            m_gfxattr.blink = 0;
            break;
        case 27:
            m_gfxattr.invert = 0;
            break;
        case 28:
            m_gfxattr.conceal = 0;
            break;
        case 39:
            m_gfxattr.fg = DEFAULT_FG;
            break;
        case 49:
            m_gfxattr.bg = DEFAULT_BG;
            break;
        default:
            if (param >= 30 && param <= 37) {
                m_gfxattr.fg = COLOR_TABLE[(param - 30)];
            }
            else if (param >= 40 && param <= 47) {
                m_gfxattr.bg = COLOR_TABLE[(param - 40)];
            }
            break;
    }
}

static void set_cell_attr(union vga_attr *a)
{
    a->fg = m_gfxattr.fg;
    a->bg = m_gfxattr.bg;

    if (m_gfxattr.bold) {
        a->fg |= BRIGHT;
    }
    if (m_gfxattr.faint) {
        /* simulate with color */
        a->fg = VGA_GRY;
    }
    if (m_gfxattr.underline) {
        /* simulate with color */
        a->fg = VGA_CYN;
    }
    if (m_gfxattr.blink) {
        /* must be enabled in VGA driver or this will do a bright bg instead */
        a->bg |= BLINK;
    }
    if (m_gfxattr.invert) {
        swap(a->fg, a->bg);
    }
    if (m_gfxattr.conceal) {
        a->fg = a->bg;
    }
}

static void do_cursor_update(void)
{
    set_cursor_pos(xy2pos(m_cursor.x, m_cursor.y));
}
