/***************************************************************************
                          KEInput.h  -  description
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
#ifndef _KEINPUT_H_
#define _KEINPUT_H_

#include <string.h>
#include <qlist.h>
#include <qobject.h>
#include <qtimer.h>
#include <qarray.h>
#include "KConnectEntry.h"
#include "KRemoteConnect.h"
#include "KProcessConnect.h"
#include "KInteractiveConnect.h"
#include "KEMessage.h"
#include "KConnectTypes.h"


class KEInput : public QObject
{
   Q_OBJECT

  private:
   int number_of_inputs;
   int previous_input,next_input;
   bool locked;
//   KEMessage *mMsg;
   QTimer *cTimer;

   QList<KRemoteConnect> remoteList;
   QList<KProcessConnect> computerList;
   QList<KInteractiveConnect> interactiveList;
   QArray<KConnectEntry> playerArray;

  public:
   KEInput(QObject * parent=0);
   ~KEInput();
   int QueryNumberOfInputs();
   int QueryNext();
   bool IsInteractive(int no=-1);
   bool IsProcess(int no=-1);
   bool IsRemote(int no=-1);
   int QueryPrevious();
   KG_INPUTTYPE QueryType(int no=-1);
   KR_STATUS QueryStatus(int no=-1);
   bool IsInput(int no);
   bool SetInputDevice(int no, KG_INPUTTYPE type, KEMessage *msg=0);
   bool RemoveInput(int no);
   bool Next(int number, bool force=false);
   bool SetInput(KEMessage *msg,int number=-1);
   bool IsLocked();
   bool SendMsg(KEMessage *msg,int no=-1);
   void Unlock();
   void Lock();

  public slots:
   void slotTimerNextRemote();
   void slotTimerNextProcess();
   void slotSetInput(KEMessage *msg,int id);
   void slotPrepareMove(KEMessage *msg,KG_INPUTTYPE type);

  signals:
  void signalReceiveInput(KEMessage *msg,int id);
  void signalPrepareRemoteMove(KEMessage *msg);
  void signalPrepareProcessMove(KEMessage *msg);
  void signalPrepareInteractiveMove(KEMessage *msg);


};
#endif
