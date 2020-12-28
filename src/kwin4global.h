/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWIN4GLOBAL_H
#define KWIN4GLOBAL_H

/** Global debug flag */
extern int global_debug;
/** Global skip intro flag */
extern bool global_skip_intro;
/** Demo or autoplay mode */
extern bool global_demo_mode;

// The user or color */
typedef enum {Nobody=-1,Yellow=0,Red=1,Tip=3} COLOUR;
/** Type of statistics */
typedef enum  {TSum,TWin,TRemis,TLost,TBrk} TABLE;
/** Movement status */
typedef enum  {GIllMove=-2,GNotAllowed=-1,GNormal=0,GYellowWin=1,GRedWin=2,GRemis=3,GTip=4} MOVESTATUS;

#endif // KWIN4GLOBAL_H

