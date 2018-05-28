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
 *   File: include/lyra/descriptor.h
 * Author: Wes Hampson
 *   Desc: i386 descriptor structure definitions and macros.
 *----------------------------------------------------------------------------*/

#ifndef __LYRA_DESC_H
#define __LYRA_DESC_H

/* Segment selectors */
#define KERNEL_CS   0x10
#define KERNEL_DS   0x18
#define USER_CS     0x23
#define USER_DS     0x2B
#define KERNEL_TSS  0x30
#define KERNEL_LDT  0x38

/* Code/data segment descriptor types.
   See section 3.4.3 of the Intel Software Developers Manual, Volume 3
   for more information.
*/
#define SEG_DESC_RO             0x00        /* read-only */
#define SEG_DESC_RO_EXPANDDOWN  0x04        /* read-only, expand down */
#define SEG_DESC_RW             0x02        /* read/write */
#define SEG_DESC_RW_EXPANDDOWN  0x06        /* read/write, expand down */
#define SEG_DESC_XO             0x08        /* execute-only */
#define SEG_DESC_XO_CONFORMING  0x0C        /* execute-only, conforming */
#define SEG_DESC_XR             0x0A        /* execute/read */
#define SEG_DESC_XR_CONFORMING  0x0D        /* execute/read, conforming */

/* System descriptor types.
   See section 3.5 of the Intel Software Developers Manual, Volume 3
   for more information.
*/
#define GATE_TASK   0x05
#define GATE_CALL16 0x04
#define GATE_CALL32 0x0C
#define GATE_INTR16 0x06
#define GATE_INTR32 0x0E
#define GATE_TRAP16 0x07
#define GATE_TRAP32 0x0F
#define DESC_LDT    0x02
#define DESC_TSS16  0x01
#define DESC_TSS32  0x09

#ifndef __ASM__
#include <stdint.h>

/* Descriptor register structure (for GDTR and IDTR). */
typedef union desc_reg {
    struct reg_fields {
        uint64_t limit      : 16;       /* table length in bytes */
        uint64_t base       : 32;       /* table base address */
        uint64_t padding    : 16;       /* padding, set to 0 */
    } fields;
    uint64_t value;
} desc_reg_t;

/* IDT entry. */
typedef union idt_gate {
    struct gate_fields {
        uint64_t offset_lo      : 16;   /* bits 15:0 of function address */
        uint64_t seg_selector   : 16;   /* code segment selector */
        uint64_t reserved       : 8;    /* reserved, set to 0 */
        uint64_t type           : 5;    /* gate type (see above macros) */
        uint64_t dpl            : 2;    /* descriptor privilege level */
        uint64_t present        : 1;    /* segment present flag */
        uint64_t offset_hi      : 16;   /* bits 31:16 of function address */
    } fields;
    uint64_t value;
} idt_gate_t;

/* GDT/LDT entry. */
typedef union seg_desc {
    struct segment_fields {
        uint64_t limit_lo       : 16;   /* bits 15:0 of segment size */
        uint64_t base_lo        : 24;   /* bits 23:0 of segment base */
        uint64_t seg_type       : 4;    /* segment type (see above macros) */
        uint64_t desc_type      : 1;    /* 0 = system, 1 = code/data */
        uint64_t dpl            : 2;    /* descriptor privilege level */
        uint64_t present        : 1;    /* segment present flag */
        uint64_t limit_hi       : 4;    /* bits 19:16 of segment size */
        uint64_t avail          : 1;    /* available for programmer use */
        uint64_t reserved       : 1;    /* reserved, set to 0 */
        uint64_t op_size        : 1;    /* operation size; 1 = 32 bit */
        uint64_t granularity    : 1;    /* limit gran.; 0 = byte, 1 = 4KiB */
        uint64_t base_hi        : 8;    /* bits 31:24 of segment base */
    } fields;
    uint64_t value;
} seg_desc_t;

/* Task State Segment (TSS) structure. */
struct tss_struct {
    uint16_t prev_task;
    uint16_t reserved0;

    uint32_t esp0;
    uint16_t ss0;
    uint16_t reserved1;

    uint32_t esp1;
    uint16_t ss1;
    uint16_t reserved2;

    uint32_t esp2;
    uint16_t ss2;
    uint16_t reserved3;

    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t reserved4;

    uint16_t cs;
    uint16_t reserved5;

    uint16_t ss;
    uint16_t reserved6;

    uint16_t ds;
    uint16_t reserved7;

    uint16_t fs;
    uint16_t reserved8;

    uint16_t gs;
    uint16_t reserved9;

    uint16_t ldt_selector;
    uint16_t reserved10;

    uint16_t debug_trap : 1;
    uint16_t reserved11 : 15;
    uint16_t io_base_addr;
};

/**
 * Load the Interrupt Descriptor Table Register.
 *
 * @param desc - a desc_reg_t structure
 */
#define lidt(desc)          \
__asm__ volatile (          \
    "lidt %0"               \
    :                       \
    : "g"(desc)             \
    : "memory", "cc"        \
);

/**
 * Load the Local Descriptor Table Register.
 *
 * @param selector - a 16-bit segment selector for the LDT
 */
#define lldt(selector)      \
__asm__ volatile (          \
    "lldt %w0"              \
    :                       \
    : "r"(selector)         \
    : "memory", "cc"        \
);

/**
 * Load the Task Register.
 *
 * @param selector - a 16-bit segment selector for the TSS
 */
#define ltr(selector)       \
__asm__ volatile (          \
    "ltr %w0"               \
    :                       \
    : "r"(selector)         \
    : "memory", "cc"        \
);

/**
 * Sets up a seg_desc_t structure for use as a system descriptor.
 *
 * @param desc  - a pointer to the seg_desc_t structure
 * @param base  - the segment base address
 * @param limit - the segment limit in bytes
 * @param type  - the descriptor type (see above macros)
 */
#define SET_SYS_DESC_PARAMS(desc, base, limit, type)    \
{                                                       \
    desc->fields.base_lo = base & 0x00FFFFFF;           \
    desc->fields.base_hi = (base & 0xFF000000) >> 24;   \
    desc->fields.limit_lo = limit & 0x0000FFFF;         \
    desc->fields.limit_hi = (limit & 0x000F0000) >> 16; \
    desc->fields.seg_type = type;                       \
    desc->fields.desc_type = 0;                         \
    desc->fields.dpl = 0;                               \
    desc->fields.present = 1;                           \
    desc->fields.avail = 0;                             \
    desc->fields.reserved = 0;                          \
    desc->fields.op_size = 1;                           \
    desc->fields.granularity = 0;                       \
}

/**
 * Returns the index of a descriptor in the GDT.
 *
 * @param selector - the segment selector
 * @return the index in the GDT of the descriptor that the segment selector
 *         points to
 */
static inline int get_gdt_index(int selector)
{
    /* Mask off the DPL and table indicator, then divide by the size of
       a descriptor */
    return (selector & 0xF8) / sizeof(seg_desc_t);
}

#endif /* __ASM__ */

#endif /* __LYRA_DESC_H */
