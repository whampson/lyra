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
 *   Desc: Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

/*******************************************************************************
 * Implementation Checklist:
 * Type                 Status              Notes
 * ----                 ------              -----
 * ptrdiff_t            NOT_IMPLEMENTED
 * size_t               COMPLETE
 * max_align_t          NOT_IMPLEMENTED     Part of C11, may not implement.
 * nullptr_t            NOT_IMPLEMENTED     Part of C11, may not implement.
 *
 * Macro Function       Status              Notes
 * --------------       ------              -----
 * offsetof             NOT_IMPLEMENTED
 *
 * Macro Constant       Status              Notes
 * --------------       ------              -----
 * NULL                 COMPLETE
 *
 ******************************************************************************/

#ifndef __STDDEF_H
#define __STDDEF_H

#include <stdint.h>

#ifndef __NULL_DEFINED
#define __NULL_DEFINED
#define NULL    ((void *) 0)
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef uint32_t size_t;
#endif

typedef int32_t ptrdiff_t;

#endif /* __STDDEF_H */
