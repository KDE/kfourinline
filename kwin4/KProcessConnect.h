/***************************************************************************
                          KProcessConnect.h  -  description
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
#ifndef _KPROCESSCONNECT_H_
#define _KPROCESSCONNECT_H_

#include <qstrlist.h>
#include <kprocess.h>
#include <ksock.h>
// #include "KEInput.h"
#include "KEMessage.h"
#include "KChildConnect.h"


class KProcessConnect: public KChildConnect
{
  Q_OBJECT

  private:
  KProcess *process;
  QString processname;
  bool running;

  public:
  KProcessConnect();
  ~KProcessConnect();
  bool Init(int id=0,KEMessage *msg=0);
  bool Exit();
  bool Next();
 //  bool SendMsg(KEMessage *msg);
  virtual bool Send(QString str);
  // void Receive(QString input);

  public slots:
  void  slotReceivedStdout(KProcess *proc, char *buffer, int buflen);
  void  slotProcessExited(KProcess *p);
  void  slotWroteStdin(KProcess *p);


  signals:
  void signalPrepareMove(KEMessage *msg,KG_INPUTTYPE type); 
//  void signalReceiveMsg(KEMessage *msg); 
};

#endif
