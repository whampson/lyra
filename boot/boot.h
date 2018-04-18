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

#ifndef _BOOT_H_
#define _BOOT_H_

/* Starting addresses of global data. */
#define STACK_BASE          0x7C00  /* first item at 0x7BFE, grows towards 0 */

/* Starting addresses of code. */
#define STAGE1_START        0x7C00
#define STAGE2_START        0x0500
#define KERNEL_START_EARLY  0x7E00
#define KERNEL_START        0x400000    /* 4 MiB */

/* Segment selectors. */
#define BOOT_CS             0x10
#define BOOT_DS             0x18

/* Boot sector signature for MBR. */
#define BOOTSECT_MAGIC      0xAA55

/* Disk layout information.
   STAGE2_NUM_SECTORS and KERNEL_NUM_SECTORS defined during linking. */
#define STAGE2_SECTOR       2
#define KERNEL_SECTOR       (STAGE2_SECTOR + STAGE2_NUM_SECTORS)

#endif /* _BOOT_H_ */
