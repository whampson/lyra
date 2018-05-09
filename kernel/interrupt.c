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
 * File: interrupt.c
 * Desc: IDT initialization.
 *----------------------------------------------------------------------------*/

#include <types.h>
#include <lyra/interrupt.h>
#include <lyra/exception.h>
#include <lyra/descriptor.h>

#define PRIVL_KERNEL 0
#define PRIVL_USER   3

void idt_init(void)
{
    size_t i;
    idt_gate_t *idt;
    size_t idt_len;
    desc_reg_t idt_ptr;

    idt = (idt_gate_t *) 0x500;
    idt_len = NUM_VEC * sizeof(idt_gate_t);

    for (i = 0; i < NUM_VEC; i++) {
        switch (i) {
            case EXCEPT_DE:
                idt[i].fields.dpl = PRIVL_KERNEL;
                idt[i].fields.present = 1;
                idt[i].fields.reserved = 0;
                idt[i].fields.seg_selector = KERNEL_CS;
                idt[i].fields.type = GATE_TRAP32;
                // idt[i].fields.offset_lo = ((uint32_t) except_de) & 0x0000FFFF;
                // idt[i].fields.offset_hi = ((uint32_t) except_de) >> 16;
                break;
            case 0x80:
                idt[i].fields.dpl = 3;
                idt[i].fields.present = 1;
                idt[i].fields.reserved = PRIVL_USER;
                idt[i].fields.seg_selector = KERNEL_CS;
                idt[i].fields.type = GATE_TRAP32;
                // idt[i].fields.offset_lo = ((uint32_t) system_call) & 0x0000FFFF;
                // idt[i].fields.offset_hi = ((uint32_t) system_call) >> 16;
                break;
            default:
                idt[i].value = 0;
                break;
        }
    }

    idt_ptr.fields.base = (uint32_t) idt;
    idt_ptr.fields.limit = (uint16_t) idt_len;
    idt_ptr.fields.padding = 0;
    lidt(idt_ptr);
}