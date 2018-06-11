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
 *   File: kernel/exception.c
 * Author: Wes Hampson
 *   Desc: Exception handling.
 *----------------------------------------------------------------------------*/

#include <stdbool.h>
#include <string.h>
#include <lyra/console.h>
#include <lyra/exception.h>
#include <drivers/vga.h>

/* Names of all non-Intel-reserved exceptions. */
static const char * const EXCEPTION_NAMES[NUM_EXCEPT] =
{
    "DIVIDE_ERROR",
    "DEBUG",
    "NON_MASKABLE_INTERRUPT",
    "BREAKPOINT",
    "OVERFLOW",
    "BOUND_RANGE_EXCEEDED",
    "INVALID_OPCODE",
    "DEVICE_NOT_AVAILABLE",
    "DOUBLE_FAULT",
    NULL,
    "INVALID_TSS",
    "SEGMENT_NOT_PRESENT",
    "STACK_SEGMENT_FAULT",
    "GENERAL_PROTECTION_FAULT",
    "PAGE_FAULT",
    NULL,
    "x87_FLOATING_POINT_ERROR",
    "ALIGNMENT_CHECK",
    "MACHINE_CHECK",
    "SIMD_FLOATING_POINT_ERROR",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static const char * const FLAG_IDS[32] =
{
    "CF",   NULL,   "PF",   NULL,
    "AF",   NULL,   "ZF",   "SF",
    "TF",   "IF",   "DF",   "OF",
    "IOPL0","IOPL1","NT",   NULL,
    "RF",   "VM",   "AC",   "VIF",
    "VIP",  "ID0",  "ID1",  "VAD0",
    "VAD1", "VAD2", "VAD3", "VAD4",
    "VAD6", "VAD7", "VAD8"  "VAD9"
};

/* Error code fields. */
struct err_code {
    uint32_t external   : 1;    /* exception generated external to CPU */
    uint32_t idt        : 1;    /* selector references IDT */
    uint32_t ti         : 1;    /* selector references { 0 = GDT, 1 = LDT } */
    uint32_t selector   : 13;   /* segment selector index */
};

static void handle_unknown_exception(int num);
static void exception_halt(void);
static void dump_regs(struct interrupt_frame *regs);
static void blue_screen(int num, bool has_error_code, struct interrupt_frame *regs);

__attribute__((fastcall))
void do_exception(struct interrupt_frame *regs)
{
    int num;
    int has_err_code;

    num = regs->vec_num;
    if (num >= NUM_EXCEPT) {
        handle_unknown_exception(num);
    }

    switch (num) {
        case EXCEPT_DF:
        case EXCEPT_TS:
        case EXCEPT_NP:
        case EXCEPT_SS:
        case EXCEPT_GP:
        case EXCEPT_PF:
        case EXCEPT_AC:
            has_err_code = 1;
            break;
        default:
            has_err_code = 0;
            break;
    }

    /* TODO: kill process unless exception happens in kernel */

    blue_screen(num, has_err_code, regs);
    exception_halt();
}

static void handle_unknown_exception(int num)
{
    kprintf("Unknown exception! (%x)\n", num);
    exception_halt();
}

static void exception_halt(void)
{
    /* Deathbed... */
    __asm__ volatile ("cli; .rip%=: hlt; jmp .rip%=" : : : "memory");
}

static void dump_regs(struct interrupt_frame *regs)
{
    char fl_str[64];
    const char * fl_id;
    int i;
    uint32_t flags;

    fl_str[0] = '\0';
    strncat(fl_str, "[ ", 2);
    flags = regs->eflags;

    for (i = 0; i < 32; i++) {
        if (flags & 0x01) {
            fl_id = FLAG_IDS[i];
            if (fl_id != NULL) {
                strcat(fl_str, fl_id);
                strcat(fl_str, " ");
            }
        }
        flags >>= 1;
    }
    strcat(fl_str, "]");

    kprintf("EAX = %08X, EBX = %08X, ECX = %08X, EDX = %08X\r\n",
        regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI = %08X, EDI = %08X, EBP = %08X, ESP = %08X\r\n",
        regs->esi, regs->edi, regs->ebp, regs->esp);
    kprintf("EIP = %08X, EFLAGS = %s",
        regs->eip, fl_str);
}

static void blue_screen(int num, bool has_error_code, struct interrupt_frame *regs)
{
    const char *s;
    size_t len;
    int row, col;

    kprintf("\033[0;44m\033[2J");

    /* TODO: sprintf would be really useful here... */

    s = OS_NAME;
    len = strlen(s) + 4;
    row = 4;
    col = (CON_COLS / 2) - (len / 2);
    kprintf("\033[%d;%dH\033[34;47m  %s  ", row, col, OS_NAME);

    row = 7;
    col = (CON_COLS / 2) - 23;
    kprintf("\033[%d;%dH\033[37;44m", row, col);
    kprintf("A fatal exception \033[1m%02X\033[21m has occurred at \033[1m%08X\033[21m.",
        num, regs->eip);

    s = "Your computer must be restarted.";
    len = strlen(s);
    row = 8;
    col = (CON_COLS / 2) - (len / 2);
    kprintf("\033[%d;%dH%s", row, col, s);

    s = EXCEPTION_NAMES[num];
    len = strlen(s);
    row = 11;
    col = (CON_COLS / 2) - (len / 2);
    kprintf("\033[%d;%dH\033[1m%s\033[21m", row, col, s);

    if (has_error_code) {
        row = 12;
        col = (CON_COLS / 2) - 2;
        kprintf("\033[%d;%dH%04X", row, col, regs->err_code);
    }

    row = CON_ROWS - 2;
    col = 1;
    kprintf("\033[%d;%dH", row, col);
    dump_regs(regs);

    hide_cursor();
}
