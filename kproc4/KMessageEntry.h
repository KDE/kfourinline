/***************************************************************************
                          KMessageEntry.h  -  description
                             -------------------
    begin                : Tue May 2 2000
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
#ifndef _KMESSAGEENTRY_H_
#define _KMESSAGEENTRY_H_

#include <string.h>
#include "KConnectTypes.h"



class KMessageEntry
{
  private:
    int size;
    KGM_TYPE type;
    char *data;

  public:
    void SetType(KGM_TYPE t);
    KGM_TYPE QueryType();
    int QuerySize();
    char *QueryData();
    bool CopyData(int s,char *c);
    KMessageEntry();
    KMessageEntry(KMessageEntry &entry);
    KMessageEntry &operator=(KMessageEntry &entry);
    ~KMessageEntry();
};

#endif
