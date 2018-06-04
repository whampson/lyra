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

#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>

#ifndef __NULL_DEFINED
#define __NULL_DEFINED
#define NULL    ((void *) 0)
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef uint32_t size_t;
#endif

/* TODO: These can definitely be optimized if need-be. */

static inline char * strcpy(char *dest, const char *src)
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

static inline char * strncpy(char *dest, const char *src, size_t n)
{
    register size_t i;
    register char c;

    if (src == NULL || dest == NULL) {
        return dest;
    }

    i = 0;
    while (i < n && (c = src[i]) != '\0') {
        dest[i++] = c;
    }
    dest[i] = '\0';

    return dest;
}

static inline size_t strlen(const char *str)
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
static inline char * strrev(char *str)
{
    char *str_orig;
    register char *a;
    register char *b;

    if (str == NULL) {
        return NULL;
    }

    str_orig = str;
    a = str;
    b = str + strlen(str) - 1;

    while (b > a) {
        *a ^= *b;
        *b ^= *a;
        *a ^= *b;
        a++;
        b--;
    }

    return str_orig;
}

#endif /* __STRING_H */
