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
 * File: ps2kbd.c
 * Desc: PS/2 keyboard driver.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <lyra/kernel.h>
#include <lyra/input.h>
#include <lyra/io.h>
#include <drivers/ps2kbd.h>

/* PS/2 keyboard and controller I/O ports. */
#define PORT_KBD            0x60    /* PS/2 keyboard I/O */
#define PORT_CTL            0x64    /* PS/2 controller I/O */

/* Controller command words.
   The command register is accessed by writing to PORT_CTL. */
#define CTL_CMD_RDCFG       0x20
#define CTL_CMD_WRCFG       0x60
#define CTL_CMD_SELFTEST    0xAA
#define CTL_CMD_P1OFF       0xAD
#define CTL_CMD_P1ON        0xAE
#define CTL_CMD_P1TEST      0xAB
#define CTL_CMD_P2OFF       0xA7
#define CTL_CMD_P2ON        0xA8
#define CTL_CMD_P2TEST      0xA9

/* PS/2 controller status register bitfields.
   The status register is accessed by reading from PORT_CTL. */
#define CTL_STS_OUTFULL     0x01    /* 0 = wait, 1 = ready for read */
#define CTL_STS_INFULL      0x02    /* 0 = ready for write, 1 = wait */
#define CTL_STS_TIMEOUTERR  0x40
#define CTL_STS_PARITYERR   0x80

/* Controller configuration bitfields.
   Use the RDCFG and WRCFG commands to read/modify these fields. */
#define CTL_CFG_P1INT       0x01    /* port 1 interrupts on */
#define CTL_CFG_P1CLK       0x10    /* port 1 clock enabled */
#define CTL_CFG_P1XLATE     0x40    /* port 1 translation enabled */
#define CTL_CFG_P2INT       0x02    /* port 2 interrupts on */
#define CTL_CFG_P2CLK       0x20    /* port 2 clock enabled */
#define CTL_CFG_P2XLATE     0x20    /* port 2 translation enabled */
#define CTL_CFG_POST        0x04    /* POST passed */

/* Controller response bytes */
#define CTL_RES_TESTPASS    0x55
#define CTL_RES_TESTFAIL    0xFC

/* Keyboard command words.
   Commands are issued by writing to PORT_KBD. */
#define KBD_CMD_SETLED      0xED
#define KBD_CMD_ECHO        0xEE
#define KBD_CMD_SCANCODE    0xF0
#define KBD_CMD_IDENTIFY    0xF2
#define KBD_CMD_TYPEMATIC   0xF3
#define KBD_CMD_SCANON      0xF4
#define KBD_CMD_SCANOFF     0xF5
#define KBD_CMD_SETDEFAULTS 0xF6
#define KBD_CMD_RESEND      0xFE
#define KBD_CMD_SELFTEST    0xFF

/* The following commands are for scancode 3 only. */
#define KBD_CMD_TYPMONLY    0xF7
#define KBD_CMD_MKONLY      0xF9
#define KBD_CMD_MKBRK       0xF8
#define KBD_CMD_MKBRKTYPM   0xFA

/* Keyboard response words.
   Responses are accessed by reading from PORT_KBD. */
#define KBD_RES_ERROR1      0x00
#define KBD_RES_ERROR2      0xFF
#define KBD_RES_TESTPASS    0xAA
#define KBD_RES_TESTFAIL1   0xFC
#define KBD_RES_TESTFAIL2   0xFD
#define KBD_RES_ECHO        0xEE
#define KBD_RES_ACK         0xFA
#define KBD_RES_RESEND      0xFE

/* KBD_CMD_SCANCODE sub-commands */
#define KBD_CFG_GETSC       0x00
#define KBD_CFG_SC1         0x01
#define KBD_CFG_SC2         0x02
#define KBD_CFG_SC3         0x03

/* KBD_CMD_SETLED bitmasks */
#define KBD_CFG_LEDSCRL     0x01
#define KBD_CFG_LEDNUM      0x02
#define KBD_CFG_LEDCAPS     0x04

/* Response codes for kbd_sendcmd() */
#define SENDCMD_SUCCESS     0
#define SENDCMD_ERROR       1
#define SENDCMD_TIMEOUT     2

/* Data sent by keyboard when a key is released. */
#define SC3_BREAK           0xF0

/* kbd_sendcmd() retry count before SENDCMD_TIMEOUT is returned. */
#define NUM_RETRIES         3

