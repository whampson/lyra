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
 *   Desc: Partial implementation of stdint.h from the C Standard Library.
 *         Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#ifndef __STDINT_H__
#define __STDINT_H__

/* i386 signed integer minimum values */
#define INT8_MIN    (-0x7F - 1)
#define INT16_MIN   (-0x7FFF - 1)
#define INT32_MIN   (-0x7FFFFFFF - 1)
#define INT64_MIN   (-0x7FFFFFFFFFFFFFFFLL - 1)
#define INTPTR_MIN  INT32_MIN
#define INTMAX_MIN  INT64_MIN

/* i386 signed integer maximum values */
#define INT8_MAX    0x7F
#define INT16_MAX   0x7FFF
#define INT32_MAX   0x7FFFFFFF
#define INT64_MAX   0x7FFFFFFFFFFFFFFFLL
#define INTPTR_MAX  INT32_MAX
#define INTMAX_MAX  INT64_MAX

/* i386 unsigned integer maximum values */
#define UINT8_MAX    0xFF
#define UINT16_MAX   0xFFFF
#define UINT32_MAX   0xFFFFFFFFU
#define UINT64_MAX   0xFFFFFFFFFFFFFFFFULL
#define UINTPTR_MAX  UINT32_MAX
#define UINTMAX_MAX  UINT64_MAX

/* i386 signed integer types */
typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed long int         int32_t;
typedef signed long long int    int64_t;
typedef int32_t                 intptr_t;
typedef int64_t                 intmax_t;

/* i386 unsigned integer types */
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned long int       uint32_t;
typedef unsigned long long int  uint64_t;
typedef uint32_t                uintptr_t;
typedef uint64_t                uintmax_t;

#endif /* __STDINT_H__ */
