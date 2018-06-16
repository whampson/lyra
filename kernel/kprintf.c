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

/* Default values for parameters */
#define W_NONE  0
#define P_NONE  (-1)

enum printf_flags {
    F_NONE      = 0,
    F_PRINTSIGN = (1 << 0), /* show sign where applicable */
    F_SIGNALIGN = (1 << 1), /* if no sign printed, print space */
    F_PREFIX    = (1 << 2), /* print radix prefix */
    F_ZEROPAD   = (1 << 3)  /* pad with zeros instead of spaces */
};

struct printf_params {
    int fd;                 /* file to write */
    char *buf;              /* buffer to write */
    int pos;                /* buffer position */
    int n;                  /* max char limit */
    bool bounded;           /* abide by max char limit */
    bool use_buf;           /* 0 = write to fd, 1 = write to buf */
    bool buf_full;          /* output buffer is full (bounded = 1 only) */
    int flags;              /* formatting flags (see above) */
    int w;                  /* width */
    int p;                  /* precision */
    int base;               /* radix */
    bool sign;              /* format as signed number (base 10 only) */
    bool lower;             /* format using lowercase digits */
    bool ljust;             /* left-justified */
};

enum printf_fmt_state {
    S_READF,                /* reading flags */
    S_READW,                /* reading width */
    S_READP,                /* reading precision */
    S_READL                 /* reading length (not supported yet) */
};

static int do_printf(struct printf_params *params, const char *f, va_list *ap);

static int fmt_char(struct printf_params *params, va_list *ap);
static int fmt_string(struct printf_params *params, va_list *ap);
static int fmt_int(struct printf_params *params, va_list *ap);

static int pad(struct printf_params *params, int n, char c);
static int writechar(struct printf_params *params, char c);
static int writestr(struct printf_params *params, const char *str, int len);

static char * strlower(char *str);
static int num2str(int val, char *str, int base, bool sign_allowed);

int printf(const char *fmt, ...)
{
    int retval;
    va_list ap;

    va_start(ap, fmt);
    retval = vprintf(fmt, ap);
    va_end(ap);

    return retval;
}

int vprintf(const char *fmt, va_list args)
{
    struct printf_params params;

    if (fmt == NULL) {
        return -1;
    }

    params.fd = 0;
    params.use_buf = false;
    params.bounded = false;

    return do_printf(&params, fmt, &args);
}

int sprintf(char *str, const char *fmt, ...)
{
    int retval;
    va_list ap;

    va_start(ap, fmt);
    retval = vsprintf(str, fmt, ap);
    va_end(ap);

    return retval;
}

int vsprintf(char *str, const char *fmt, va_list args)
{
    int count;
    struct printf_params params;

    if (fmt == NULL || str == NULL) {
        return -1;
    }

    params.buf = str;
    params.use_buf = true;
    params.bounded = false;

    count = do_printf(&params, fmt, &args);
    str[count] = '\0';      /* TODO: check that this is corect behavior */

    return count;
}

// int snprintf(char *str, int n, const char *fmt, ...)

