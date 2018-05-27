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
 *   File: include/stddef.h
 * Author: Wes Hampson
 *   Desc: Partial implementation of stddef.h from the C Standard Library.
 *         Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <stdint.h>

#ifndef __NULL_DEFINED
#define __NULL_DEFINED
#define NULL    ((void *) 0)
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef uint32_t size_t;
#endif

#endif /* __STDDEF_H__ */