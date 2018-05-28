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
 *   File: include/stdarg.h
 * Author: Wes Hampson
 *   Desc: Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#ifndef __STDARG_H
#define __STDARG_H

/**
 * Holds the information need by va_start, va_arg, va_copy, and va_end.
 */
typedef char * va_list;

/**
 * Initializes a va_list, which enables access to variadic function arguments.
 *
 * @param ap     - va_list to initialize
 * @param paramN - the named parapter preceding the first variable parameter
 */
#define va_start(ap, paramN)                \
    (ap = (va_list) ((char *) &paramN + sizeof(paramN)))

/**
 * Creates a copy of the variadic function arguments.
 *
 * @param dest - va_list to initialize
 * @param src  - the va_list that will be used to initialize 'dest'
 */
#define va_copy(dest, src)                  \
    (dest = src)

/**
 * Accesses the next variadic function argument.
 *
 * @param   ap - va_list instance
 * @param type - the type of the next parameter
 * @return the value of the next parameter
 */
#define va_arg(ap, type)                    \
    (ap = (va_list) (ap + sizeof(type)),    \
    ((type *) ap)[-1])

/**
 * Ends traversal of the variadic function arguments.
 *
 * @param ap - the va_list instance to clean up
 */
#define va_end(ap)                          \
    (ap = (void *) 0)

#endif /* __STDARG_H */
