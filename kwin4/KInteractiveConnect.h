/***************************************************************************
                          KInteractiveConnect.h  -  description
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
#ifndef _KINTERACTIVECONNECT_H_
#define _KINTERACTIVECONNECT_H_

#include <qobject.h>
#include "KEMessage.h"
#include "KChildConnect.h"

class KInteractiveConnect:public KChildConnect
{
  Q_OBJECT

  public:
  KInteractiveConnect();
  ~KInteractiveConnect();
  bool Next();
  bool Init(int id=0,KEMessage *msg=0);
  bool Exit();
  bool SendMsg(KEMessage *msg);
  bool Send(QString str);

  signals:
  void signalReceiveMsg(KEMessage *msg,int id); 
  void signalPrepareMove(KEMessage *msg,KG_INPUTTYPE type); 
};

#endif
