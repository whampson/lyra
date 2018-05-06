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
 * File: kernel.ld
 * Desc: Linker script for the kernel code.
 *       Run through scripts/gen-lds.sh to resolve preprocessor directives.
 *----------------------------------------------------------------------------*/

#include <init.h>

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(ENTRY_POINT)

SECTIONS
{
    .text KERNEL_START :
    {
        LONG(ENTRY_POINT);
        __TEXT_START = .;
        *(.text)
        __TEXT_END = .;
    }

    .data : ALIGN(512)
    {
        __DATA_START = .;
        *(.data)
        __DATA_END = .;
    }

    .bss : ALIGN(512)
    {
        __BSS_START = .;
        *(.bss)
        __BSS_END = .;
    }

    .rodata : ALIGN(512)
    {
        __RODATA_START = .;
        *(.rodata)
        __RODATA_END = .;
    }
}
