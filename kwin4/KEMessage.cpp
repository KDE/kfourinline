/***************************************************************************
                          KEMessage.cpp  -  description
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "KEMessage.h"

void KEMessage::AddEntry(QString key,KMessageEntry *entry)
{
  // printf("  AddingEntry: %s with data field %p\n",(char *)key,entry->QueryData());
  if (!entry) return ;
  dict.insert(key,entry);
  keys.append(key.latin1());
}

void KEMessage::AddDataType(QString key,int size,char *data,KGM_TYPE type)
{
//   printf("AddDataType for %s size=%d\n",(const char *)key,size);
  if (size<=0) return ;
  KMessageEntry *entry=new KMessageEntry;
  entry->SetType(type);
  entry->CopyData(size,data);
  AddEntry(key,entry);
}

void KEMessage::AddData(QString key,short data)
{
  AddDataType(key,sizeof(short),(char *)&data,KGM_TYPE_SHORT);
}

void KEMessage::AddData(QString key,long data)
{
  AddDataType(key,sizeof(long),(char *)&data,KGM_TYPE_LONG);
}

void KEMessage::AddData(QString key,float data)
{
  AddDataType(key,sizeof(float),(char *)&data,KGM_TYPE_FLOAT);
}

void KEMessage::AddData(QString key,char *data,int size)
{
  if (size<0) size=strlen(data)+1; // +1 for 0 Byte
  AddDataType(key,size,data,KGM_TYPE_DATA);
}

KGM_TYPE KEMessage::QueryType(QString key)
{
  KMessageEntry *entry;
  entry=dict.find(key);
  if (!entry) return (KGM_TYPE)0;
  return entry->QueryType();
}

bool KEMessage::HasKey(QString key)
{
  KMessageEntry *entry;
  entry=dict.find(key);
  if (!entry) return false;
  return true;
}

bool KEMessage::GetData(QString key,short &s)
{
  short *result;
  KMessageEntry *entry;
  entry=dict.find(key);
  if (!entry) return false;
  if (entry->QueryType()!=KGM_TYPE_SHORT) return false;
  // printf("GetShortData: %p for %s\n",entry->QueryData(),(char *)key);
  result=(short *)entry->QueryData();
  s=*result;
  return true;
}

bool KEMessage::GetData(QString key,long &l)
{
  long *result;
  KMessageEntry *entry;
  entry=dict.find(key);
  if (!entry) return false;
  if (entry->QueryType()!=KGM_TYPE_LONG) return false;
  result=(long *)entry->QueryData();
  l=*result;
  return true;
}

bool KEMessage::GetData(QString key,float &f)
{
  float *result;
  KMessageEntry *entry;
  entry=dict.find(key);
  if (!entry) return false;
  if (entry->QueryType()!=KGM_TYPE_FLOAT) return false;
  // printf("GetFloatData: %p for %s\n",entry->QueryData(),(char *)key);
  result=(float *)entry->QueryData();
  f=*result;
  return true;
}

bool KEMessage::GetData(QString key,char * &c,int &size)
{
  KMessageEntry *entry;
  entry=dict.find(key);
  if (!entry) return false;
  if (entry->QueryType()!=KGM_TYPE_DATA) return false;
  c=entry->QueryData();
  size=entry->QuerySize();
  return true;
}

QString KEMessage::EntryToString(char *key,KMessageEntry *entry)
{
  QString s,tmp;
  int size,i;
  KGM_TYPE type;
  char *data;
  s=QCString("");
  if (!entry) return s;
  size=entry->QuerySize();
  type=entry->QueryType();
  data=entry->QueryData();

  // Key
  /*
  tmp.sprintf("%s%s%d%s%d%s",
                key,KEMESSAGE_SEP,
                size,KEMESSAGE_SEP,
                (int)type,KEMESSAGE_SEP);
  */
  tmp=QCString(key);
  s+=tmp;
  s+=KEMESSAGE_SEP;
  tmp.sprintf("%d",size);
  s+=tmp;
  s+=KEMESSAGE_SEP;
  tmp.sprintf("%d",(int)type);
  s+=tmp;
  s+=KEMESSAGE_SEP;


  // We ignore the type of data and process them all as
  // byte sequence
  for (i=0;i<size;i++)
  {
    // Convert to 4 bit value ... someone can improves
    tmp.sprintf("%c%c",
       'a'+(data[i]&15),
       'a'+((data[i]>>4)&15));
    s+=tmp;
  }
  s+=KEMESSAGE_CR;

  return s;
}

