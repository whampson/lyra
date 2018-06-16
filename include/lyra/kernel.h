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
 *   File: include/lyra/kernel.h
 * Author: Wes Hampson
 *   Desc: Commonly-used and convenient function prototypes and macros.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_KERNEL_H
#define __LYRA_KERNEL_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <lyra/init.h>

extern const char * const OS_NAME;

#define PRIVL_KERNEL 0
#define PRIVL_USER   3

/**
 * Returns the greater value.
 *
 * Adapted from https://stackoverflow.com/a/3437484.
 */
#define max(a, b)           \
do {                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
} while (0)

/**
 * Returns the smaller value.
 *
 * Adapted from https://stackoverflow.com/a/3437484.
 */
#define min(a, b)           \
do {                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
} while (0)

/**
 * Swaps two values quickly.
 */
#define swap(a, b)          \
do {                        \
    a ^= b;                 \
    b ^= a;                 \
    a ^= b;                 \
} while(0)

/**
 * Checks whether a given bit is set in a bitfield.
 */
#define flag_set(val, flag) \
    ((val & flag) == flag)

/**
 * Negates an int.
 */
static inline int negate(int x)
{
    return ~x + 1;
}

#define kprintf printf  /* Probably going to be removed later... */



int atoi(const char *str);

#endif  /* __LYRA_KERNEL_H */
