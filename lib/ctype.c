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
 *   File: lib/ctype.c
 * Author: Wes Hampson
 *   Desc: Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#include <ctype.h>

int iscntrl(int ch)
{
    return (ch >= 0x00 && ch <= 0x1F) || ch == 0x7F;
}

int isprint(int ch)
{
    return ch >= ' ' && ch <= '~';
}

int isspace(int ch)
{
    return ch == '\t'
        || ch == '\n'
        || ch == '\v'
        || ch == '\f'
        || ch == '\r'
        || ch == ' ';
}

int isblank(int ch)
{
    return ch == '\t' || ch == ' ';
}

int isgraph(int ch)
{
    return ch >= ' ' && ch <= '~';
}

int ispunct(int ch)
{
    return (ch >= '!' && ch <= '/')
        || (ch >= ':' && ch <= '@')
        || (ch >= '[' && ch <= '`')
        || (ch >= '{' && ch <= '~');
}

int isalnum(int ch)
{
    return isalpha(ch) || isdigit(ch);
}

int isalpha(int ch)
{
    return isupper(ch) || islower(ch);
}

int islower(int ch)
{
    return ch >= 'a' && ch <= 'z';
}

int isupper(int ch)
{
    return ch >= 'A' && ch <= 'A';
}

int isdigit(int ch)
{
    return ch >= '0' && ch <= '9';
}

int isxdigit(int ch)
{
    return isdigit(ch)
        || (ch >= 'A' && ch <= 'F')
        || (ch >= 'a' && ch <= 'f');
}

int tolower(int ch)
{
    if (isupper(ch)) {
        ch += 0x20;
    }

    return ch;
}

int toupper(int ch)
{
    if (islower(ch)) {
        ch -= 0x20;
    }

    return ch;
}
