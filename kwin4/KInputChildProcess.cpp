/***************************************************************************
                          KInputChildProcess.cpp  -  description
                             -------------------
    begin                : Tue May 2 2000
    copyright            : (C) 2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qstring.h>

#include "KInputChildProcess.h"
#include "KInputChildProcess.moc"


KInputChildProcess::~KInputChildProcess()
{
  delete buffer;
  delete childConnect;
}
KInputChildProcess::KInputChildProcess(int size_buffer)
  : QObject(0,0)
{
  buffersize=size_buffer;
  if (buffersize<1) buffersize=1024;
  buffer=new char[buffersize];
  inputbuffer="";
  terminateChild=false;
}
bool KInputChildProcess::exec()
{
  int pos;
  QString s;

  childConnect=new KChildConnect;
  if (!childConnect) return false;
  connect(childConnect,SIGNAL(signalReceiveMsg(KEMessage *,int)),
          this,SLOT(slotReceiveMsg(KEMessage *,int)));
  do
  {
    // Wait for input
    if (feof(stdin)) 
    {
      sleep(1);
      continue;
    }

    if (!fgets(buffer,buffersize,stdin) )
    {
     continue;
    }
    s=buffer;
    s=inputbuffer+s;
    // printf("ChildABC '%s'\n",(const char *)s);
    // fflush(stdout);
    pos=s.findRev(KEMESSAGE_CR);
    if (pos<0)
    {
      inputbuffer=s;
    }
    else if (pos+KEMESSAGE_CR.length()==s.length())
    {
      // CR at the end...calling receive
      childConnect->Receive(s);
    }
    else
    {
      inputbuffer=s.right(s.length()-pos-KEMESSAGE_CR.length());
      s=s.left(pos+KEMESSAGE_CR.length());
      // printf("s='%s' in='%s'\n",(const char *)s,(const char *)inputbuffer);
      childConnect->Receive(s);
    }
  }while(!terminateChild);
  return true;
}

void KInputChildProcess::Terminate()
{
  terminateChild=true;
}
bool KInputChildProcess::IsTerminated()
{
  return terminateChild;
}

bool KInputChildProcess::ReceiveMsg(KEMessage *msg,int id)
{
  return false;
}
void KInputChildProcess::slotReceiveMsg(KEMessage *msg,int id)
{
  if (!ReceiveMsg(msg,id))  // made for virtual override
  {
    // otherwise emit signal
    emit signalReceiveMsg(msg,id);
  }
}
bool KInputChildProcess::SendMsg(KEMessage *msg)
{
  if (childConnect) return childConnect->SendMsg(msg);
  return false;
}


