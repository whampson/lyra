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
 * File: floppy.h
 * Desc: Floppy disk properties and driver behavior.
 *----------------------------------------------------------------------------*/

#ifndef _FLOPPY_H_
#define _FLOPPY_H_

/* Properties of a 1.4 MiB, 3.5in floppy disks. */
#define HEAD_COUNT          2
#define CYL_COUNT           80
#define SECTOR_COUNT        18
#define SECTOR_SIZE         512

#define RETRY_COUNT         3

#define BIOS_READ_FLOPPY    0x02

#endif /* _FLOPPY_H_ */
