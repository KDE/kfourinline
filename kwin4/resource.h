/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

///////////////////////////////////////////////////////////////////
// resource.h  -- contains macros used for commands


///////////////////////////////////////////////////////////////////
// COMMAND VALUES FOR MENUBAR AND TOOLBAR ENTRIES


///////////////////////////////////////////////////////////////////
// File-menu entries
// #define ID_FILE_NEW_WINDOW          10010
#define ID_FILE_NEW                 10020
#define ID_FILE_REMOTE              10030
// #define ID_FILE_OPEN                10030
// #define ID_FILE_OPEN_RECENT         10040
#define ID_FILE_CLOSE               10050

// #define ID_FILE_SAVE                10060
// #define ID_FILE_SAVE_AS             10070

// #define ID_FILE_PRINT               10080

#define ID_FILE_QUIT                10090

#define ID_FILE_STATISTICS          10100
#define ID_FILE_STOPREMOTE          10110
#define ID_FILE_HINT                10120
#define ID_FILE_MESSAGE             10130

///////////////////////////////////////////////////////////////////
// Edit-menu entries
#define ID_EDIT_COPY                11010
#define ID_EDIT_CUT                 11020
#define ID_EDIT_PASTE               11030
#define ID_EDIT_UNDO                11040
#define ID_EDIT_REDO                11050

///////////////////////////////////////////////////////////////////
// View-menu entries                    
#define ID_VIEW_TOOLBAR             12010
#define ID_VIEW_STATUSBAR           12020

///////////////////////////////////////////////////////////////////
// Options-menu entries
#define ID_STARTCOLOUR_YELLOW       13110
#define ID_STARTCOLOUR_RED          13120

#define ID_YELLOW_PLAYER            13210
#define ID_YELLOW_COMPUTER          13220
#define ID_YELLOW_REMOTE            13230

#define ID_RED_PLAYER               13310
#define ID_RED_COMPUTER             13320
#define ID_RED_REMOTE               13330

#define ID_LEVEL_1                  13410
#define ID_LEVEL_2                  13411
#define ID_LEVEL_3                  13412
#define ID_LEVEL_4                  13413
#define ID_LEVEL_5                  13414
#define ID_LEVEL_6                  13415
#define ID_LEVEL_7                  13416
#define ID_LEVEL_8                  13417
#define ID_LEVEL_9                  13418
#define ID_LEVEL_10                 13419

#define ID_OPTIONS_NAMES            13510

#define ID_OPTIONS_NETWORK          13520
#define ID_OPTIONS_ANIMATIONS       13530


///////////////////////////////////////////////////////////////////
// Help-menu entries
#define ID_HELP_CONTENTS            1002

///////////////////////////////////////////////////////////////////
// General application values
#define ID_STATUS_MSG                1003
#define ID_STATUS_MOVER              1002
#define ID_STATUS_TIME               1001


#endif // RESOURCE_H
