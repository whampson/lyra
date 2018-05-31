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

#define MAX_WIDTH       64
#define MAX_PRECISION   64

enum printf_fl {
    F_PREFIX    = 0x01,
    F_ZEROPAD   = 0x02
};

enum printf_len {
    L_NONE,
    L_L,
    L_Z,
    L_T
};

enum fmt_state {
    S_READF,
    S_READW,
    S_READP,
    S_READL
};

static size_t num2str(int val, char *str, int base, bool s);

static void fmt_int(char *buf, enum printf_fl fl, enum printf_len l, int w,
                   int p, bool s, int b, va_list *ap);

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
 *     -        - NOT_SUPPORTED
 *     +        - NOT_SUPPORTED
 *     (space)  - NOT_SUPPORTED
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
        - implement 'X'
        - implement flags,width,precision,length
        - allow for negative numbers in %d and %i
    */

    bool formatting;
    enum fmt_state state;
    int count;
    char c;
    char *s;
    char fmtbuf[128];
    enum printf_fl fl;
    enum printf_len len;
    int w, p;

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
                    fl = 0;
                    len = L_NONE;
                    w = -1;
                    p = -1;
                    continue;
                }
                formatting = false;
                goto sendchar;

            case 'd':
            case 'i':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, len, w, p, true, 10, &args);
                goto sendfmt;

            case 'u':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, len, w, p, false, 10, &args);
                goto sendfmt;

            case 'o':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, len, w, p, false, 8, &args);
                goto sendfmt;

            case 'x':
            case 'X':
                if (!formatting) {
                    goto sendchar;
                }
                fmt_int(fmtbuf, fl, len, w, p, false, 16, &args);
                if (c == 'x') {
                    // TODO: convert to lower
                }
                goto sendfmt;


            // case 'o':
            //      if (!formatting) {
            //         goto sendchar;
            //     }

            //     itoa(va_arg(args, int), fmtbuf, 8);
            //     puts(fmtbuf);
            //     formatting = false;
            //     continue;

            // case 'X':
            //     if (!formatting) {
            //         goto sendchar;
            //     }

            //     fmt_hex(fmtbuf, false, false);
            //     puts(fmtbuf);
            //     formatting = false;
            //     continue;

            // case 'x':
            // case 'p':
            //     if (!formatting) {
            //         goto sendchar;
            //     }

            //     fmt_hex(fmtbuf, true, c == 'p');
            //     puts(fmtbuf);
            //     formatting = false;
            //     continue;

            // case 'c':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     c = (char) va_arg(args, int);
            //     formatting = false;
            //     goto sendchar;

            // case 's':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     s = (char *) va_arg(args, char*);
            //     puts(s);
            //     formatting = false;
            //     continue;

            /* Flags */
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
                continue;

            case '*':
                if (!formatting) {
                    goto sendchar;
                }
                if (state == S_READF) {
                    state = S_READW;
                    w = va_arg(args, int);
                }
                else if (state == S_READP) {
                    p = va_arg(args, int);
                }
                continue;


            // /* Length specifiers */
            // case 'l':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     reading_flags = false;
            //     len = L_L;
            //     continue;

            // case 'z':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     reading_flags = false;
            //     len = L_Z;
            //     continue;

            // case 't':
            //     if (!formatting) {
            //         goto sendchar;
            //     }
            //     reading_flags = false;
            //     len = L_T;
            //     continue;

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
            val = ~val + 1;
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

static void fmt_int(char *buf, enum printf_fl fl, enum printf_len l, int w,
                   int p, bool s, int b, va_list *ap)
{
    int count;
    int val;
    int i;
    char padch;
    int npad;
    int nchar;
    int ndigit;
    char tmpbuf[32];

    padch = ' ';
    npad = 0;
    nchar = 0;

    val = va_arg(*ap, int);

    /* A value of zero with zero precision should be blank. */
    if (p == 0 && val == 0) {
        *buf = '\0';
        return;
    }

    // switch (l) {
    //     case L_L:
    //         val = va_arg(*ap, long int);
    //         break;
    //     case L_Z:
    //         val = va_arg(*ap, size_t);
    //         break;
    //     case L_T:
    //         val = va_arg(*ap, ptrdiff_t);
    //         break;
    //     default:
    //         val = va_arg(*ap, int);
    //         break;
    // }

    ndigit = num2str(val, tmpbuf, b, s);
    nchar += ndigit;

    if (flag_set(fl, F_ZEROPAD) && p < 0) {
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

    if (w > nchar) {
        if (p > ndigit) {
            npad = w - nchar - (p - ndigit);
        }
        else {
            npad = w - nchar;
        }
        for (i = 0; i < npad; i++) {
            *(buf++) = padch;
        }
        nchar += i;
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

    while (p > ndigit) {
        *(buf++) = '0';
        ndigit++;
    }

    strncpy(buf, tmpbuf, sizeof(tmpbuf));
}