/**
 * Mapping of physical scancodes to virtual scancodes for "scancode set 3", as
 * it's known.
 */
static const scancode_t SCANCODE3[256] =
{
/*00-07*/  0,0,0,0,0,0,0,KB_F1,
/*08-0F*/  KB_ESCAPE,0,0,0,0,KB_TAB,'`',KB_F2,
/*10-17*/  0,KB_LCTRL,KB_LSHIFT,0,KB_CAPLK,'q','1',KB_F3,
/*18-1F*/  0,KB_LALT,'z','s','a','w','2',KB_F4,
/*20-27*/  0,'c','x','d','e','4','3',KB_F5,
/*28-2F*/  0,' ','v','f','t','r','5',KB_F6,
/*30-37*/  0,'n','b','h','g','y','6',KB_F7,
/*38-3F*/  0,KB_RALT,'m','j','u','7','8',KB_F8,
/*40-47*/  0,',','k','i','o','0','9',KB_F9,
/*48-4F*/  0,'.','/','l',';','p','-',KB_F10,
/*50-57*/  0,0,'\'','\\','[','=',KB_F11,KB_PRTSC,
/*58-5F*/  KB_RCTRL,KB_RSHIFT,KB_ENTER,']','\\',0,KB_F12,KB_SCRLK,
/*60-67*/  KB_DOWN,KB_LEFT,KB_PAUSE,KB_UP,KB_DELETE,KB_END,KB_BACKSPACE,KB_INSERT,
/*68-6F*/  0,KB_NUM1,KB_RIGHT,KB_NUM4,KB_NUM7,KB_PAGEDN,KB_HOME,KB_PAGEUP,
/*70-77*/  KB_NUM0,KB_DECIMAL,KB_NUM2,KB_NUM5,KB_NUM6,KB_NUM8,KB_NUMLK,KB_DIVIDE,
/*78-7F*/  0,KB_RENTER,KB_NUM3,0,KB_ADD,KB_NUM9,KB_MULTIPLY,0,
/*80-8F*/  0,0,0,0,KB_SUBTRACT,0,0,0,
/*00-0F*/  0,0,0,0,0,0,0,0,
/*90-9F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*A0-AF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*B0-BF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*C0-CF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*D0-DF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*E0-EF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*F0-FF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static void ctl_outb(uint8_t data);
static void kbd_outb(uint8_t data);
static uint8_t kbd_inb(void);
static int kbd_sendcmd(uint8_t cmd);
static void kbd_flush(void);
static void ctl_test(void);
static void kbd_test(void);
static void kbd_cli(void);
static void kbd_sti(void);
static void kbd_sc3init(void);
static void kbd_setled(int num, int caps, int scrl);

void ps2kbd_init(void)
{
    uint8_t data;

    puts("Initializing PS/2 keyboard...\n");

    /* disable ports */
    ctl_outb(CTL_CMD_P1OFF);
    ctl_outb(CTL_CMD_P2OFF);
    kbd_flush();

    /* disable interrupts and translation */
    ctl_outb(CTL_CMD_RDCFG);
    data = kbd_inb();
    data &= ~CTL_CFG_P1INT;
    data &= ~CTL_CFG_P2INT;
    data &= ~CTL_CFG_P1XLATE;
    data &= ~CTL_CFG_P2XLATE;
    ctl_outb(CTL_CMD_WRCFG);
    kbd_outb(data);

    /* perform controller tests */
    ctl_test();

    /* init keyboard */
    ctl_outb(CTL_CMD_P1ON);
    kbd_test();
    kbd_sc3init();
    kbd_setled(0, 0, 0);        /* enables keyboard interrupts */
}

