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
#include "floppy.h"

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(entry)

SECTIONS
{
    .stage1 STAGE1_START :
        AT ( 0x00 )
    {
        *(.stage1)
        . = SECTOR_SIZE - 2;
        SHORT(BOOTSECT_MAGIC)
    }

    .stage2 STAGE2_START :
        AT ( (STAGE2_SECTOR - 1) * SECTOR_SIZE )
    {
        *(.stage2)
        . = ALIGN(256);     /* Not sure why this only works with 256... */
    }
    __STAGE2_END = .;

    STAGE2_NUM_SECTORS = (__STAGE2_END - STAGE2_START) / SECTOR_SIZE;
    KERNEL_NUM_SECTORS = 2;
}
