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
 * File: types.h
 * Desc: Definitions for commonly-used types.
 *----------------------------------------------------------------------------*/

#ifndef __TYPES_H__
#define __TYPES_H__

#define NULL 0

/* i386 integer types */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int int16_t;
typedef unsigned short int uint16_t;
typedef long int int32_t;
typedef unsigned long int uint32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;

typedef unsigned long int size_t;

#endif /* __TYPES_H__ */
