#ifndef KWIN4GLOBAL_H
#define KWIN4GLOBAL_H
/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

extern int global_debug;

// The user or color?
typedef enum e_Farbe {Niemand=-1,Gelb=0,Rot=1,Tip=3,Rand=4,GelbWin=8,RotWin=9} FARBE;
// The type of player
typedef enum  {Men=0,Computer=1,Remote=2} PLAYER;
typedef enum  {TSum,TWin,TRemis,TLost,TBrk} TABLE;
typedef enum  {GIllMove=-2,GNotAllowed=-1,GNormal=0,GYellowWin=1,GRedWin=2,GRemis=3,GTip=4} MOVESTATUS;

#define NOOFPLAYER 2

#define FIELD_SIZE_X 7
#define FIELD_SIZE_Y 6
#define FIELD_SPACING 40

#endif // KWIN4GLOBAL_H

