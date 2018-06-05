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
 *   File: mem/memory.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#include <lyra/kernel.h>
#include <lyra/memory.h>

/* Page directory base address */
#define PD_BASE     0x1000

#define PG_BIT      (1 << 31)   /* CR0 - enable paging */
#define PSE_BIT     (1 << 4)    /* CR4 - allow for 4 MiB pages */

static void paging_enable(void);

/* ===== Intel i386 Paging Structures ===== */
/* See Chapter 3 of the Intel Software Developers Manual, Volume 3 for more
   information about i386 memory paging structures.
*/

/* Page Directory Entry (PDE) for a 4 MiB page. */
typedef union {
    struct {
        uint32_t p          : 1;  /* present */
        uint32_t rw         : 1;  /* read/write; 1 = r/w, 0 = read only */
        uint32_t us         : 1;  /* user/supervisor; 1 = user */
        uint32_t pwt        : 1;  /* page-level write policy; 1 = WT, 0 = WB */
        uint32_t pcd        : 1;  /* page-level cache disabled */
        uint32_t a          : 1;  /* accessed; has been read or written */
        uint32_t d          : 1;  /* dirty; has been written to */
        uint32_t ps         : 1;  /* page size; 1 = 4 MiB */
        uint32_t g          : 1;  /* global; don't flush from TLB */
        uint32_t avail      : 3;  /* available for misc. use */
        uint32_t pat        : 1;  /* page attr. table index */
        uint32_t reserved   : 9;  /* reserved bits; set to 0 */
        uint32_t base_addr  : 10; /* page base address (4 MiB aligned) */
    } fields;
    uint32_t value;
} pde4m_t;

/* Page Directory Entry (PDE) for a page table of 4 KiB pages. */
typedef union {
    struct {
        uint32_t p          : 1;  /* (same descriptions as above) */
        uint32_t rw         : 1;
        uint32_t us         : 1;
        uint32_t pwt        : 1;
        uint32_t pcd        : 1;
        uint32_t a          : 1;
        uint32_t d          : 1;
        uint32_t ps         : 1;
        uint32_t g          : 1;
        uint32_t avail      : 3;
        uint32_t base_addr  : 20; /* page table base address (4 KiB aligned) */
    } fields;
    uint32_t value;
} pde4k_t;

/* Page Table Entry (PTE) for a 4 KiB page. */
typedef union {
    struct {
        uint32_t p          : 1;  /* (same descriptions as above) */
        uint32_t rw         : 1;
        uint32_t us         : 1;
        uint32_t pwt        : 1;
        uint32_t pcd        : 1;
        uint32_t a          : 1;
        uint32_t d          : 1;
        uint32_t pat        : 1;
        uint32_t g          : 1;
        uint32_t avail      : 3;
        uint32_t base_addr  : 20; /* page base address (4 KiB aligned) */
    } fields;
    uint32_t value;
} pte_t;

void mem_init(void)
{
    kprintf("Initializing memory...");

    pde4m_t *page_dir = (pde4m_t *) PD_BASE;
    for (int i = 0; i < 1024; i++) {
        page_dir[i].value = 0;
    }

    page_dir[0].fields.p = 1;
    page_dir[0].fields.rw = 1;
    page_dir[0].fields.ps = 1;
    page_dir[0].fields.g = 1;
    page_dir[0].fields.base_addr = 0;

    paging_enable();
    kprintf(" done.\n");
}

void flush_tlb(void)
{
    __asm__ volatile (
        "                           \n\
        movl    %%cr3, %%eax        \n\
        movl    %%eax, %%cr3        \n\
        "
        : /* no outputs */
        : /* no inputs */
    );
}

static void paging_enable(void)
{
    __asm__ volatile (
        "                       \n\
        movl    %%cr4, %%eax    \n\
        orl     %0, %%eax       \n\
        movl    %%eax, %%cr4    \n\
        movl    %1, %%cr3       \n\
        movl    %%cr0, %%eax    \n\
        orl     %2, %%eax       \n\
        movl    %%eax, %%cr0    \n\
        "
        : /* no outputs */
        : "b"(PSE_BIT), "c"(PD_BASE), "d"(PG_BIT)
    );
}