static int do_printf(struct printf_params *params, const char *f, va_list *ap)
{
    bool formatting;
    char *numptr;
    int state;
    int count;
    char c;

    formatting = false;
    numptr = NULL;
    state = S_READF;
    count = 0;

    params->pos = 0;
    params->buf_full = false;

    while ((c = *(f++)) != '\0')
    {
        switch (c)
        {
            /* Format specifiers */
            case '%':
                if (!formatting) {
                    formatting = true;
                    /* Formatting defaults */
                    state = S_READF;
                    numptr = NULL;
                    params->flags = F_NONE;
                    params->w = W_NONE;
                    params->p = P_NONE;
                    params->ljust = false;
                    continue;
                }
                goto sendchar;

            case 'd':
            case 'i':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { params->w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { params->p = atoi(numptr); }
                // count += fmt_int(fd, dest, fl, w, p, 1, 10, 0, ljust, &args);
                params->sign = true;
                params->base = 10;
                count += fmt_int(params, ap);
                formatting = false;
                continue;

            case 'u':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { params->w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { params->p = atoi(numptr); }
                // count += fmt_int(fd, dest, fl, w, p, 0, 10, 0, ljust, &args);
                params->sign = false;
                params->base = 10;
                count += fmt_int(params, ap);
                formatting = false;
                continue;

            case 'o':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { params->w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { params->p = atoi(numptr); }
                // count += fmt_int(fd, dest, fl, w, p, 0, 8, 0, ljust, &args);
                params->base = 8;
                count += fmt_int(params, ap);
                formatting = false;
                continue;

            case 'p':
                params->flags |= F_PREFIX;
            case 'x':
            case 'X':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { params->w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { params->p = atoi(numptr); }
                // count += fmt_int(fd, dest, fl, w, p, 0, 16, 1, ljust, &args);
                params->base = 16;
                params->lower = (c == 'x');
                count += fmt_int(params, ap);
                formatting = false;
                continue;

            // case 'X':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
            //     if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
            //     count += fmt_int(fd, dest, fl, w, p, 0, 16, 0, ljust, &args);
            //     formatting = false;
            //     continue;

            case 'c':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { params->w = atoi(numptr); }
                // count += fmt_char(fd, dest, w, ljust, &args);
                count += fmt_char(params, ap);
                formatting = false;
                continue;

            case 's':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READW && numptr != NULL) { params->w = atoi(numptr); }
                if (state == S_READP && numptr != NULL) { params->p = atoi(numptr); }
                // count += fmt_string(fd, dest, w, p, ljust, &args);
                count += fmt_string(params, ap);
                formatting = false;
                continue;

            // case 'p':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     if (state == S_READW && numptr != NULL) { w = atoi(numptr); }
            //     if (state == S_READP && numptr != NULL) { p = atoi(numptr); }
            //     fl |= F_PREFIX;
            //     count += fmt_int(fd, dest, fl, w, p, 0, 16, 1, ljust, &args);
            //     formatting = false;
            //     continue;

            /* Flags */
            case '-':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    params->ljust = true;
                }
                continue;

            case '+':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    params->flags |= F_PRINTSIGN;
                }
                continue;

            case ' ':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    params->flags |= F_SIGNALIGN;
                }
                continue;

            case '#':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    params->flags |= F_PREFIX;
                }
                continue;

            case '0':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    params->flags |= F_ZEROPAD;
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
                    numptr = (char *) (f - 1);
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
                params->p = 0;

                if (numptr != NULL) {
                    params->w = atoi(numptr);
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
                    params->w = va_arg(*ap, int);
                    if (params->w < 0) {
                        params->ljust = true;
                        params->w = negate(params->w);
                    }
                    state = S_READW;
                }
                else if (state == S_READP) {
                    params->p = va_arg(*ap, int);
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
        count += writechar(params, c);
    }

    tty_flush(&sys_tty);

    return count;
}

static int fmt_char(struct printf_params *params, va_list *ap)
{
    int count;
    int npad;
    unsigned char c;

    count = 0;
    npad = params->w - 1;
    c = (unsigned char) va_arg(*ap, int);

    if (!params->ljust && params->w > 0) {
        count += pad(params, npad, ' ');
    }

    count += writechar(params, c);

    if (params->ljust && params->w > 0) {
        count += pad(params, npad, ' ');
    }

    return count;
}

static int fmt_string(struct printf_params *params, va_list *ap)
{
    int count;
    int npad;
    char *s;
    int len;

    if (params->p == 0) {
        return 0;
    }

    count = 0;
    s = (char *) va_arg(*ap, char*);
    len = strlen(s);

    if (params->p > 0) {
        len = params->p;
    }
    npad = params->w - len;

    if (!params->ljust && npad > 0 && params->w > 0) {
        count += pad(params, npad, ' ');
    }

    count += writestr(params, s, len);

    if (params->ljust && npad > 0 && params->w > 0) {
        count += pad(params, npad, ' ');
    }

    return count;
}

static int fmt_int(struct printf_params *params, va_list *ap)
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
    sign_align = (flag_set(params->flags, F_SIGNALIGN) && params->base == 10);
    print_plus = (flag_set(params->flags, F_PRINTSIGN) && params->base == 10);

    val = va_arg(*ap, int);

    /* A value of zero with zero precision should be blank. */
    if (params->p == 0 && val == 0) {
        return 0;
    }

    /* A prefix should not be printed for 0. */
    if (val == 0 && flag_set(params->flags, F_PREFIX)) {
        params->flags &= ~F_PREFIX;
    }

    buflen = num2str(val, buf, params->base, params->sign);
    ndigit = buflen;
    if (params->lower) {
        strlower(buf);
    }
    nchar = ndigit;

    if (flag_set(params->flags, F_ZEROPAD) && params->p == P_NONE) {
        padch = '0';
    }

    if (flag_set(params->flags, F_PREFIX)) {
        switch (params->base) {
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

    npad = params->w - nchar;
    if (params->p > ndigit) {
        npad -= params->p - ndigit;
    }

    if (flag_set(params->flags, F_PREFIX)) {
        switch (params->base) {
            case 8:
                writechar(params, '0');
                break;
            case 16:
                writechar(params, '0');
                if (params->lower) {
                    writechar(params, 'x');
                }
                else {
                    writechar(params, 'X');
                }
                break;
        }
    }
    else if (print_plus && val >= 0) {
        writechar(params, '+');
    }
    else if (sign_align && val >= 0) {
        writechar(params, ' ');
    }

    if (!params->ljust && params->w > nchar) {
        nchar += pad(params, npad, padch);
    }

    while (params->p > ndigit) {
        writechar(params, '0');
        ndigit++;
        nchar++;
    }

    writestr(params, buf, buflen);

    if (params->ljust && params->w > nchar) {
        nchar += pad(params, npad, ' ');
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

static int pad(struct printf_params *params, int n, char c)
{
    int i;

    for (i = 0; i < n; i++) {
        writechar(params, c);
    }

    return i;
}

static int writechar(struct printf_params *params, char c)
{
    int count;

    /* TODO: bounds */

    if (params->use_buf) {
        params->buf[params->pos++] = c;
        return 1;
    }

    count = 0;
    switch (params->fd) {
        case 0:
            count += tty_write(&sys_tty, &c, 1);
            break;
    }

    return count;
}

static int writestr(struct printf_params *params, const char *str, int len)
{
    int count;

    count = 0;
    while (count < len && *str != '\0') {
        count += writechar(params, *(str++));
    }

    return count;
}
