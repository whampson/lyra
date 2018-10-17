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
 *   File: lyra.ld
 * Author: Wes Hampson
 *   Desc: Linker script for the Lyra kernel code.
 *         Run through scripts/gen-lds.sh to resolve preprocessor directives.
 *----------------------------------------------------------------------------*/

#include <lyra/init.h>

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(KERNEL_ENTRY)

SECTIONS
{
    .text KERNEL_BASE :
    {
        LONG(KERNEL_ENTRY);
        __TEXT_START = .;
        *(.text)
        __TEXT_END = .;
    }

    .data :
    {
        __DATA_START = .;
        *(.data)
        __DATA_END = .;
    }

    .bss :
    {
        __BSS_START = .;
        *(.bss)
        __BSS_END = .;
    }

    .rodata :
    {
        __RODATA_START = .;
        *(.rodata)
        __RODATA_END = .;
    }
    __KERNEL_END = .;

    /*__KERNEL_NUM_SECTORS = (__KERNEL_END - KERNEL_START) / 512;
    ASSERT(__KERNEL_NUM_SECTORS < 256,
        "Error: Kernel is too large for current floppy disk driver!")*/
}
