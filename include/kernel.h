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
 * File: kernel.h
 * Desc: Common function prototyyes and macros.
 *----------------------------------------------------------------------------*/

#ifndef __KERNEL_H__
#define __KERNEL_H__

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

void putc(char c);
void puts(const char *s);
void clear(void);

#endif  /* __KERNEL_H__ */
