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
 * File: io.h
 * Desc: I/O macros.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_IO_H__
#define __LYRA_IO_H__

#include <types.h>

static inline uint8_t inb(uint16_t port)
{
    uint8_t data;
    __asm__ volatile (
        "inb    %w1, %b0"
        : "=a"(data)
        : "d"(port)
        : "memory", "cc"
    );
    return data;
}

static inline void outb(uint8_t data, uint16_t port)
{
    __asm__ volatile (
        "outb   %b0, %w1"
        :
        : "a"(data), "d"(port)
        : "memory", "cc"
    );
}

#endif /* __LYRA_IO_H__ */
