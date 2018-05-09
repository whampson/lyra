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
void do_except(struct interrupt_frame *regs)
{
    switch (regs->vec_num) {
        case EXCEPT_DE:
            puts("Divide error!\n");
            __asm__ volatile ("except_halt: hlt; jmp except_halt" : : : "memory");
            break;

        default:
            puts("Unknown exception!\n");
            break;
    }
}