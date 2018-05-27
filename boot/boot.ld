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
 *   File: boot/boot.ld
 * Author: Wes Hampson
 *   Desc: Linker script for the boot code.
 *         Run through scripts/gen-lds.sh to resolve preprocessor directives.
 *----------------------------------------------------------------------------*/

#include "boot.h"
#include "floppy.h"

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(BOOT_ENTRY)

SECTIONS
{
    .stage1 STAGE1_START : AT (0x0000)
    {
        __DISK_INFO_START = SECTOR_SIZE - 16;
        __MAGIC_START = SECTOR_SIZE - 2;

        *(.stage1)

        ASSERT(. <= __DISK_INFO_START,
            "ERROR: Stage 1 code does not fit within bootsector!");

        . = __DISK_INFO_START;
        STAGE2_SECTOR = .;
        SHORT(__STAGE2_SECTOR)
        STAGE2_NUM_SECTORS = .;
        SHORT(__STAGE2_NUM_SECTORS)
        KERNEL_SECTOR = .;
        SHORT(__KERNEL_SECTOR)
        KERNEL_NUM_SECTORS = .;
        SHORT(0)    /* Written during final creation of disk image. */

        FILL(0x5748)

        /* MBR magic number. */
        . = __MAGIC_START;
        SHORT(BOOTSECT_MAGIC)
    }

    __STAGE2_SECTOR = 2;

    .stage2 STAGE2_START : AT ((__STAGE2_SECTOR - 1) * SECTOR_SIZE)
    {
        *(.stage2)
        . = ALIGN(SECTOR_SIZE);
        __STAGE2_END = .;
    }

    __STAGE2_NUM_SECTORS = (__STAGE2_END - STAGE2_START) / SECTOR_SIZE;
    __KERNEL_SECTOR = __STAGE2_SECTOR + __STAGE2_NUM_SECTORS;

    ASSERT(__STAGE2_NUM_SECTORS < SECTOR_COUNT - 1,
        "Error: Stage 2 too large for current floppy disk driver!")
}
