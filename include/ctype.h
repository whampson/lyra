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

#ifndef __CTYPE_H__
#define __CTYPE_H__

/**
 * Checks if a character is a control character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a control character,
 *         zero otherwise
 */
int iscntrl(int ch);

/**
 * Checks if a character is a printing character.
 *
 * @param ch - character to classify
 * @return non-zero if the character can be printed,
 *         zero otherwise
 */
int isprint(int ch);

/**
 * Checks if a character is a space character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a whitespace character,
 *         zero otherwise
 */
int isspace(int ch);

/**
 * Checks if a character is a blank character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a blank character,
 *         zero otherwise
 */
int isblank(int ch);

/**
 * Checks if a character is a graphical character.
 *
 * @param ch - character to classify
 * @return non-zero if the character has a graphical representation,
 *         zero otherwise
 */
int isgraph(int ch);

/**
 * Checks if a character is a punctuation character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a punctuation character,
 *         zero otherwise
 */
int ispunct(int ch);

/**
 * Checks if a character is alphanumeric.
 *
 * @param ch - character to classify
 * @return non-zero if the character is an alphanumeric character,
 *         zero otherwise
 */
int isalnum(int ch);

/**
 * Checks if a character is alphabetic.
 *
 * @param ch - character to classify
 * @return non-zero if the character is an alphabetic character,
 *         zero otherwise
 */
int isalpha(int ch);

/**
 * Checks if a character is lowercase.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a lowercase letter,
 *         zero otherwise
 */
int islower(int ch);

/**
 * Checks if a character is uppercase.
 *
 * @param ch - character to classify
 * @return non-zero if the character is an uppercase letter,
 *         zero otherwise
 */
int isupper(int ch);

/**
 * Checks if a character is a numeric character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a numeric character,
 *         zero otherwise
 */
int isdigit(int ch);

/**
 * Checks if a character is a hexadecimal numeric character.
 *
 * @param ch - character to classify
 * @return non-zero if the character is a hexadecimal numeric character,
 *         zero otherwise
 */
int isxdigit(int ch);

/**
 * Converts a character to lowercase.
 *
 * @param ch - character to be converted
 * @return lowercase version of 'ch',
 *         unmodified 'ch' if no lowercase version exists
 */
int tolower(int ch);

/**
 * Converts a character to uppercase.
 *
 * @param ch - character to be converted
 * @return uppervase version of 'ch',
 *         unmodified 'ch' if no uppercase version exists
 */
int toupper(int ch);

#endif /* __CTYPE_H__ */
