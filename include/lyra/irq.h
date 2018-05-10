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
 * File: irq.h
 * Desc:
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_IRQ_H__
#define __LYRA_IRQ_H__

#define NUM_IRQ         16

#define IRQ_BASE_VEC    0x20

/* IRQ numbers for devices connected to the Intel 8259 PICs. */
#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1
#define IRQ_RTC         8

#ifndef __ASM__

void irq_enable(int irq_num);
void irq_disable(int irq_num);

#endif /* __ASM__ */

#endif /* __LYRA_IRQ_H__ */
