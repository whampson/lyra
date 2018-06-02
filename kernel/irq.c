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
 *   File: kernel/irq.c
 * Author: Wes Hampson
 *   Desc: Device interrupt handling.
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <lyra/irq.h>
#include <lyra/kernel.h>
#include <drivers/ps2kbd.h>

static int eoi(unsigned int irq_num);

void irq_init(void)
{
    i8259_init();
    irq_enable(IRQ_SLAVE_PIC);
}

int irq_enable(unsigned int irq_num)
{
    if (irq_num >= NUM_IRQ) {
        return -1;
    }

    i8259_unmask(irq_num);
    return 0;
}

int irq_disable(unsigned int irq_num)
{
    if (irq_num >= NUM_IRQ) {
        return -1;
    }

    i8259_mask(irq_num);
    return 0;
}

__attribute__((fastcall))
void do_irq(struct interrupt_frame *regs)
{
    unsigned int irq_num = ~(regs->vec_num);

    switch (irq_num) {
        case IRQ_KEYBOARD:
            ps2kbd_do_irq();
            eoi(irq_num);
            break;
        default:
            kprintf("Unknown IRQ! (%d)\n", irq_num);
            break;
    }
}

static int eoi(unsigned int irq_num)
{
    if (irq_num >= NUM_IRQ) {
        return -1;
    }

    i8259_eoi(irq_num);
    return 0;
}
