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
 *   File: include/lyra/io.h
 * Author: Wes Hampson
 *   Desc: Early kernel initialization macros; shared between bootloader and
 *         kernel.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_IO_H
#define __LYRA_IO_H

/* BIOS POST code port.
   Reading/writing to this port incurs about a 1us delay, according to various
   sources I've found online.
   We don't really care about BIOS POST codes, to writing to this port for the
   purpose of adding a small delay should pose no harm.
*/
#define IO_DELAY_PORT   0x80

#ifndef __ASM
#include <stdint.h>

/**
 * Read a byte from an I/O port.
 *
 * @param port - the port to read from
 * @return the byte read
 */
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

/**
 * Read a byte from an I/O port, but pause for a short time before reading.
 *
 * @param port - the port to read from
 * @return the byte read
 */
static inline uint8_t inb_p(uint16_t port)
{
    uint8_t data;
    __asm__ volatile (
        "                   \n\
        xorb    %%al, %%al  \n\
        outb    %%al, %w2   \n\
        inb     %w1, %b0    \n\
        "
        : "=a"(data)
        : "d"(port), "n"(IO_DELAY_PORT)
        : "memory", "cc"
    );
    return data;
}


/**
 * Write a byte to an I/O port.
 *
 * @param data - the byte to write
 * @param port - the port to write to
 */
static inline void outb(uint8_t data, uint16_t port)
{
    __asm__ volatile (
        "outb   %b0, %w1"
        :
        : "a"(data), "d"(port)
        : "memory", "cc"
    );
}

/**
 * Write a byte to an I/O port, but pause for a short time after writing.
 *
 * @param data - the byte to write
 * @param port - the port to write to
 */
static inline void outb_p(uint8_t data, uint16_t port)
{
    __asm__ volatile (
        "                   \n\
        outb    %b0, %w1    \n\
        xorb    %%al, %%al  \n\
        outb    %%al, %w2   \n\
        "
        :
        : "a"(data), "d"(port), "n"(IO_DELAY_PORT)
        : "memory", "cc"
    );
}

#endif /* __ASM */

#endif /* __LYRA_IO_H */
