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

#include <lyra/kernel.h>

/**
 * Kernel printf(). Use this to print to the kernel console.
 * Most of the same rules as printf() apply, though some features have been left
 * out because I see no need for them in the context of this kernel. Details
 * below.
 *
 * Format Specifier Breakdown:
 *   %[flags][width][.precision][length]specifier
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
 *   length:      d i           u o x X                 c       s       p
 *     (none)   - int           unsigned int            int     char*   void*
 *     hh       - signed char   unsigned char
 *     h        - short int     unsigned short int
 *     l        - long int      unsigned long int       wint_t  wchar_t*
 *     ll       - long long int unsigned long long int
 *     j        - intmax_t      uintmax_t
 *     z        - size_t        size_t
 *     t        - ptrdiff_t     ptrdiff_t
 *     L        - NOT_SUPPORTED
 *     NOTE: 'wint_t' and 'wchar_t' are not supported yet. Length conversions to
 *           these types will be 'long int', and 'long int*' respectively.
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
 */
int kprintf(const char * fmt, ...)
{
    int retval;
    va_list ap;

    va_start(ap, fmt);
    retval = vkprintf(fmt, ap);
    va_end(ap);

    return retval;
}

int vkprintf(const char * fmt, va_list args)
{
    /* OBSERVATIONS:
        - keeps reading flags until a # or '0' not found
            - this means both # and 0 may be included int the same fmt spec
        - invalid fmt specs are simply printed verbatim
        - 'p' automatically adds '0x' prefix
    */

    return 0;
}