void ps2kbd_do_irq(void)
{
    /* Keyboard state flags */
    static int evt_keyrelease = 0;
    static int flag_ctrl = 0;
    static int flag_shift = 0;
    static int flag_alt = 0;
    static int flag_num = 0;
    static int flag_caps = 0;
    static int flag_scroll = 0;

    uint8_t kb_data;
    scancode_t sc;
    struct keystroke k = { 0 };

    /* Disable keyboard */
    ctl_outb(CTL_CMD_P1OFF);

    /* Read raw scancode from keyboard */
    kb_data = inb(PORT_KBD);
    if (kb_data == KBD_RES_ERROR1 || kb_data == KBD_RES_ERROR2) {
        /* TODO: handle error */
        puts("Keyboard error!\n");
        goto irq_cleanup;
    }
    else if (kb_data == SC3_BREAK) {
        /* Key was released.
           Next interrupt will have scancode of released key. */
        evt_keyrelease = 1;
        goto irq_cleanup;
    }

    /* Convert to virtual scancode */
    sc = SCANCODE3[kb_data];
    if (sc == 0) {
        /* TODO: not mapped. log? */
        goto irq_cleanup;
    }

    /* Handle modifier and toggle keys */
    switch (sc) {
        case KB_LCTRL:
        case KB_RCTRL:
            flag_ctrl = (!evt_keyrelease) ? 1 : 0;
            break;
        case KB_LSHIFT:
        case KB_RSHIFT:
            flag_shift = (!evt_keyrelease) ? 1 : 0;
            break;
        case KB_LALT:
        case KB_RALT:
            flag_alt = (!evt_keyrelease) ? 1 : 0;
            break;
        case KB_NUMLK:
            if (!evt_keyrelease) {
                flag_num ^= 1;
                kbd_setled(flag_num, flag_caps, flag_scroll);
            }
            break;
        case KB_CAPLK:
            if (!evt_keyrelease) {
                flag_caps ^= 1;
                kbd_setled(flag_num, flag_caps, flag_scroll);
            }
            break;
        case KB_SCRLK:
            if (!evt_keyrelease) {
                flag_scroll ^= 1;
                kbd_setled(flag_num, flag_caps, flag_scroll);
            }
            break;
    }

    /* Build and transmit keystroke */
    k.key_id = sc;
    k.flag_keypress = !evt_keyrelease;
    k.flag_ctrl = flag_ctrl;
    k.flag_shift = flag_shift;
    k.flag_alt = flag_alt;
    k.flag_numlk = flag_num;
    k.flag_capslk = flag_caps;
    k.flag_scrlk = flag_scroll;
    sendkey(encode_keystroke(k));

    evt_keyrelease = 0;

irq_cleanup:
    /* Re-enable keyboard. */
    ctl_outb(CTL_CMD_P1ON);
}

/**
 * Output a byte to the PS/2 controller.
 * @param data - the byte to output
 */
static void ctl_outb(uint8_t data)
{
    /* Poll status register,
       wait until input buffer is empty before writing */
    while (inb(PORT_CTL) & CTL_STS_INFULL);
    outb(data, PORT_CTL);
}

/**
 * Output a byte to the PS/2 keyboard.
 * @param data - the byte to output
 */
static void kbd_outb(uint8_t data)
{
    /* Poll status register,
       wait until input buffer is empty before writing */
    while (inb(PORT_CTL) & CTL_STS_INFULL);
    outb(data, PORT_KBD);
}

/**
 * Read a byte from the PS/2 keyboard.
 * @return a byte from the keyboard
 */
static uint8_t kbd_inb(void)
{
    /* Poll status register,
       wait until output buffer is full before reading */
    while (!(inb(PORT_CTL) & CTL_STS_OUTFULL));
    return inb(PORT_KBD);
}

/**
 * Send a command to the PS/2 keyboard.
 * @param cmd - the keyboard command word (one of KBD_CMD_*)
 * @return SENDCMD_SUCCESS when a command is successfully issued
 *         SENDCMD_ERROR   if the keyboard responds with an error
 *         SENDCMD_TIMEOUT if the keyboard asks to reissue the command too many times
 */
static int kbd_sendcmd(uint8_t cmd)
{
    int i;
    int retval;
    uint8_t data;

    i = 0;
    retval = SENDCMD_ERROR;
    do {
        kbd_outb(cmd);
        data = kbd_inb();

        switch (data) {
            case KBD_RES_ACK:
                retval = SENDCMD_SUCCESS;
                goto sendcmd_done;
            case KBD_RES_RESEND:
                break;
            case KBD_RES_ERROR1:
            case KBD_RES_ERROR2:
                retval = SENDCMD_ERROR;
                goto sendcmd_done;
            default:
                puts("Unrecognized response from PS/2 keyboard: ");
                putix(data);
                puts("\n");
                retval = SENDCMD_ERROR;
                goto sendcmd_done;
        }
        i++;
    } while (i < NUM_RETRIES);

    if (i == NUM_RETRIES) {
        retval = SENDCMD_TIMEOUT;
    }

sendcmd_done:
    return retval;
}

