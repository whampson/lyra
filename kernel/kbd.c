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
 * File: kbd.c
 * Desc: PS/2 keyboard driver.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <lyra/kbd.h>
#include <lyra/io.h>

/* PS/2 keyboard and controller I/O ports. */
#define PORT_KBD        0x60
#define PORT_CTL        0x64

/* Keyboard command bytes. */
#define CMD_KBD_LED     0xED    /* set LEDs */
#define CMD_KBD_ECHO    0xEE    /* echo (sends same byte back on success) */
#define CMD_KBD_SC      0xF0    /* get/set scancode set */
#define CMD_KBD_TYPM    0xF3    /* set typematic rate and delay */

/* Controller command bytes. */
#define CMD_CTL_RDCFG   0x20    /* read controller configuration port */
#define CMD_CTL_WRCFG   0x60    /* write controller configuration port */
#define CMD_CTL_KBDOFF  0xAD    /* disable keyboard */
#define CMD_CTL_KBDON   0xAE    /* enable keyboard */

/* Keyboard response bytes. */
#define KBD_ECHO        0xEE    /* echo byte (see above) */
#define KBD_ACK         0xFA    /* command acknowledged */
#define KBD_RSND        0xFE    /* resend last command */

void kbd_init(void)
{
    uint8_t data;

    kbd_disable();
    kbd_set_scancode(1);

    do {
        outb_p(CMD_KBD_SC, PORT_KBD);
    } while ((data = inb(PORT_KBD)) == KBD_RSND);

    do {
        outb_p(0, PORT_KBD);
    } while ((data = inb(PORT_KBD)) == KBD_RSND);

    /* Scancode set (hopefully) */
    data = inb_p(PORT_KBD);

    switch (data) {
        case 0x43:
            puts("Scancode 1!\n");
            break;
        case 0x41:
            puts("Scancode 2!\n");
            break;
        case 0x3f:
            puts("Scancode 3!\n");
            break;
        default:
            puts("Unknown scancode?\n");
            break;
    }

    kbd_enable();
}

void kbd_enable(void)
{
    outb_p(CMD_CTL_KBDON, PORT_CTL);
}

void kbd_disable(void)
{
    outb_p(CMD_CTL_KBDOFF, PORT_CTL);
}

void kbd_set_scancode(int sc)
{
    uint8_t data;

    do {
        outb_p(CMD_KBD_SC, PORT_KBD);
    } while ((data = inb(PORT_KBD)) == KBD_RSND);

    do {
        outb_p(sc, PORT_KBD);
    } while ((data = inb(PORT_KBD)) == KBD_RSND);
}

void kbd_handle_interrupt(void)
{
    uint8_t data;
    data = inb(PORT_KBD);

    putchar((char) data);
}
