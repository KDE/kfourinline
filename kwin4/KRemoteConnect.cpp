/***************************************************************************
                          KRemoteConnect.cpp  -  description
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "KRemoteConnect.h"

#include "KRemoteConnect.moc"

KRemoteConnect::KRemoteConnect()
  : KChildConnect()
{
  port=7687;
  IP="localhost";
  socketStatus=KR_INVALID;
  kSocket=0;
  kServerSocket=0;
  bufferMsg=0;
  buffer=new char[4097];
  inputbuffer="";
  input_pending=false;
}
KRemoteConnect::~KRemoteConnect()
{
  Exit();
  delete buffer;
//  printf("DESTGRUCTING KRemoteConenct\n");
}

KR_STATUS KRemoteConnect::QueryStatus()
{
  return socketStatus;
}

bool KRemoteConnect::Init(int id,KEMessage *msg)
{
short prt;
char *p;
int size;
bool tryserver;

  SetID(id);
  if (msg)
  {
    if (msg->GetData(QCString("Port"),prt)) 
    {
      port=(unsigned int)prt;
      msg->Remove(QCString("Port"));
    }
    if (msg->GetData(QCString("IP"),p,size)) 
    {
      IP=QCString(p);
      msg->Remove(QCString("IP"));
    }
  }
  /*
  printf("Connecting to %s %u (remain=%d)\n",
      (const char *)IP,port,msg->QueryNumberOfKeys());
  */
  
  // First try to connect to given host:socket
  // if no IP given only offer server
  tryserver=false;
  if (IP && IP.length()>0)
  {
    kSocket=new KSocket(IP.latin1(),port);
    if (!kSocket) return false;
    if (kSocket->socket()!=-1) // success
    {
      kSocket->enableRead(TRUE);
      kSocket->enableWrite(TRUE);
      connect(kSocket,SIGNAL(closeEvent(KSocket *)),
            this,SLOT(socketClosed(KSocket *)));
      connect(kSocket,SIGNAL(readEvent(KSocket *)),
            this,SLOT(socketRead(KSocket *)));
      /*
      connect(kSocket,SIGNAL(writeEvent(KSocket *)),
            this,SLOT(socketWrite(KSocket *)));
      */
      /*
      printf("Socket(%d) %p connection built to a server\n",
              kSocket->socket(),kSocket);
      */
      socketStatus=KR_CLIENT;

      // Send msg if any
      if (msg->QueryNumberOfKeys()>0)
      {
        msg->AddData(QCString("Server"),(short)QueryID());
        SendMsg(msg);
      }

    }
    else
    {
      tryserver=true;
    }
  }
  else
  {
    printf("NO IP given..only server possible\n");
    tryserver=true;
  }

  if (tryserver) // become a server
  {
    delete kSocket;
    kSocket=0;
    // Store message
    if (msg->QueryNumberOfKeys()>0)
    {
      bufferMsg=new KEMessage;
      *bufferMsg=*msg;
    }
    else
    {
      bufferMsg=0;
    }
    socketStatus=KR_WAIT_FOR_CLIENT;
    OfferServerSocket();
    return false;
  }

  return true;
}

bool KRemoteConnect::OfferServerSocket()
{
  if (kServerSocket)
  {
    return false;
  }
  kServerSocket=new KRServerSocket(port,SO_REUSEADDR);
  if (!kServerSocket)
  {
    socketStatus=KR_INVALID;
    return false;
  }
  if (kServerSocket->socket()==-1)
  {
    socketStatus=KR_NO_SOCKET;
    return false;
  }
  connect(kServerSocket,SIGNAL(accepted(KSocket *)),
        this,SLOT(socketRequest(KSocket *)));

  return true;
}
void KRemoteConnect::socketRequest(KSocket *sock)
{
  if (kSocket) // already connected
  {
    delete sock;
    delete kServerSocket;
    kServerSocket=0;
    return ;
  }
  kSocket=sock;
  if (kSocket->socket()!=-1) // success
  {
    kSocket->enableRead(TRUE);
    kSocket->enableWrite(TRUE);
    connect(kSocket,SIGNAL(closeEvent(KSocket *)),
          this,SLOT(socketClosed(KSocket *)));
    connect(kSocket,SIGNAL(readEvent(KSocket *)),
          this,SLOT(socketRead(KSocket *)));
    /*
    connect(kSocket,SIGNAL(writeEvent(KSocket *)),
          this,SLOT(socketWrite(KSocket *)));
    */
    socketStatus=KR_SERVER;
    delete kServerSocket; // no more connections
    kServerSocket=0;
    if (bufferMsg)
    {
      // Delayed sending of init msg
      bufferMsg->AddData(QCString("Client"),(short)QueryID());
      SendMsg(bufferMsg);
      delete bufferMsg;
      bufferMsg=0;
    }
  }
}

void KRemoteConnect::socketClosed(KSocket *sock)
{
  delete sock;
  kSocket=0;
  socketStatus=KR_INVALID;
  KEMessage *msg=new KEMessage;
  msg->AddData(QCString("ConnectionLost"),(short)QueryID());
  emit signalReceiveMsg(msg,QueryID());
  delete msg;
}
void KRemoteConnect::socketRead(KSocket *sock)
{
  ssize_t buflen;
  QString s;
  char c;
  int pos;

  // printf("++++++ Incoming socket read +++++++\n");
  if (-1==sock->socket()) return ;
  // printf("Read input on socket %p\n",sock);
  buflen=read(sock->socket(),buffer,4096);
  buffer[buflen]=0;
  // printf("Read %d byte  <%s>\n",buflen,buffer);

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


void KRemoteConnect::socketWrite(KSocket *sock)
{
 // printf("wrtie input on socket %p\n",sock);
}

bool KRemoteConnect::Exit()
{
  if (kSocket) delete kSocket;
  if (kServerSocket) delete kServerSocket;
  if (bufferMsg) delete bufferMsg;
  kSocket=0;
  kServerSocket=0;
  bufferMsg=0;
  socketStatus=KR_INVALID;
  return true;
}
bool KRemoteConnect::Next()
{
  bool result;
  // printf("+- KRemoteConnect::Next() status=%d\n",socketStatus);
  if (socketStatus<=0) return false;
  // create and send message
  KEMessage *msg=new KEMessage;
  // User fills message
  emit signalPrepareMove(msg,KG_INPUTTYPE_REMOTE); 
  result=SendMsg(msg);
  delete msg;
  return result;
}

// Send string to child
bool KRemoteConnect::Send(QString str)
{
  // connected?
  if (!kSocket || kSocket->socket()==-1) return false;
  if (socketStatus<=0) return false; 

  if (-1==write(kSocket->socket(),str.latin1(),str.length()+1))
  {
     printf("Warning: Problems writing to socket.\n");
     return false;
  }
  return true;;
}
