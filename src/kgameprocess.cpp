/*
    This file is part of the KDE games library
    Copyright (C) 2001 Martin Heni (kde at heni-online.de)
    Copyright (C) 2001 Andreas Beckermann (b_mann@gmx.de)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kgameprocess.h"

#include <KRandomSequence>

#include <QBuffer>
#include <QDataStream>
#include <QFile>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>

#define READ_BUFFER_SIZE  1024

class KGameProcessPrivate
{
public:
    QFile rFile;
    QFile wFile;
    KRandomSequence* mRandom;
};

// ----------------------- Process Child ---------------------------

KGameProcess::KGameProcess()
    : QObject(), d(new KGameProcessPrivate)
{
  mTerminate=false;
  // Check whether a player is set. If not create one!
  d->rFile.open(stdin, QIODevice::ReadOnly|QIODevice::Unbuffered);
  d->wFile.open(stdout, QIODevice::WriteOnly|QIODevice::Unbuffered);
  mMessageIO = new KMessageFilePipe(this, &d->rFile, &d->wFile);
//  mMessageClient=new KMessageClient(this);
//  mMessageClient->setServer(mMessageIO);
//  connect (mMessageClient, SIGNAL(broadcastReceived(QByteArray,quint32)),
//          this, SLOT(receivedMessage(QByteArray,quint32)));
  connect(mMessageIO, &KMessageFilePipe::received, this, &KGameProcess::receivedMessage);
 
  d->mRandom = new KRandomSequence;
  d->mRandom->setSeed(0);
}
KGameProcess::~KGameProcess() 
{
  delete d->mRandom;
  //delete mMessageClient;
  //delete mMessageServer;
  fprintf(stderr,"KGameProcess::destructor\n");
  fflush(stderr);
  delete mMessageIO;
  d->rFile.close();
  d->wFile.close();
  delete d;
}


bool KGameProcess::exec()
{
  do
  {
    mMessageIO->exec();
  }  while(!mTerminate);
  return true;
}

//    You have to do this to create a message 
//    QByteArray buffer;
//    QDataStream wstream(buffer,QIODevice::WriteOnly);
//    then stream data into the stream and call this function
void KGameProcess::sendSystemMessage(QDataStream &stream,int msgid,quint32 receiver)
{
  fprintf(stderr,"KGameProcess::sendSystemMessage to parent id=%d recv=%ld\n",msgid,(unsigned long)receiver);
  QByteArray a;
  QDataStream outstream(&a,QIODevice::WriteOnly);

  QBuffer *device=(QBuffer *)stream.device();
  QByteArray data=device->buffer();

  KGameMessage::createHeader(outstream,0,receiver,msgid);
  outstream.writeRawData(data.data(),data.size());

  //  if (mMessageClient) mMessageClient->sendForward(a,2);
  if (mMessageIO) mMessageIO->send(a);
  else fprintf(stderr,"KGameProcess::sendSystemMessage:: NO IO DEVICE ... WILL FAIL\n");
}

void KGameProcess::sendMessage(QDataStream &stream,int msgid,quint32 receiver)
{
  sendSystemMessage(stream,msgid+KGameMessage::IdUser,receiver);
}

void KGameProcess::receivedMessage(const QByteArray& receiveBuffer)
{
 QDataStream stream(receiveBuffer);
 int msgid;
 quint32 sender; 
 quint32 receiver; 
 KGameMessage::extractHeader(stream, sender, receiver, msgid);
 fprintf(stderr,"--- KGameProcess::receivedMessage(): id=%d sender=%ld,recv=%ld\n",
         msgid,(unsigned long)sender,(unsigned long)receiver);
 switch(msgid)
 {
   case KGameMessage::IdTurn:
     qint8 b;
     stream >> b;
     emit signalTurn(stream,(bool)b);
   break;
   case KGameMessage::IdIOAdded:
     qint16 id;
     stream >> id;
     emit signalInit(stream,(int)id);
   break;
   default:
      emit signalCommand(stream,msgid-KGameMessage::IdUser,receiver,sender);
   break;
 }
}

KRandomSequence* KGameProcess::random()
{
  return d->mRandom;
}

// ----------------------- KMessageFilePipe ---------------------------
KMessageFilePipe::KMessageFilePipe(QObject *parent,QFile *readfile,QFile *writefile) : KMessageIO(parent)
{
  mReadFile=readfile;
  mWriteFile=writefile;
  mReceiveCount=0;
  mReceiveBuffer.resize(1024);
}

KMessageFilePipe::~KMessageFilePipe()
{
}

bool KMessageFilePipe::isConnected () const
{
  return (mReadFile!=nullptr)&&(mWriteFile!=nullptr);
}

// Send to parent
void KMessageFilePipe::send(const QByteArray &msg)
{
  unsigned int size=msg.size()+2*sizeof(long);

  char *tmpbuffer=new char[size];
  long *p1=(long *)tmpbuffer;
  long *p2=p1+1;
  memcpy(tmpbuffer+2*sizeof(long),msg.data(),msg.size());
  *p1=0x4242aeae;
  *p2=size;

  QByteArray buffer(tmpbuffer,size);
  mWriteFile->write(buffer);
  mWriteFile->flush();
  delete [] tmpbuffer;

  /* DEBUG:
  fprintf(stderr,"+++ KMessageFilePipe:: SEND(%d to parent) realsize=%d\n",msg.size(),buffer.size());
  for (int i=0;i<buffer.size();i++) fprintf(stderr,"%02x ",(unsigned char)buffer.at(i));fprintf(stderr,"\n");
  fflush(stderr);
  */


}

void KMessageFilePipe::exec()
{

  // According to BL: Blocking read is ok
  // while(mReadFile->atEnd()) { usleep(100); }
   char ch;
   mReadFile->getChar(&ch);

   while (mReceiveCount>=mReceiveBuffer.size()) mReceiveBuffer.resize(mReceiveBuffer.size()+1024);
   mReceiveBuffer[mReceiveCount]=ch;
   mReceiveCount++;

   // Change for message
   if (mReceiveCount>=int(2*sizeof(long)))
   {
     long *p1=(long *)mReceiveBuffer.data();
     long *p2=p1+1;
     int len;
     if (*p1!=0x4242aeae)
     {
       fprintf(stderr,"KMessageFilePipe::exec:: Cookie error...transmission failure...serious problem...\n");
       fflush(stderr);
//       for (int i=0;i<16;i++) fprintf(stderr,"%02x ",mReceiveBuffer[i]);fprintf(stderr,"\n");
     }
     len=(int)(*p2);
     if (len==mReceiveCount)
     {
       //fprintf(stderr,"KMessageFilePipe::exec:: Got Message with len %d\n",len);

       QByteArray msg;
       msg.resize(len);
       //msg.setRawData(mReceiveBuffer.data()+2*sizeof(long),len-2*sizeof(long));
       std::copy(mReceiveBuffer.begin()+2*sizeof(long),mReceiveBuffer.begin()+len, msg.begin());
//  msg.duplicate(mReceiveBuffer.data()+2*sizeof(long),len-2*sizeof(long));
       emit received(msg);
       //msg.resetRawData(mReceiveBuffer.data()+2*sizeof(long),len-2*sizeof(long));
       mReceiveCount=0;
     }
   }


   return ;


}


