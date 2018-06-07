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

/* TODO: Some of these can definitely be optimized if need-be. */

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

static inline char * strcat(char *dest, const char *src)
{
    char *end;

    end = dest + strlen(dest);
    strcpy(end, src);

    return dest;
}

static inline char * strncat(char *dest, const char *src, size_t n)
{
    char *end;

    end = dest + strlen(dest);
    strncpy(end, src, n);

    return dest;
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

static inline void * memset(void *dest, int c, size_t n)
{
    unsigned char ch;
    ch = (unsigned char) c;

    __asm__ volatile (
        "                               \n\
    .memset_top%=:                      \n\
        testl   %%ecx, %%ecx            \n\
        jz      .memset_done%=          \n\
        testl   $3, %%edi               \n\
        jz      .memset_aligned%=       \n\
        movb    %%al, (%%edi)           \n\
        addl    $1, %%edi               \n\
        subl    $1, %%ecx               \n\
    .memset_aligned%=:                  \n\
        movw    %%ds, %%dx              \n\
        movw    %%dx, %%es              \n\
        movl    %%ecx, %%edx            \n\
        shrl    $2, %%ecx               \n\
        andl    $3, %%edx               \n\
        cld                             \n\
        rep     stosl                   \n\
    .memset_bottom%=:                   \n\
        testl   %%edx, %%edx            \n\
        jz      .memset_done%=          \n\
        movb    %%al, (%%edi)           \n\
        addl    $1, %%edi               \n\
        subl    $1, %%edx               \n\
    .memset_done%=:                     \n\
        "
        :
        : "D"(dest), "a"(ch << 24 | ch << 16 | ch << 8 | ch), "c"(n)
        : "edx", "memory", "cc"
    );

    return dest;
}

static inline void * memmove(void *dest, const void *src, size_t n)
{
    __asm__ volatile (
        "                               \n\
        movw    %%ds, %%dx              \n\
        movw    %%dx, %%es              \n\
        cld                             \n\
        cmpl    %%edi, %%esi            \n\
        jae     .memmove_start%=        \n\
        leal    -1(%%esi, %%ecx), %%esi \n\
        leal    -1(%%edi, %%ecx), %%edi \n\
        std                             \n\
    .memmove_start%=:                   \n\
        rep     movsb                   \n\
        "
        :
        : "D"(dest), "S"(src), "c"(n)
        : "edx", "cc", "memory"
    );

    return dest;
}

#endif /* __STRING_H */
