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
 *   File: drivers/input/input.c
 * Author: Wes Hampson
 *----------------------------------------------------------------------------*/

#include <lyra/input.h>

/**
 * Mapping of "unshifted" ASCII characters to "shifted" ASCII characters.
 */
const char SHIFT_MAP[256] =
{
/*00-0F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*10-1F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*20-2F*/  0,0,0,0,0,0,0,'"',0,0,0,0,'<','_','>','?',
/*30-3F*/  ')','!','@','#','$','%','^','&','*','(',0,':',0,'+',0,0,
/*40-4F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*50-5F*/  0,0,0,0,0,0,0,0,0,0,0,'{','|','}',0,0,
/*60-6F*/  '~','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
/*70-7F*/  'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
/*80-8F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*90-9F*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*A0-AF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*B0-BF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*C0-CF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*D0-DF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*E0-EF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*F0-FF*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
