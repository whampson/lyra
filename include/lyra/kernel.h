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
#include <lyra/init.h>

#define PRIVL_KERNEL 0
#define PRIVL_USER   3

/**
 * Clear interrupt flag.
 */
#define cli()               \
__asm__ volatile (          \
    "cli"                   \
    :                       \
    :                       \
    : "memory", "cc"        \
);

/**
 * Set interrupt flag.
 */
#define sti()               \
__asm__ volatile (          \
    "sti"                   \
    :                       \
    :                       \
    : "memory", "cc"        \
);

/**
 * Backup EFLAGS register, then clear interrupt flag.
 */
#define cli_save(flags)     \
__asm__ volatile (          \
    "                       \n\
    pushfl                  \n\
    popl %0                 \n\
    cli                     \n\
    "                       \
    : "=r"(flags)           \
    :                       \
    : "memory", "cc"        \
);

/**
 * Restore EFLAGS register.
 */
#define restore_flags(flags)\
__asm__ volatile (          \
    "                       \n\
    push %0                 \n\
    popfl                   \n\
    "                       \
    :                       \
    : "r"(flags)            \
    : "memory", "cc"        \
);

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

/* Functions defined in kprintf.c */

/**
 * Kernel printf() -- prints messages to the kernel console.
 * Works the same as printf() from the C Standard Library, though not all
 * format specifiers are supported because I see no need for them in the context
 * of this kernel. See below.
 *
 * @param fmt - the format specifier
 * @param ... - format arguments
 * @return number of characters printed
 *
 * Format Specifier Prototype:
 *   %[flags][width][.precision][length]specifier
 *
 * specifier:
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
 *     %        - literal '%'
 *
 *   flags:
 *     -        - left justify padding
 *     +        - always print sign
 *     (space)  - print a space if a sign would be printed (unless + specified)
 *     #        - (o,x,X) - prepend (0,0x,0X) for nonzero values
 *     0        - (d,i,o,u,x,X) - left-pad with zeros instead of spaces
 *
 *   width:
 *     (num)    - min. chars to be printed; padded w/ blank spaces or 0
 *     *        - width specified as next argument in arg list
 *
 *   .precision:
 *     (num)    - (d,i,o,u,x,X) - min. digits to be written; pad w/ zeros
 *                (s) - max. chars to be written
 *     *        - precision specified as next argument in arg list
 *
 *   length:
 *      The length field is NOT_SUPPORTED at this time. May change if absolutely
 *      needed.
 */
int kprintf(const char *fmt, ...);
int vkprintf(const char *fmt, va_list args);

int atoi(const char *str);

#endif  /* __LYRA_KERNEL_H */
