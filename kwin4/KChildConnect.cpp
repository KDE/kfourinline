/***************************************************************************
                          KChildConnect.cpp  -  description
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
#include <stdio.h>
#include "KChildConnect.h"

#include "KChildConnect.moc"

KChildConnect::KChildConnect()
  : QObject(0,0)
{
  input_pending=false;
  inputbuffer="";
}

KChildConnect::~KChildConnect()
{
}

KR_STATUS KChildConnect::QueryStatus()
{
  return KR_OK;
}

// Communication with process
bool KChildConnect::SendMsg(KEMessage *msg)
{
  QString sendstring=msg->ToString();
  // Debug only
  if (msg->HasKey(QCString("KLogSendMsg")))
  {
    char *p;
    int size;
    FILE *fp;
    msg->GetData(QCString("KLogSendMsg"),p,size);
    if (p && (fp=fopen(p,"a")) )
    {
      fprintf(fp,"------------------------------------\n");
      fprintf(fp,sendstring);
      fclose(fp);
    }
  }
  // end debug only
  return Send(sendstring);
}

// Send string to parent
bool KChildConnect::Send(QString str)
{
  if (!str || str.length()<1) return true; // no need to send crap
  printf("%s",str.latin1());
  fflush(stdout);
  return true;
}

void KChildConnect::Receive(QString input)
{
  // Cut out CR
  int len,pos;
  QString tmp;


  // Call us recursive until there are no CR left
  len=KEMESSAGE_CR.length();
  pos=input.find(KEMESSAGE_CR);
  if (pos>0)
  {
    tmp=input.left(pos);
    if (tmp.length()>0) Receive(tmp); // CR free
    input=input.right(input.length()-pos-len);
    if (input.length()>0) Receive(input);
    return ;
  }

//  printf("  ---> KChildConnect::Receive: '%s'\n",(const char *)input);
  if (input==(KEMESSAGE_HEAD) && !input_pending) 
  {
    input_pending=true;
    inputcache.clear();
    return ;
  }
  if (!input_pending) return ; // ignore
  if (input!=(KEMESSAGE_TAIL))
  {
    inputcache.append(input.latin1());
    return;
  }
  input_pending=0;

  KEMessage *msg=new KEMessage;
  char *it;
  for (it=inputcache.first();it!=0;it=inputcache.next())
  {
    msg->AddString(QCString(it));
  }

//  printf("+- CHILDprocess:: GOT MESSAGE::Emmiting slotReceiveMsg\n");
  emit signalReceiveMsg(msg,ID);
  
  delete msg;
}

void KChildConnect::SetID(int id)
{
  ID=id;
}
int KChildConnect::QueryID()
{
  return ID;
}