/**
 * Flush the keyboard output buffer.
 */
static void kbd_flush(void)
{
    /* TODO: timeout */
    while (inb(PORT_CTL) & CTL_STS_OUTFULL) {
        (void)kbd_inb();
    }
}

/**
 * Perform PS/2 controller self-test and test keyboard port.
 */
static void ctl_test(void)
{
    uint8_t data;

    /* controller self-test */
    ctl_outb(CTL_CMD_SELFTEST);
    data = kbd_inb();
    if (data != CTL_RES_TESTPASS) {
        puts("PS/2 controller self-test failed!\n");
    }

    /* port tests */
    ctl_outb(CTL_CMD_P1TEST);
    data = kbd_inb();
    if (data != 0x00) {
        puts("PS/2 controller port 1 test failed!\n");
    }
}

/**
 * Perform keyboard self-test.
 */
static void kbd_test(void)
{
    uint8_t data;

    /* keyboard self-test */
    if (kbd_sendcmd(KBD_CMD_SELFTEST) != SENDCMD_SUCCESS) {
        puts("Failed to start PS/2 keyboard self-test!\n");
    }

    data = kbd_inb();
    if (data != KBD_RES_TESTPASS) {
        puts("PS/2 keyboard self-test failed! (");
        putix(data);
        puts(")\n");
    }
}

/**
 * Disable keyboard interrupts.
 */
static void kbd_cli(void)
{
    uint8_t data;

    ctl_outb(CTL_CMD_RDCFG);
    data = kbd_inb();
    data &= ~CTL_CFG_P1INT;
    ctl_outb(CTL_CMD_WRCFG);
    kbd_outb(data);
}

/**
 * Enable keyboard interrupts.
 */
static void kbd_sti(void)
{
    uint8_t data;

    ctl_outb(CTL_CMD_RDCFG);
    data = kbd_inb();
    data |= CTL_CFG_P1INT;
    ctl_outb(CTL_CMD_WRCFG);
    kbd_outb(data);
}

/**
 * Tell keyboard to send scancodes using scancode set 3.
 */
static void kbd_sc3init(void)
{
    /* set scancode */
    if (kbd_sendcmd(KBD_CMD_SCANCODE) != SENDCMD_SUCCESS) {
        goto sc3_fail;
    }
    if (kbd_sendcmd(KBD_CFG_SC3) != SENDCMD_SUCCESS) {
        goto sc3_fail;
    }

    /* read back scancode for sanity */
    if (kbd_sendcmd(KBD_CMD_SCANCODE) != SENDCMD_SUCCESS) {
        goto sc3_fail;
    }
    if (kbd_sendcmd(KBD_CFG_GETSC) != SENDCMD_SUCCESS) {
        goto sc3_fail;
    }
    if (kbd_inb() != KBD_CFG_SC3) {
        goto sc3_fail;
    }

    /* set make/break/repeat and enable scanning
       some keyboards (QEMU) don't support the make/break/repeat command... */
    kbd_sendcmd(KBD_CMD_MKBRKTYPM);
    kbd_sendcmd(KBD_CMD_SCANON);
    return;

sc3_fail:
    puts("PS/2 keyboard: failed to switch to scancode 3!\n");
}

/**
 * Set the states of the NUMLOCK, CAPSLOCK, and SCRLOCK lights.
 * @param num  - numlock state
 * @param caps - capslock state
 * @param scrl - scrlock state
 */
static void kbd_setled(int num, int caps, int scrl)
{
    /* TODO: debug this, QEMU keyboard responds to SETLED with 0xF0
       (Mac only) */

    uint8_t data;

    kbd_cli();
    data = 0;
    data |= (num)  ? KBD_CFG_LEDNUM  : 0;
    data |= (caps) ? KBD_CFG_LEDCAPS : 0;
    data |= (scrl) ? KBD_CFG_LEDSCRL : 0;

    if (kbd_sendcmd(KBD_CMD_SETLED) != SENDCMD_SUCCESS) {
        goto setled_fail;
    }
    if (kbd_sendcmd(data) != SENDCMD_SUCCESS) {
        goto setled_fail;
    }
    goto setled_done;

setled_fail:
    puts("Failed to set PS/2 keyboard LEDs!\n");
setled_done:
    kbd_sti();
}
