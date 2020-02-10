/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#ifndef __INIMODE_H
#define __INIMODE_H

#include <windows.h>
#include <imagehlp.h>

void Inimode_Init(char* ProgramName);
void Inimode_Done();

typedef struct RepControl
{
    char  TgtModule[32];
    char  TgtFunc1[32];
    char  TgtFunc2[32];
    char  TgtFunc3[32];
    DWORD newfunc;
    DWORD oldfunc;
} RepControl;

typedef struct KeyItem
{
    char  name[MAXPATH];
    char  value[MAXPATH];
} KeyItem;

typedef struct KeyData
{
    HKEY     hKey;
    char     keyname[MAXPATH];
    int      itemnum;
    KeyItem  *item;
} KeyData;

typedef struct RkeyWrapper
{
    int      max;
    int      num;
    KeyData  *data;
} RkeyWrapper;

#endif
