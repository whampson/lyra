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

/*******************************************************************************
 * Implementation Checklist:
 * Function             Status              Notes
 * --------             ------              -----
 * isalnum              NOT_IMPLEMENTED
 * isalpha              NOT_IMPLEMENTED
 * isblank              NOT_IMPLEMENTED     Part of C11, may not implement.
 * iscntrl              NOT_IMPLEMENTED
 * isdigit              NOT_IMPLEMENTED
 * isgraph              NOT_IMPLEMENTED
 * islower              NOT_IMPLEMENTED
 * isprint              NOT_IMPLEMENTED
 * ispunct              NOT_IMPLEMENTED
 * isspace              NOT_IMPLEMENTED
 * isupper              NOT_IMPLEMENTED
 * isxdigit             NOT_IMPLEMENTED
 * tolower              NOT_IMPLEMENTED
 * toupper              NOT_IMPLEMENTED
 *
 ******************************************************************************/

#ifndef __CTYPE_H__
#define __CTYPE_H__

int iscntrl(int ch);
int isprint(int ch);
int isspace(int ch);
int isblank(int ch);
int isgraph(int ch);
int ispunct(int ch);
int isalnum(int ch);
int isalpha(int ch);
int islower(int ch);
int isupper(int ch);
int isdigit(int ch);
int isxdigit(int ch);
int tolower(int ch);
int toupper(int ch);

#endif /* __CTYPE_H__ */
