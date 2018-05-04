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
 * File: boot.h
 * Desc: Important memory addresses and other macros used during initial
 *       bootstrapping.
 *----------------------------------------------------------------------------*/

#ifndef _BOOT_H_
#define _BOOT_H_

/* Starting addresses of global data. */
#define STACK_BASE              0x7C00  /* first item at 0x7BFE, grows to 0 */

/* Starting addresses of code. */
#define STAGE1_START            0x7C00
#define STAGE2_START            0x0600
#define KERNEL_START_EARLY      0x7E00
#define KERNEL_START            0x400000

/* Segment selectors for protected mode. */
#define BOOT_CS                 0x10
#define BOOT_DS                 0x18

/* Boot sector signature for MBR. */
#define BOOTSECT_MAGIC          0xAA55

/* Disk layout information.
   The following symbols are defined during linking and are stored on-disk
   (see bootloader.ld):
       STAGE2_SECTOR        -- first sector of stage 2
       STAGE2_NUM_SECTORS   -- number of sectors used by stage 2 code/data
       KERNEL_SECTOR        -- first sector of kernel image
       KERNEL_NUM_SECTORS   -- number of sectors used by kernel image
*/

#endif /* _BOOT_H_ */
