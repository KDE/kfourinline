/***************************************************************************
                          KConnectEntry.h  -  description
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
#ifndef _KCONNECTENTRY_H_
#define _KCONNECTENTRY_H_

#include <string.h>
#include "KRemoteConnect.h"
#include "KProcessConnect.h"
#include "KInteractiveConnect.h"
#include "KConnectTypes.h"
#include "KEMessage.h"

union UConnect
{
  KRemoteConnect *r;
  KProcessConnect *p;
  KInteractiveConnect *i;
};

class KConnectEntry
{
  private:
  KG_INPUTTYPE type; // remote,computer,interactive
  UConnect connect;
  protected:
  int ID;

  public:
  KG_INPUTTYPE QueryType();
  KRemoteConnect *QueryRemoteConnect();
  KProcessConnect *QueryProcessConnect();
  KInteractiveConnect *QueryInteractiveConnect();
  KConnectEntry();
  ~KConnectEntry();
  KConnectEntry(KConnectEntry &entry);
  KConnectEntry &operator=(KConnectEntry &entry);
  bool Exit();
  bool Init(KG_INPUTTYPE stype,int id=0,KEMessage *msg=0);
  KR_STATUS QueryStatus();
  bool SendMsg(KEMessage *msg);
};
#endif
