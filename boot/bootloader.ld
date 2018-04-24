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
 *       Run through scripts/gen-lds.sh to resolve preprocessor directives.
 *----------------------------------------------------------------------------*/

#include "boot.h"
#include "floppy.h"

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(entry)

SECTIONS
{
    .stage1 STAGE1_START :
        AT (0x0000)
    {
        *(.stage1)
        ASSERT(. <= SECTOR_SIZE - 4, "ERROR: Stage 1 does not fit within bootsector.");
        . = SECTOR_SIZE - 4;
        
        /* Reserve space for kernel image sector count. */
        SHORT(0);

        /* MBR magic number. */
        SHORT(BOOTSECT_MAGIC);
    }

    .stage2 STAGE2_START :
        AT ((STAGE2_SECTOR - 1) * SECTOR_SIZE)
    {
        *(.stage2)
        . = ALIGN(512);     /* Not sure why this only works with 256... */
    }
    __STAGE2_END = .;

    STAGE2_NUM_SECTORS = (__STAGE2_END - STAGE2_START) / SECTOR_SIZE;
}
