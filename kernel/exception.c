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

#include <lyra/exception.h>
#include <lyra/kernel.h>

__attribute__((fastcall))
static void handle_unknown_exception(void);

__attribute__((fastcall))
static void exception_halt(void);

__attribute__((fastcall))
void do_exception(struct interrupt_frame *regs)
{
    if (regs->vec_num >= NUM_EXCEPT) {
        handle_unknown_exception();
    }

    // handler_stub_func stub;
    // stub = exception_stubs[regs->vec_num];
    // if (stub == NULL) {
    //     handle_unknown_exception();
    // }

    switch (regs->vec_num) {
        case EXCEPT_DE:
            puts("Divide error!\n");
            exception_halt();
            break;
        default:
            handle_unknown_exception();
            break;
    }
}

__attribute__((fastcall))
static void handle_unknown_exception(void)
{
    puts("Unknown exception!\n");
    exception_halt();
}

__attribute__((fastcall))
void exception_halt(void)
{
    /* Deathbed... */
    __asm__ volatile ("rip: hlt; jmp rip" : : : "memory");
}
