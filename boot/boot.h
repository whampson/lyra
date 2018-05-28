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
 *   File: boot/boot.h
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#ifndef __BOOT_H
#define __BOOT_H

#include <lyra/init.h>

/* Starting addresses of global data. */
#define BOOT_STACK_BASE         0x7C00  /* first item at 0x7BFE, grows to 0 */

/* Starting addresses of code. */
#define BOOT_ENTRY              entry
#define STAGE1_START            0x7C00
#define STAGE2_START            0x1000
#define KERNEL_START_EARLY      0x7E00

/* Boot sector signature for MBR. */
#define BOOTSECT_MAGIC          0xAA55

/* Protected Mode enable bit */
#define PE_BIT                  0x01

/* Disk layout information.
   The following symbols are defined during linking and are stored on-disk
   (see bootloader.ld):
       STAGE2_SECTOR        -- first sector of stage 2
       STAGE2_NUM_SECTORS   -- number of sectors used by stage 2 code/data
       KERNEL_SECTOR        -- first sector of kernel image
       KERNEL_NUM_SECTORS   -- number of sectors used by kernel image
*/

#endif /* __BOOT_H */
