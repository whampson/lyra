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
