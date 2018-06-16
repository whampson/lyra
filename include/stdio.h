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
 *   File: include/stdio.h
 * Author: Wes Hampson
 *   Desc: Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#ifndef __STDIO_H
#define __STDIO_H

#include <stdarg.h>
#include <stdint.h>

#ifndef __NULL_DEFINED
#define __NULL_DEFINED
#define NULL    ((void *) 0)
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef uint32_t size_t;
#endif

/**
 * Printf-family functions. Most standard format specifiers are supported.
 * See below for a complete list.
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

int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int sprintf(char *str, const char *fmt, ...);
int vsprintf(char *str, const char *fmt, va_list args);
int snprintf(char *str, size_t n, const char *fmt, ...);
int vsnprintf(char *str, size_t n, const char *fmt, va_list args);

#endif /* __STDIO_H */
