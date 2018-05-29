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
 *   File: kernel/kprintf_tests.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#ifdef __DEBUG

#include <lyra/kernel.h>

void kprintf_test(void)
{
    char *ptr = (char *) 0xb8000;

    kprintf("Hello, world!\n");
    kprintf("Literal percent sign: %%\n");
    kprintf("An octal integer (33): %o\n", 27);
    kprintf("A decimal integer (42): %d\n", 42);
    kprintf("A hex integer (BEEF): %x\n", 0xBEEF);
    kprintf("A single character (q): %c\n", 'q');
    kprintf("A string (foo): %s\n", "foo");
    kprintf("A pointer: %p\n", ptr);

    kprintf("A vector: <%d, %d, %d>\n", 45, -537, 17);
    kprintf("Madlib: A %s went to the store and bought %i %s!\n",
            "canary", 76, "limes");
}

#else
extern int __dummy;
#endif /* __DEBUG */
