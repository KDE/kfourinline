/***************************************************************************
                          KEMessage.h  -  description
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
#ifndef _KEMESSAGE_H_
#define _KEMESSAGE_H_

#include <string.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qdict.h>
#include "KMessageEntry.h"

#define KEMESSAGE_HEAD QString(QCString("BEGIN_V1000"))
#define KEMESSAGE_TAIL QString(QCString("END_V1000"))
#define KEMESSAGE_CR   QString(QCString("\n"))
#define KEMESSAGE_SEP  QString(QCString(":::"))

class KEMessage
{
  private:
  QStrList keys;
  QDict<KMessageEntry> dict;

  protected:
  void AddEntry(QString key,KMessageEntry *entry);
  public:
  QStrList *QueryKeys();
  uint QueryNumberOfKeys();
  void AddDataType(QString key,int size,char *data,KGM_TYPE type);
  void AddData(QString key,short data);
  void AddData(QString key,long data);
  void AddData(QString key,float data);
  void AddData(QString key,char *data,int size=-1);
  bool GetData(QString key,short &s);
  bool GetData(QString key,long &l);
  bool GetData(QString key,float &f);
  bool GetData(QString key,char * &c,int &size);
  bool HasKey(QString key);
  void Remove(QString key);
  KGM_TYPE QueryType(QString key);
  QString ToString();
  QString EntryToString(char *key,KMessageEntry *entry);
  QString StringToEntry(QString str,KMessageEntry *entry);
  bool AddString(QString s);
  bool AddStringMsg(QString str);
  void RemoveAll();
  ~KEMessage();
  KEMessage();
  KEMessage(KEMessage &msg);
  KEMessage &operator=(KEMessage &msg);
};

#endif
