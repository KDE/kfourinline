/***************************************************************************
                          KConnectTypes.h  -  description
                             -------------------
    begin                : Sun Apr 9 2000
    copyright            : (C) 2000 by Martin Heni
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
#ifndef _KCONNECTTYPES_H_
#define _KCONNECTTYPES_H_

enum KGM_TYPE {KGM_TYPE_INVALID=0,KGM_TYPE_SHORT=1,KGM_TYPE_LONG=2,
              KGM_TYPE_FLOAT=3,KGM_TYPE_DATA=4};

enum KG_INPUTTYPE {
              KG_INPUTTYPE_INVALID=0,
              KG_INPUTTYPE_INTERACTIVE=1,
              KG_INPUTTYPE_PROCESS=2,
              KG_INPUTTYPE_REMOTE=3};

enum KR_STATUS {
              KR_NO_SOCKET=-2,
              KR_WAIT_FOR_CLIENT=-1,
              KR_INVALID=0,
              // >0 OK
              KR_OK=1,
              KR_CLIENT=2,
              KR_SERVER=3
              };
#endif
