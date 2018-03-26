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
 * Copyright (C) 2018 Wes Hampson
 *
 * File: bootloader.ld
 * Desc: Linker script for the boot code.
 *----------------------------------------------------------------------------*/

#include "boot.h"

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(entry)

SECTIONS
{
    .stage1 (STAGE1_BASE) :
    {
        *(.stage1)
        . = 0x1FE;
        SHORT(0xAA55)
    }

    .stage2 (STAGE2_BASE) :
    {
        *(.stage2)
        . = ALIGN(SECTOR_SIZE);
    }

    .text (0x8000) :
    {
        __KERNEL_CODE_START = .;
        *(.text)
        . = ALIGN(SECTOR_SIZE);
        __KERNEL_CODE_END = .;
    }
    KERNEL_NUM_SECTORS = (__KERNEL_CODE_END - __KERNEL_CODE_START) / 512;
}
