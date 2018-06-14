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
 *   File: kernel/kprintf.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <lyra/console.h>
#include <lyra/kernel.h>

#define W_MAX   64
#define P_MAX   64

#define W_NONE  0
#define P_NONE  (-1)

enum printf_fl {
    F_NONE      = 0x00,
    F_PRINTSIGN = 0x01,
    F_SIGNALIGN = 0x02,
    F_PREFIX    = 0x04,
    F_ZEROPAD   = 0x08
};

enum fmt_state {
    S_READF,
    S_READW,
    S_READP,
    S_READL
};

static int fmt_char(int w, bool ljust, va_list *ap);
static int fmt_string(int w, int p, bool ljust, va_list *ap);
static int fmt_int(enum printf_fl fl, int w, int p, bool s, int b, bool lower,
                   bool ljust, va_list *ap);

static int writechar(char c);
static int writestr(const char *s, int len);

static char * strlower(char *str);
static int num2str(int val, char *str, int base, bool sign_allowed);
static int pad(int n, char c);

int kprintf(const char *fmt, ...)
{
    int retval;
    va_list ap;

    va_start(ap, fmt);
    retval = vkprintf(fmt, ap);
    va_end(ap);

    return retval;
}

int vkprintf(const char *fmt, va_list args)
{
    bool formatting;
    enum fmt_state state;
    int count;
    char c;
    enum printf_fl fl;
    int w, p;
    bool ljust;
    char *numptr;

    state = S_READF;
    fl = F_NONE;
    w = W_NONE;
    p = P_NONE;
    ljust = false;
    numptr = NULL;

    formatting = false;
    count = 0;

    while ((c = *(fmt++)) != '\0')
    {
        switch (c)
        {
            /* Format specifiers */
            case '%':
                if (!formatting) {
                    formatting = true;
                    /* Formatting defaults */
                    state = S_READF;
                    fl = F_NONE;
                    w = W_NONE;
                    p = P_NONE;
                    ljust = false;
                    numptr = NULL;
                    continue;
                }
                formatting = false;
                goto sendchar;

            case 'd':
            case 'i':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
                count += fmt_int(fl, w, p, true, 10, false, ljust, &args);
                formatting = false;
                continue;

            case 'u':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
                count += fmt_int(fl, w, p, false, 10, false, ljust, &args);
                formatting = false;
                continue;

            case 'o':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
                count += fmt_int(fl, w, p, false, 8, false, ljust, &args);
                formatting = false;
                continue;

            case 'x':
            case 'X':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
                count += fmt_int(fl, w, p, false, 16, (c == 'x'), ljust, &args);
                formatting = false;
                continue;

            case 'c':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                count += fmt_char(w, ljust, &args);
                formatting = false;
                continue;

            case 's':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
                count += fmt_string(w, p, ljust, &args);
                formatting = false;
                continue;

            case 'p':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
                fl |= F_PREFIX;
                count += fmt_int(fl, w, p, false, 16, true, ljust, &args);
                formatting = false;
                continue;


            /* Flags */
            case '-':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    ljust = true;
                }
                continue;

            case '+':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    fl |= F_PRINTSIGN;
                }
                continue;

            case ' ':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    fl |= F_SIGNALIGN;
                }
                continue;

            case '#':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    fl |= F_PREFIX;
                }
                continue;

            case '0':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    fl |= F_ZEROPAD;
                    continue;
                }
                goto handle_num;

            /* Special */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
        handle_num:
                if (!formatting) {
                    goto sendchar;
                }
                if (numptr == NULL) {
                    numptr = (char *) (fmt - 1);
                }
                if (state == S_READF) {
                    state = S_READW;
                }
                continue;

            case '.':
                if (!formatting) {
                    goto sendchar;
                }
                state = S_READP;
                p = 0;

                if (numptr != NULL) {
                    w = atoi(numptr);
                    numptr = NULL;
                }
                continue;

            case '*':
                if (!formatting) {
                    goto sendchar;
                }
                if (numptr != NULL) {
                    /* Invalid format, cancel. */
                    formatting = false;
                    break;
                }
                if (state == S_READF) {
                    w = va_arg(args, int);
                    if (w < 0) {
                        ljust = true;
                        w = negate(w);
                    }
                    state = S_READW;
                }
                else if (state == S_READP) {
                    p = va_arg(args, int);
                }
                continue;

            /* Invalid specifier */
            default:
                if (!formatting) {
                    goto sendchar;
                }
                formatting = false;
                break;
        }

    sendchar:
        count += writechar(c);
    }

    tty_flush(&sys_tty);

    return count;
}

static int fmt_char( int w, bool ljust, va_list *ap)
{
    int count;
    int npad;
    unsigned char c;

    count = 0;
    npad = w - 1;
    c = (unsigned char) va_arg(*ap, int);

    if (!ljust && w > 0) {
        count += pad(npad, ' ');
    }

    count += writechar(c);

    if (ljust && w > 0) {
        count += pad(npad, ' ');
    }

    return count;
}

