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

#include <lyra/kernel.h>
#include <lyra/descriptor.h>
#include <lyra/interrupt.h>
#include <lyra/irq.h>
#include <lyra/io.h>
#include <lyra/memory.h>

/* The TSS. */
static struct tss_struct tss = { 0 };

/* The LDT.
   We're not using LDTs on our system, but we need one to keep the CPU happy. */
static seg_desc_t ldt[2];

static void ldt_init(void);
static void tss_init(void);
static void i8259_init(void);

/**
 * "Fire 'er up, man!"
 */
void kernel_init(void)
{
    clear();

    ldt_init();
    tss_init();
    idt_init();

    puts("Initializing PIC...\n");
    i8259_init();
    irq_enable(IRQ_KEYBOARD);

    puts("Enabling interrupts...\n");
    sti();

    //__asm__ volatile ("int $0x21" : : : "memory");

    /* TODO:
        init paging
        create __simple__ terminal driver
    */

   puts("Halting system...");
}

#define ICW1        0x11
#define ICW2_MASTER 0x20
#define ICW2_SLAVE  0x28
#define ICW3_MASTER 0x04
#define ICW3_SLAVE  0x02
#define ICW4        0x01

#define PIC0_CMD    0x20
#define PIC0_DATA   0x21
#define PIC1_CMD    0xA0
#define PIC1_DATA   0xA1

static void io_delay(void)
{
    volatile short i;

    i = 0x0FFF;
    while (--i > 0);
}

static void i8259_init(void)
{
    uint8_t mask0, mask1;

    mask0 = inb(PIC0_DATA);
    mask1 = inb(PIC1_DATA);

    outb(PIC0_CMD, ICW1);
    io_delay();
    outb(PIC0_DATA, ICW2_MASTER);
    io_delay();
    outb(PIC0_DATA, ICW3_MASTER);
    io_delay();
    outb(PIC0_DATA, ICW4);
    io_delay();

    outb(PIC1_CMD, ICW1);
    io_delay();
    outb(PIC1_DATA, ICW2_SLAVE);
    io_delay();
    outb(PIC1_DATA, ICW3_SLAVE);
    io_delay();
    outb(PIC1_DATA, ICW4);
    io_delay();

    outb(PIC0_DATA, mask0);
    outb(PIC1_DATA, mask1);

    irq_enable(0x02);
}

static void ldt_init(void)
{
    seg_desc_t *gdt;
    seg_desc_t *ldt_desc;
    uint32_t ldt_base;
    size_t ldt_size;
    int ldt_desc_idx;
    size_t i;

    puts("Initializing LDT...");

    ldt_base = (uint32_t) ldt;
    ldt_size = sizeof(ldt);

    /* Zero-out the LDT. */
    for (i = 0; i < ldt_size / sizeof(seg_desc_t); i++) {
        ldt[i].value = 0L;
    }

    /* Get LDT descriptor from GDT */
    gdt = (seg_desc_t *) GDT_BASE;
    ldt_desc_idx = get_gdt_index(KERNEL_LDT);
    ldt_desc = &gdt[ldt_desc_idx];

    /* Set up the LDT descriptor and load the LDTR */
    SET_SYS_DESC_PARAMS(ldt_desc, ldt_base, ldt_size, DESC_LDT);
    lldt(KERNEL_LDT);

    puts(" done.\n");
}

static void tss_init(void)
{
    seg_desc_t *gdt;
    seg_desc_t *tss_desc;
    uint32_t tss_base;
    size_t tss_size;
    int tss_desc_idx;

    puts("Initializing TSS...");

    /* Get TSS descriptor from GDT */
    gdt = (seg_desc_t *) GDT_BASE;
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
