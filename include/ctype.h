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
 *   File: include/ctype.h
 * Author: Wes Hampson
 *   Desc: Part of wklibc - Wes's Kernel C Standard Library.
 *----------------------------------------------------------------------------*/

#ifndef __CTYPE_H
#define __CTYPE_H

/**
 * Checks if a character is a control character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a control character,
 *         zero otherwise
 */
static inline int iscntrl(int ch)
{
    return (ch >= 0x00 && ch <= 0x1F) || ch == 0x7F;
}

/**
 * Checks if a character is a printing character.
 *
 * @param ch - character to classify
 * @return non-zero if the character can be printed,
 *         zero otherwise
 */
static inline int isprint(int ch)
{
    return ch >= ' ' && ch <= '~';
}

/**
 * Checks if a character is a space character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a whitespace character,
 *         zero otherwise
 */
static inline int isspace(int ch)
{
    return ch == '\t'
        || ch == '\n'
        || ch == '\v'
        || ch == '\f'
        || ch == '\r'
        || ch == ' ';
}

/**
 * Checks if a character is a blank character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a blank character,
 *         zero otherwise
 */
static inline int isblank(int ch)
{
    return ch == '\t' || ch == ' ';
}

/**
 * Checks if a character is a graphical character.
 *
 * @param ch - character to classify
 * @return non-zero if the character has a graphical representation,
 *         zero otherwise
 */
static inline int isgraph(int ch)
{
    return ch >= ' ' && ch <= '~';
}

/**
 * Checks if a character is a punctuation character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a punctuation character,
 *         zero otherwise
 */
static inline int ispunct(int ch)
{
    return (ch >= '!' && ch <= '/')
        || (ch >= ':' && ch <= '@')
        || (ch >= '[' && ch <= '`')
        || (ch >= '{' && ch <= '~');
}

/**
 * Checks if a character is a numeric character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a numeric character,
 *         zero otherwise
 */
static inline int isdigit(int ch)
{
    return ch >= '0' && ch <= '9';
}

/**
 * Checks if a character is a hexadecimal numeric character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a hexadecimal numeric character,
 *         zero otherwise
 */
static inline int isxdigit(int ch)
{
    return isdigit(ch)
        || (ch >= 'A' && ch <= 'F')
        || (ch >= 'a' && ch <= 'f');
}

/**
 * Checks if a character is lowercase.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a lowercase letter,
 *         zero otherwise
 */
static inline int islower(int ch)
{
    return ch >= 'a' && ch <= 'z';
}

/**
 * Checks if a character is uppercase.
 *
 * @param ch - character to classify
 * @return non-zero if the character is an uppercase letter,
 *         zero otherwise
 */
static inline int isupper(int ch)
{
    return ch >= 'A' && ch <= 'Z';
}

/**
 * Checks if a character is alphabetic.
 *
 * @param ch - character to classify
 * @return non-zero if the character is an alphabetic character,
 *         zero otherwise
 */
static inline int isalpha(int ch)
{
    return isupper(ch) || islower(ch);
}

/**
 * Checks if a character is alphanumeric.
 *
 * @param ch - character to classify
 * @return non-zero if the character is an alphanumeric character,
 *         zero otherwise
 */
static inline int isalnum(int ch)
{
    return isalpha(ch) || isdigit(ch);
}

/**
 * Converts a character to lowercase.
 *
 * @param ch - character to be converted
 * @return lowercase version of 'ch',
 *         unmodified 'ch' if no lowercase version exists
 */
static inline int tolower(int ch)
{
    if (isupper(ch)) {
        ch += 0x20;
    }

    return ch;
}

/**
 * Converts a character to uppercase.
 *
 * @param ch - character to be converted
 * @return uppervase version of 'ch',
 *         unmodified 'ch' if no uppercase version exists
 */
static inline int toupper(int ch)
{
    if (islower(ch)) {
        ch -= 0x20;
    }

    return ch;
}

#endif /* __CTYPE_H */
