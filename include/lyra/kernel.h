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

/**
 * Backup EFLAGS register, then clear interrupt flag.
 */
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

/**
 * Restore EFLAGS register.
 */
#define restore_flags(flags)\
__asm__ volatile (          \
    "                       \n\
    push %0                 \n\
    popfl                   \n\
    "                       \
    :                       \
    : "r"(flags)            \
    : "memory", "cc"        \
);

/**
 * Checks whether a given bit is set in a bitfield.
 */
#define flag_set(val, flag) \
    ((val & flag) == flag)

/**
 * Returns the greater value.
 *
 * Adapted from https://stackoverflow.com/a/3437484.
 */
#define max(a, b)           \
({  __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
})

/**
 * Returns the smaller value.
 *
 * Adapted from https://stackoverflow.com/a/3437484.
 */
#define min(a, b)           \
({  __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
})


void clear(void);

/* Nonstandard string functions. */
char * strrev(char *str);
char * itoa(int val, char *str, int base);

void puti(uint32_t i);
void putix(uint32_t i);

int kprintf(const char *fmt, ...);
int vkprintf(const char *fmt, va_list args);

#ifdef __DEBUG
void kprintf_test(void);
#endif

#endif  /* __LYRA_KERNEL_H */
