/***************************************************************************
                          KRemoteConnect.h  -  description
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
#ifndef _KREMOTECONNECT_H_
#define _KREMOTECONNECT_H_

#include <qobject.h>
//#include <ksock.h>
#include "KEMessage.h"
#include "KRSocket.h"
#include "ksock.h"
#include "KChildConnect.h"




class KRemoteConnect: public KChildConnect
{
  Q_OBJECT

  protected:
  KRServerSocket *kServerSocket;
  KSocket *kSocket;
  QString IP;
  ushort port;
  KR_STATUS socketStatus;
  char *buffer;
  KEMessage *bufferMsg;
//  QString inputbuffer;
//  bool input_pending;
//  QStrList inputcache;

  public:
  KRemoteConnect();
  ~KRemoteConnect();
  bool Init(int id=0,KEMessage *msg=0);
  bool Exit();
  bool Next();
//  bool SendMsg(KEMessage *msg);
  virtual bool Send(QString str);
//  void Receive(QString input);
  virtual KR_STATUS QueryStatus();

  protected:
  bool OfferServerSocket();

  protected slots:
   void socketClosed(KSocket *sock);
   void socketRead(KSocket *sock);
   void socketWrite(KSocket *sock);
   void socketRequest(KSocket *sock);



  signals:
  void signalPrepareMove(KEMessage *msg,KG_INPUTTYPE type); 
//  void signalReceiveMsg(KEMessage *msg); 
};

#endif
