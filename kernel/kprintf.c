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
#include <lyra/kernel.h>

#define W_MAX   64
#define P_MAX   64

#define W_NONE  0
#define P_NONE  (-1)

enum printf_fl {
    F_NONE      = 0x00,
    F_SIGN      = 0x01,
    F_SPACE     = 0x02,
    F_PREFIX    = 0x04,
    F_ZEROPAD   = 0x08
};

enum fmt_state {
    S_READF,
    S_READW,
    S_READP,
    S_READL
};

static char * make_lower(char *str);
static size_t num2str(int val, char *str, int base, bool s);
static void pad(char *buf, int n, char c);

static void fmt_char(char *buf, int w, bool ljust, va_list *ap);
static void fmt_int(char *buf, enum printf_fl fl, int w, int p, bool s, int b,
                    bool ljust, va_list *ap);

/**
 * Kernel printf(). Use this to print to the kernel console.
 * Most of the same rules as printf() apply, though some features have been left
 * out because I see no need for them in the context of this kernel. Details
 * below.
 *
 * Format Specifier Prototype:
 *   %[flags][width][.precision][length]specifier
 *
*   specifier:
 *     d/i      - signed decimal integer
 *     u        - unsigned decimal integer
 *     o        - unsigned octal integer
 *     x        - unsigned hexadecimal integer
 *     X        - unsigned hexadecimal integer (uppercase)
 *     f        - NOT_SUPPORTED (may change if floats needed)
 *     F        - NOT_SUPPORTED (may change if floats needed)
 *     e        - NOT_SUPPORTED
 *     E        - NOT_SUPPORTED
 *     g        - NOT_SUPPORTED
 *     G        - NOT_SUPPORTED
 *     a        - NOT_SUPPORTED
 *     A        - NOT_SUPPORTED
 *     c        - character
 *     s        - string of characters
 *     p        - pointer address
 *     n        - NOT_SUPPORTED
 *     %        - '%'
 *
 *   flags:
 *     -        - left justify padding
 *     +        - always print sign
 *     (space)  - print a space if a sign would be printed (unless + specified)
 *     #        - (o,x,X) prepend (0,0x,0X) for nonzero values
 *     0        - left-pad the number with zeros instead of spaces
 *
 *   width:
 *     (num)    - min. chars to be printed; padded w/ blank spaces or 0
 *     *        - width specified as next argument in arg list
 *
 *   .precision:
 *     (num)    - (d,i,o,u,x,X) min. digits to be written; pad w/ zeros
 *     *        - precision specified as next argument in arg list
 *
 *   length:      d i           u o x X             c       s       p
 *     (none)   - int           unsigned int        int     char*   void*
 *     hh       - N/S           N/S                 N/S
 *     h        - N/S           N/S                 N/S     N/S     N/S
 *     l        - long int      unsigned long int   N/S     N/S
 *     ll       - N/S           N/S                 N/S     N/S     N/S
 *     j        - N/S           N/S                 N/S     N/S     N/S
 *     z        - size_t        size_t              N/S     N/S     N/S
 *     t        - ptrdiff_t     ptrdiff_t           N/S     N/S     N/S
 *     L        - N/S           N/S                 N/S     N/S     N/S
 *
 * NOTE: Using an unsupported specifier will cause undefined behavior.
 */
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
    /* OBSERVATIONS:
        - keeps reading flags until a # or '0' not found
            - this means both # and 0 may be included int the same fmt spec
        - invalid fmt specs are simply printed verbatim
        - 'p' automatically adds '0x' prefix
    */

    /* TODO:
        - implement 's'
        - read numbers embedded within format string
    */

    bool formatting;
    enum fmt_state state;
    int count;
    char c;
    char fmtbuf[128];
    enum printf_fl fl;
    int w, p;
    bool ljust;

    formatting = false;
    count = 0;

    while ((c = *(fmt++)) != '\0') {
        switch (c) {
            /* Format specifiers */
            case '%':
                if (!formatting) {
                    formatting = true;
                    state = S_READF;
                    /* Formatting defaults */
                    fl = F_NONE;
                    w = W_NONE;
                    p = P_NONE;
                    ljust = false;
                    continue;
                }
                formatting = false;
                goto sendchar;

            case 'd':
            case 'i':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, w, p, true, 10, ljust, &args);
                goto sendfmt;

            case 'u':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, w, p, false, 10, ljust, &args);
                goto sendfmt;

            case 'o':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, w, p, false, 8, ljust, &args);
                goto sendfmt;

            case 'x':
            case 'X':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, w, p, false, 16, ljust, &args);
                if (c == 'x') {
                    make_lower(fmtbuf);
                }
                goto sendfmt;

            case 'c':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_char(fmtbuf, w, ljust, &args);
                goto sendfmt;

            case 's':
            case 'p':
                if (!formatting) {
                    goto sendchar;
                }
                fl |= F_PREFIX;
                fmt_int(fmtbuf, fl, w, p, false, 16, ljust, &args);
                make_lower(fmtbuf);
                goto sendfmt;


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
                    fl |= F_SIGN;
                }
                continue;

            case ' ':
                if (!formatting) {
                    goto sendchar;
                }
                else if (state == S_READF) {
                    fl |= F_SPACE;
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
                }
                continue;

            /* Special */
            case '.':
                if (!formatting) {
                    goto sendchar;
                }
                state = S_READP;
                p = 0;
                continue;

            case '*':
                if (!formatting) {
                    goto sendchar;
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
        putchar(c);
        count++;
        continue;

    sendfmt:
        count += puts(fmtbuf);
        formatting = false;
    }

    return count;
}

