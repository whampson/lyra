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
 * File: exception.c
 * Desc: Exception handling routines.
 *----------------------------------------------------------------------------*/

#include <lyra/kernel.h>
#include <lyra/exception.h>

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

/* Error code fields. */
struct err_code {
    uint32_t external   : 1;    /* exception generated external to CPU */
    uint32_t idt        : 1;    /* selector references IDT */
    uint32_t ti         : 1;    /* selector references { 0 = GDT, 1 = LDT } */
    uint32_t selector   : 13;   /* segment selector index */
};

__attribute__((fastcall))
static void handle_unknown_exception(void);

__attribute__((fastcall))
static void exception_halt(void);

__attribute__((fastcall))
void do_exception(struct interrupt_frame *regs)
{
    const char *name;
    int num;
    int has_err_code;

    num = regs->vec_num;
    if (num >= NUM_EXCEPT) {
        handle_unknown_exception();
    }

    name = EXCEPTION_NAMES[num];
    if (name == NULL) {
        handle_unknown_exception();
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

    /* TODO: Register dump, show error code, etc. (BSOD?) */
    /* For now, just print the exception name and die */
    puts(name);
    exception_halt();
}

__attribute__((fastcall))
static void handle_unknown_exception(void)
{
    /* TODO: print num */
    puts("Unknown exception!\n");
    exception_halt();
}

__attribute__((fastcall))
void exception_halt(void)
{
    /* Deathbed... */
    __asm__ volatile ("rip: hlt; jmp rip" : : : "memory");
}
