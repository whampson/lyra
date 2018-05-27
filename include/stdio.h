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
 *   File: include/stdint.h
 * Author: Wes Hampson
 *   Desc: Partial implementation of stdio.h from the C Standard Library.
 *         Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdint.h>

#ifndef __NULL_DEFINED__
#define __NULL_DEFINED__
#define NULL    ((void *) 0)
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef uint32_t size_t;
#endif

/* End-of-File indicator. */
#define EOF     (-1)

/**
 * Writes a character to the standard output stream.
 *
 * @param ch - the character to write
 * @return (success) the actual character written
 *         (failure) EOF
 */
int putchar(int ch);

/**
 * Writes a string to the standard output stream.
 *
 * @param str - C-style string to be written
 * @return (success) the number of characters written
 *         (failure) EOF
 */
int puts(const char *str);

#endif /* __STDIO_H__ */