static int fmt_string(int w, int p, bool ljust, va_list *ap)
{
    int count;
    int npad;
    char *s;
    int len;

    if (p == 0) {
        return 0;
    }

    count = 0;
    s = (char *) va_arg(*ap, char*);
    len = strlen(s);

    if (p > 0) {
        len = p;
    }
    npad = w - len;

    if (!ljust && npad > 0 && w > 0) {
        count += pad(npad, ' ');
    }

    count += writestr(s, len);

    if (ljust && npad > 0 && w > 0) {
        count += pad(npad, ' ');
    }

    return count;
}

static int fmt_int(enum printf_fl fl, int w, int p, bool s, int b, bool lower,
                   bool ljust, va_list *ap)
{
    int val;
    char padch;
    int npad;
    int nchar;
    int ndigit;
    bool print_plus;
    bool sign_align;
    char buf[32];
    int buflen;

    padch = ' ';
    npad = 0;
    sign_align = (flag_set(fl, F_SIGNALIGN) && b == 10);
    print_plus = (flag_set(fl, F_PRINTSIGN) && b == 10);

    val = va_arg(*ap, int);

    /* A value of zero with zero precision should be blank. */
    if (p == 0 && val == 0) {
        return 0;
    }

    /* A prefix should not be printed for 0. */
    if (val == 0 && flag_set(fl, F_PREFIX)) {
        fl &= ~F_PREFIX;
    }

    buflen = num2str(val, buf, b, s);
    ndigit = buflen;
    if (lower) {
        strlower(buf);
    }
    nchar = ndigit;

    if (flag_set(fl, F_ZEROPAD) && p == P_NONE) {
        padch = '0';
    }

    if (flag_set(fl, F_PREFIX)) {
        switch (b) {
            case 8:
                nchar += 1;
                break;
            case 16:
                nchar += 2;
                break;
        }
    }

    if (print_plus || sign_align) {
        nchar += 1;
    }

    npad = w - nchar;
    if (p > ndigit) {
        npad -= p - ndigit;
    }

    if (flag_set(fl, F_PREFIX)) {
        switch (b) {
            case 8:
                writechar('0');
                break;
            case 16:
                writechar('0');
                if (lower) {
                    writechar('x');
                }
                else {
                    writechar('X');
                }
                break;
        }
    }
    else if (print_plus && val >= 0) {
        writechar('+');
    }
    else if (sign_align && val >= 0) {
        writechar(' ');
    }

    if (!ljust && w > nchar) {
        nchar += pad(npad, padch);
    }

    while (p > ndigit) {
        writechar('0');
        ndigit++;
        nchar++;
    }

    writestr(buf, buflen);

    if (ljust && w > nchar) {
        nchar += pad( npad, ' ');
    }

    return nchar;
}

static char * strlower(char *str)
{
    int i;

    i = 0;
    while (str[i] != '\0') {
        str[i] = tolower(str[i]);
        i++;
    }

    return str;
}

/* itoa() but with more parameters :) */
static int num2str(int val, char *str, int base, bool sign_allowed)
{
    static char lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char *str_base;
    int i;
    bool sign;
    unsigned int uval;

    if (str == NULL) {
        return 0;
    }

    if (base < 2 || base > 36) {
        str[0] = '\0';
        return 0;
    }

    /* Special case for zero */
    if (val == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }

    str_base = str;

    if (sign_allowed && base == 10) {
        sign = (val < 0);
        if (sign && base == 10) {
            val = negate(val);
        }
        while (val > 0) {
            i = val % base;
            *str = lookup[i];
            str++;
            val /= base;
        }
    }
    else {
        sign = false;
        uval = (unsigned int) val;
        while (uval > 0) {
            i = (int) uval % base;
            *str = lookup[i];
            str++;
            uval /= base;
        }
    }

    if (sign) {
        *(str++) = '-';
    }
    *str = '\0';

    strrev(str_base);

    return (int) (str - str_base);
}

int atoi(const char *str)
{
    size_t i;
    int val;
    bool sign;
    char c;

    if (str == NULL) {
        return 0;
    }

    while (isspace(*str)) {
        str++;
    }

    sign = false;
    if (*str == '-') {
        sign = true;
        str++;
    }
    else if (*str == '+') {
        str++;
    }

    val = 0;
    i = 0;
    while (i < 10 && (c = str[i++]) != '\0') {
        if (!isdigit(c)) {
            break;
        }
        val *= 10;
        val += (c - '0');
    }

    if (sign) {
        val = negate(val);
    }

    return val;
}

static int pad(int n, char c)
{
    int i;

    for (i = 0; i < n; i++) {
        writechar(c);
    }

    return i;
}

static int writechar(char c)
{
    return tty_write(&sys_tty, &c, 1);
}

static int writestr(const char *s, int len)
{
    int count;

    count = 0;
    while (count < len && *s != '\0') {
        count += writechar(*(s++));
    }

    return count;
}