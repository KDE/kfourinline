/***************************************************************************
                          KChildConnect.h  -  description
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
#ifndef _KCHILDCONNECT_H_
#define _KCHILDCONNECT_H_

#include <qobject.h>
#include <qstrlist.h>
#include "KEMessage.h"


class KChildConnect: public QObject
{
  Q_OBJECT

  protected:
  QStrList inputcache;
  bool input_pending;
  QString inputbuffer;
  int ID;

  public:
  KChildConnect();
  ~KChildConnect();
  void Receive(QString input);
  int QueryID();
  void SetID(int id);

  virtual bool SendMsg(KEMessage *msg);
  virtual bool Send(QString str);
  virtual KR_STATUS QueryStatus();

  public slots:


  signals:
  void signalReceiveMsg(KEMessage *msg,int id); 
};

#endif
