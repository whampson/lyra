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
#include <lyra/kernel.h>

#define SET_IDT_ENTRY(desc, in_use, privilege, gate_type, func) \
{                                                               \
    desc.fields.reserved = 0;                                   \
    desc.fields.present = in_use;                               \
    desc.fields.type = gate_type;                               \
    desc.fields.seg_selector = KERNEL_CS;                       \
    desc.fields.dpl = privilege;                                \
    desc.fields.offset_lo = func & 0x0000FFFF;                  \
    desc.fields.offset_hi = func >> 16;                         \
}

/* All exception handler stubs. */
static const intr_handler_stub exception_stubs[NUM_EXCEPT] = {
    stub_except_de, stub_except_db, stub_except_nmi, stub_except_bp,
    stub_except_of, stub_except_br, stub_except_ud, stub_except_nm,
    stub_except_df, NULL,           stub_except_ts, stub_except_np,
    stub_except_ss, stub_except_gp, stub_except_pf, NULL,
    stub_except_mf, stub_except_ac, stub_except_mc, stub_except_xf,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL
};

void idt_init(void)
{
    size_t i;
    idt_gate_t *idt;
    size_t idt_len;
    desc_reg_t idt_ptr;

    int in_use;
    int privl;
    int type;
    intr_handler_stub stub;

    idt = (idt_gate_t *) IDT_BASE;
    idt_len = NUM_VEC * sizeof(idt_gate_t);

    /* Populate IDT entries */
    for (i = 0; i < NUM_VEC; i++) {
        in_use = 0;
        privl = PRIVL_KERNEL;
        type = GATE_INTR32;
        stub = NULL;

        if (i < NUM_EXCEPT) {
            in_use = 1;
            privl = PRIVL_KERNEL;
            type = GATE_TRAP32;
            stub = exception_stubs[i];
        }
        else if (i == SYSCALL_VEC) {
            in_use = 1;
            privl = PRIVL_USER;
            type = GATE_TRAP32;
            stub = stub_syscall;
        }

        SET_IDT_ENTRY(idt[i], in_use, privl, type, (uint32_t) stub)
    }

    /* Load IDTR */
    idt_ptr.fields.base = (uint32_t) idt;
    idt_ptr.fields.limit = (uint16_t) idt_len;
    idt_ptr.fields.padding = 0;
    lidt(idt_ptr);
}