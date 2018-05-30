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
 *   File: include/string.h
 * Author: Wes Hampson
 *   Desc: Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#include <string.h>
#include <stdint.h>

char * strcpy(char *dest, const char *src)
{
    register int i;

    if (src == NULL || dest == NULL) {
        return dest;
    }

    i = -1;
    do {
        i++;
        dest[i] = src[i];
    } while (src[i] != '\0');

    return dest;
}

char * strncpy(char *dest, const char *src, size_t n)
{
    register size_t i;
    register int c;

    if (src == NULL || dest == NULL) {
        return dest;
    }

    for (i = 0; i < n; i++) {
        if (i == 0 || c != '\0') {
            c = src[i];
        }
        dest[i] = c;
    }

    return dest;
}

size_t strlen(const char *str)
{
    register size_t len;

    if (str == NULL) {
        return 0;
    }

    len = 0;
    while (str[len] != '\0') {
        len++;
    }

    return len;
}

/* Non-standard but useful. */
char * strrev(char *str)
{
    register char c;
    register size_t len;
    register size_t i;

    len = strlen(str);

    for (i = 0; i < len / 2; i++) {
        c = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = c;
    }

    return str;
}

/* Non-standard but useful. */
char * itoa(int64_t val, char *str, int base)
{
    static char lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char *str_base;
    size_t i;
    int sign;
    uint64_t uval;

    if (str == NULL) {
        return str;
    }

    if (base < 2 || base > 36) {
        str[0] = '\0';
        return str;
    }

    /* Special case for zero */
    if (val == 0) {
        str = "0";
        return str;
    }

    str_base = str;

    /* Check for negative */
    sign = (val < 0);
    if (sign && base == 10) {
        val *= -1;
    }
    uval = (uint64_t) val;

    while (uval > 0) {
        i = (size_t) uval % base;
        *str = lookup[i];
        str++;
        uval /= base;
    }

    if (sign && base == 10) {
        *(str++) = '-';
    }
    *str = '\0';

    return strrev(str_base);
}
