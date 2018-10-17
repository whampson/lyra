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
 *   File: include/lyra/init.h
 * Author: Wes Hampson
 *   Desc: Early kernel initialization macros; shared between bootloader and
 *         kernel.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_INIT_H
#define __LYRA_INIT_H

#define KERNEL_ENTRY        kernel_init
#define KERNEL_BASE_SEG     0x1000      /* 64 KiB */
#define KERNEL_BASE_OFF     0x0000
#define KERNEL_BASE         ((KERNEL_BASE_SEG << 4) + KERNEL_BASE_OFF)
#define KERNEL_STACK        0x80000     /* 512 KiB */
#define GDT_BASE            0xA000
#define IDT_BASE            0xA400

#endif /* __LYRA_INIT_H */
