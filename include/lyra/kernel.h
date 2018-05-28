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
 *   File: include/lyra/kernel.h
 * Author: Wes Hampson
 *   Desc: Commonly-used function prototypes and macros.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_KERNEL_H
#define __LYRA_KERNEL_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <lyra/init.h>

#define PRIVL_KERNEL 0
#define PRIVL_USER   3

/**
 * Clear interrupt flag.
 */
#define cli()               \
__asm__ volatile (          \
    "cli"                   \
    :                       \
    :                       \
    : "memory", "cc"        \
);

/**
 * Set interrupt flag.
 */
#define sti()               \
__asm__ volatile (          \
    "sti"                   \
    :                       \
    :                       \
    : "memory", "cc"        \
);

#define cli_save(flags)     \
__asm__ volatile (          \
    "                       \n\
    pushfl                  \n\
    popl %0                 \n\
    cli                     \n\
    "                       \
    : "=r"(flags)           \
    :                       \
    : "memory", "cc"        \
);

#define set_flags(flags)    \
__asm__ volatile (          \
    "                       \n\
    push %0                 \n\
    popfl                   \n\
    "                       \
    :                       \
    : "r"(flags)            \
    : "memory", "cc"        \
);

void clear(void);

char * itoa(unsigned int value, char *buf, int radix);
char * strrev(char *s);
size_t strlen(const char *s);
void puti(uint32_t i);
void putix(uint32_t i);

int kprintf(const char * fmt, ...);
int vkprintf(const char * fmt, va_list args);

#endif  /* __LYRA_KERNEL_H */