static char * make_lower(char *str)
{
    int i;

    i = 0;
    while (str[i] != '\0') {
        str[i] = tolower(str[i]);
        i++;
    }

    return str;
}

static size_t num2str(int val, char *str, int base, bool s)
{
    static char lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char *str_base;
    size_t i;
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

    if (s && base == 10) {
        sign = (val < 0);
        if (sign && base == 10) {
            val = negate(val);
        }
        while (val > 0) {
            i = (size_t) val % base;
            *str = lookup[i];
            str++;
            val /= base;
        }
    }
    else {
        sign = false;
        uval = (unsigned int) val;
        while (uval > 0) {
            i = (size_t) uval % base;
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

    return (size_t) (str - str_base);
}

static void pad(char *buf, int n, char c)
{
    int i;

    for (i = 0; i < n; i++) {
        buf[i] = c;
    }
}

static void fmt_char(char *buf, int w, bool ljust, va_list *ap)
{
    int npad;
    unsigned char val;

    // TODO: decide how to handle non-printing characters

    npad = w - 1;
    val = (unsigned char) va_arg(*ap, int);

    if (!ljust && w > 0) {
        pad(buf, npad, ' ');
        buf += npad;
    }

    *(buf++) = val;

    if (ljust && w > 0) {
        pad(buf, npad, ' ');
        buf += npad;
    }
    *buf = '\0';
}

static void fmt_int(char *buf, enum printf_fl fl, int w, int p, bool s, int b,
                    bool ljust, va_list *ap)
{
    int val;
    char padch;
    int npad;
    int nchar;
    int ndigit;
    bool print_plus;
    bool sign_would_print;
    char tmpbuf[32];
    size_t tmpbuflen;

    // TODO: '+' and ' ' flags can be optimized

    padch = ' ';
    npad = 0;
    sign_would_print = (flag_set(fl, F_SPACE) && b == 10);
    print_plus = (flag_set(fl, F_SIGN) && b == 10);

    val = va_arg(*ap, int);

    /* A value of zero with zero precision should be blank. */
    if (p == 0 && val == 0) {
        *buf = '\0';
        return;
    }

    tmpbuflen = num2str(val, tmpbuf, b, s);
    ndigit = tmpbuflen;
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

    if (print_plus || sign_would_print) {
        nchar += 1;
    }

    npad = w - nchar;
    if (p > ndigit) {
        npad -= p - ndigit;
    }

    if (!ljust && w > nchar) {
        pad(buf, npad, padch);
        nchar += npad;
        buf += npad;
    }

    if (flag_set(fl, F_PREFIX)) {
        switch (b) {
            case 8:
                *(buf++) = '0';
                break;
            case 16:
                *(buf++) = '0';
                *(buf++) = 'X';
                break;
        }
    }
    else if (print_plus && val >= 0) {
        *(buf++) = '+';
    }
    else if (sign_would_print && val >= 0) {
        *(buf++) = ' ';
    }

    while (p > ndigit) {
        *(buf++) = '0';
        ndigit++;
        nchar++;
    }

    strncpy(buf, tmpbuf, sizeof(tmpbuf));

    if (ljust && w > nchar) {
        buf += tmpbuflen;
        pad(buf, npad, ' ');
        nchar += npad;
        buf += npad;
    }

    buf[nchar] = '\0';
}
