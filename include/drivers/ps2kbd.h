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
 *   File: include/drivers/ps2kbd.h
 * Author: Wes Hampson
 *   Desc: PS/2 keyboard driver interface.
 *----------------------------------------------------------------------------*/

#ifndef __DRIVERS_INPUT_PS2KBD_H
#define __DRIVERS_INPUT_PS2KBD_H

/**
 * Test PS/2 controller and keyboard, then configure keyboard to transmit
 * scancodes from scancode set 3.
 */
void ps2kbd_init(void);

/**
 * IRQ handler for keyboard interrupts.
 * Sends virtual keystrokes to the terminal.
 */
void ps2kbd_do_irq(void);

#endif /* __DRIVERS_INPUT_PS2KBD_H */
