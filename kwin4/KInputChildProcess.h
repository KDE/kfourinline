/***************************************************************************
                          KInputChildProcess.h  -  description
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
#ifndef _KINPUTCHILDPROCESS_H_
#define _KINPUTCHILDPROCESS_H_

#include <qobject.h>
#include "KEMessage.h"
#include "KChildConnect.h"


class KInputChildProcess : public QObject
{
  Q_OBJECT

  private:
  char *buffer;
  QString inputbuffer;
  int buffersize;
  bool terminateChild;
  protected:
  KChildConnect *childConnect;

  public:
  KInputChildProcess(int size_buffer=4096);
  ~KInputChildProcess();
  bool exec();
  virtual bool ReceiveMsg(KEMessage *msg,int id);
  // Forward calls to childconnect
  bool SendMsg(KEMessage *msg);
  // Immediately kills child's exec !
  void Terminate();
  bool IsTerminated();


  public slots:
  void slotReceiveMsg(KEMessage *msg,int id);

  signals:
  void signalReceiveMsg(KEMessage *msg,int id);
};


#endif