QString KEMessage::StringToEntry(QString str,KMessageEntry *entry)
{
  int pos,oldpos,cnt,len;
  QString key,size,type,data;
  const char *p;
  char *q;
  char c;

  len=KEMESSAGE_SEP.length();

  if (!entry) return QString();
  pos=str.find(KEMESSAGE_SEP,0);
  if (pos<0) return QString();   // wrong format
  key=str.left(pos);


  oldpos=pos;
  pos=str.find(KEMESSAGE_SEP,oldpos+len);
  if (pos<0) return QString();   // wrong format
  size=str.mid(oldpos+len,pos-oldpos-len);


  oldpos=pos;
  pos=str.find(KEMESSAGE_SEP,oldpos+len);
  if (pos<0) return QString();   // wrong format
  type=str.mid(oldpos+len,pos-oldpos-len);


  data=str.right(str.length()-pos-len);

  
  cnt=size.toInt();
  entry->SetType((KGM_TYPE)type.toInt());

  // I hope this works with unicode strings as well
  p=data.latin1();
  q=(char *)calloc(data.length()/2,sizeof(char));
  if (!q) return QString();
  for(pos=0;pos<cnt;pos++)
  {
    if (pos*2+1>(int)data.length()) return QString(); // SEVERE ERROR
    c=*(p+2*pos)-'a' | ((*(p+2*pos+1)-'a')<<4); 
    q[pos]=c;
  }
  entry->CopyData(cnt,q);

  free(q);
  return key; 
}

QString KEMessage::ToString()
{
  QString s;
  KMessageEntry *entry;
  char *it;
  s=KEMESSAGE_HEAD+KEMESSAGE_CR;
  for (it=keys.first();it!=0;it=keys.next())
  {
    entry=dict.find(QCString(it));
    s+=EntryToString(it,entry);
  }
  s+=KEMESSAGE_TAIL+KEMESSAGE_CR;
  return s;
}

bool KEMessage::AddString(QString s)
{
    // break s into key,size and data
    QString key;
    KMessageEntry *entry=new KMessageEntry;
    key=StringToEntry(s,entry);
    if (!key) return false;
    AddEntry(key,entry);
    return true;
}
bool KEMessage::AddStringMsg(QString str)
{
  bool result;
  QString data;
  int pos,oldpos,len;

  len=KEMESSAGE_CR.length();

  pos=str.find(KEMESSAGE_CR);
  if (pos<0) return false;  // wrong format
  if (str.left(pos)!=(KEMESSAGE_HEAD)) return false; // wrong message

  do
  {
    oldpos=pos;
    pos=str.find(KEMESSAGE_CR,oldpos+len);
    if (pos<0) return false;   // wrong format
    data=str.mid(oldpos+len,pos-oldpos-len);
    if (data!=(KEMESSAGE_TAIL))
    {
      result=AddString(data);
      if (!result) return false;   // wrong format
    }
  }while(data!=(KEMESSAGE_TAIL));
  
  return result;
}

void KEMessage::RemoveAll()
{
  keys.clear();
  dict.clear();
}

void KEMessage::Remove(QString key)
{
  keys.remove(key.latin1());
  dict.remove(key);
}

uint KEMessage::QueryNumberOfKeys()
{
  return keys.count();
}
QStrList *KEMessage::QueryKeys()
{
  return &keys;
}

KEMessage::~KEMessage()
{
  // printf("Deleteing KEMessage %p\n",this);
}
KEMessage::KEMessage()
{
  // printf("KEMessage construct %p\n",this);
  dict.setAutoDelete(true);
}
KEMessage::KEMessage(KEMessage &msg)
{
  // printf("KEMessage copy constructor from %p to %p\n",&msg,this);
  *this=msg;
}
KEMessage &KEMessage::operator=(KEMessage &msg)
{
  // KEMessage *newmsg=new KEMessage;
  KMessageEntry *entry;
  KMessageEntry *newentry;
  char *it;
  // printf("Assigning = KEMessage from %p to %p\n",&msg,this);
  for (it=msg.keys.first();it!=0;it=msg.keys.next())
  {
    entry=msg.dict.find(QCString(it));
    newentry=new KMessageEntry;
    *newentry=*entry;
    AddEntry(QCString(it),newentry);

  }
  // return *newmsg;
  return *this;
}
