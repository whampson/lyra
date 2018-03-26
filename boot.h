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

#define BOOT_STACK_BASE     0x7C00  /* first item at 0x7BFE, grows towards 0 */

#define STAGE1_BASE         0x7C00
#define STAGE2_BASE         0x7E00
#define KERNEL_TEMP_BASE    0x8000  /* loaded here before prot. mode enabled */

#define FLOPPY_NUM_RETRIES  3
#define STAGE2_NUM_SECTORS  1
#define STAGE2_START_SECTOR 2
#define KERNEL_START_SECTOR (STAGE2_START_SECTOR + STAGE2_NUM_SECTORS)
#define SECTOR_SIZE         512

#endif /* _BOOT_H_ */
