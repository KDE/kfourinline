/***************************************************************************
                          KProcessConnect.cpp  -  description
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
/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Tue Apr 4 2000
    copyright            : (C) |1995-2000 by Martin Heni
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
#include <stdio.h>
#include "KProcessConnect.h"

#include "KProcessConnect.moc"

KProcessConnect::KProcessConnect()
  : KChildConnect()
{
  running=false;
  process=0;
}

KProcessConnect::~KProcessConnect()
{
  Exit();
  if (process) delete process;
//  printf("DESTRUCTRING KPROCESSCONNECT\n");
}

bool KProcessConnect::Init(int id,KEMessage *msg)
{
  int size;
  char *p;

  SetID(id);
  if (msg)
  {
    if (!msg->GetData(QCString("ProcessName"),p,size)) return false; // no process name
    processname=p;
    /*
    printf("Found processname '%s' size %d size=%u\n",
       p,size,msg->QueryNumberOfKeys());
    */
    msg->Remove(QCString("ProcessName"));
  }
  if (processname.length()<1) return false;

  inputbuffer="";

  // Delete first on multiple init
  if (running) Exit();

  // create process
  process=new KProcess;
  *process << processname;
  connect(process, SIGNAL(receivedStdout(KProcess *, char *, int )), 
                        this, SLOT(slotReceivedStdout(KProcess *, char * , int )));
  connect(process, SIGNAL(processExited(KProcess *)), 
                        this, SLOT(slotProcessExited(KProcess *)));
  /*
  connect(process, SIGNAL(wroteStdin(KProcess *)), 
                        this, SLOT(slotWroteStdin(KProcess *)));
  */

  // TRUE if ok
  running=process->start(KProcess::NotifyOnExit,KProcess::All);

  if (running && msg && msg->QueryNumberOfKeys()>0)
  {
    SendMsg(msg);
  }

  return running;
}

void KProcessConnect::slotReceivedStdout(KProcess *proc, char *buffer, int buflen)
{
  QString s;
  char c;
  int pos;

  if (buflen<1) return ;
  if (buffer[buflen-1]!=0) // shit..we got a not null terminated string
  {
    c=buffer[buflen-1];
    buffer[buflen-1]=0;
    s=buffer;
    s+=c;
  }
  else
  {
    s=buffer;
  }
  // Append old unresolved input
  s=inputbuffer+s;
  pos=s.findRev(KEMESSAGE_CR);
  // printf("String '%s' pos=%d len=%d\n",(const char *)s,pos,s.length());
  if (pos<0)
  {
    inputbuffer=s;
  }
  else if (pos+KEMESSAGE_CR.length()==s.length())
  {
    // CR at the end...calling receive
    Receive(s);
  }
  else
  {
    inputbuffer=s.right(s.length()-pos-KEMESSAGE_CR.length());
    s=s.left(pos+KEMESSAGE_CR.length());
    // printf("s='%s' in='%s'\n",(const char *)s,(const char *)inputbuffer);
    Receive(s);
  }
}
void KProcessConnect::slotProcessExited(KProcess *p)
{
  running=false;
  delete process;
  process=0;
  Init(QueryID());
}
void KProcessConnect::slotWroteStdin(KProcess *p)
{
  printf("slotWroteStdin:: IS NEVER CALLED\n");
}

bool KProcessConnect::Exit()
{
  // kill process if running
  if (running)
  {
    running=false;
    if (process) process->kill();
    delete process;
    process=0;
  }
  return true;
}

bool KProcessConnect::Next()
{
  bool result;
  if (!running) return false;
  // create and send message
  // printf("+- KProcessConnect::ProcessNext\n");
  KEMessage *msg=new KEMessage;
  // User fills message
  emit signalPrepareMove(msg,KG_INPUTTYPE_PROCESS); 
  result=SendMsg(msg);
  delete msg;
  return result;
}

// Send string to child
bool KProcessConnect::Send(QString str)
{
  bool result;
  // printf("****** PROCESS:SEND\n");
  if (!running || !process) return false;
  if (!str || str.length()<1) return true; // no need to send crap
  // TODO ..why?
  QString s;
  s=KEMESSAGE_CR+KEMESSAGE_CR;
  str=s+str;
  // printf("+++ Sending to child '%s'!!!\n",(const char *)str);
  result=process->writeStdin(str.latin1(),str.length()+1);
  if (!result) printf("ERROR in PROCESS SEND\n");
  return result;
}

