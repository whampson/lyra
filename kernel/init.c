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
 * File: init.c
 * Desc: Kernel entry point and initialization.
 *----------------------------------------------------------------------------*/

#include <kernel.h>
#include <descriptor.h>
#include <memory.h>
#include <types.h>

__attribute__((fastcall))
extern void except_de(void);
__attribute__((fastcall))
extern void except_db(void);
__attribute__((fastcall))
extern void system_call(void);

static void ldt_init(void);
static void tss_init(void);
static void idt_init(void);

/**
 * "Fire 'er up, man!"
 */
void kernel_init(void)
{
    clear();
    ldt_init();
    tss_init();
    idt_init();

    // volatile int a = 1;
    // volatile int b = 0;
    // volatile int c = a / b;

    __asm__ volatile ("int $0x80" : : : "memory");

    puts("Recovered from interrupt!\n");

    /* TODO:
        init idt
        init paging
        create __simple__ terminal driver
    */
}

static void idt_init(void)
{
    size_t i;
    idt_gate_t *idt;
    size_t idt_len;
    desc_reg_t idt_ptr;

    idt = (idt_gate_t *) 0x500;
    idt_len = NUM_VEC * sizeof(idt_gate_t);

    for (i = 0; i < NUM_VEC; i++) {
        idt[i].value = 0;
        if (i == 0x80) {
            idt[i].fields.dpl = 0;
            idt[i].fields.present = 1;
            idt[i].fields.reserved = 0;
            idt[i].fields.seg_selector = KERNEL_CS;
            idt[i].fields.type = GATE_TRAP32;
            idt[i].fields.offset_lo = ((uint32_t) system_call) & 0x0000FFFF;
            idt[i].fields.offset_hi = ((uint32_t) system_call) >> 16;
        }
    }

    idt_ptr.fields.base = (uint32_t) idt;
    idt_ptr.fields.limit = (uint16_t) idt_len;
    idt_ptr.fields.padding = 0;
    lidt(idt_ptr);
}

static void ldt_init(void)
{
    desc_reg_t gdt_ptr;
    seg_desc_t *gdt;
    seg_desc_t *ldt_desc;
    uint32_t ldt_base;
    size_t ldt_size;
    int ldt_desc_idx;
    size_t i;

    puts("Initializing the LDT...");

    ldt_base = (uint32_t) ldt;
    ldt_size = sizeof(ldt);

    /* Zero-out the LDT.
       We're not using LDTs on our system, but we need one
       to keep the CPU happy, so just zero it. */
    for (i = 0; i < ldt_size / sizeof(seg_desc_t); i++) {
        ldt[i].value = 0L;
    }

    /* Get GDT pointer */
    sgdt(gdt_ptr);
    gdt = (seg_desc_t *) gdt_ptr.fields.base;

    /* Get LDT descriptor from GDT */
    ldt_desc_idx = get_gdt_index(KERNEL_LDT);
    ldt_desc = &gdt[ldt_desc_idx];

    /* Set up the LDT descriptor and load the LDTR */
    SET_SYS_DESC_PARAMS(ldt_desc, ldt_base, ldt_size, DESC_LDT);
    lldt(KERNEL_LDT);

    puts(" done.\n");
}

static void tss_init(void)
{
    desc_reg_t gdt_ptr;
    seg_desc_t *gdt;
    seg_desc_t *tss_desc;
    uint32_t tss_base;
    size_t tss_size;
    int tss_desc_idx;

    puts("Initializing the TSS...");

    /* Get GDT pointer */
    sgdt(gdt_ptr);
    gdt = (seg_desc_t *) gdt_ptr.fields.base;

    /* Get TSS descriptor from GDT */
    tss_desc_idx = get_gdt_index(KERNEL_TSS);
    tss_desc = &gdt[tss_desc_idx];

    tss_base = (uint32_t) &tss;
    tss_size = sizeof(struct tss_struct);

    /* Set up the TSS descriptor */
    SET_SYS_DESC_PARAMS(tss_desc, tss_base, tss_size, DESC_TSS32);

    /* Populate TSS params and load task register */
    tss.ldt_selector = KERNEL_LDT;
    tss.esp0 = KERNEL_STACK_BASE;
    tss.ss0 = KERNEL_DS;
    ltr(KERNEL_TSS);

    puts(" done.\n");
}
