/***************************************************************************
                          KMessageEntry.cpp  -  description
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "KMessageEntry.h"


void KMessageEntry::SetType(KGM_TYPE t)
{
  type=t;
}

KGM_TYPE KMessageEntry::QueryType()
{
  return type;
}

int KMessageEntry::QuerySize()
{
  return size;
}

char * KMessageEntry::QueryData()
{
  return data;
}

bool KMessageEntry::CopyData(int s,char *c)
{
  if (s<1) return false;
  data=(char *)calloc(s,1);
  if (!data) return false;
  // printf("  MessageEntry Copy Data to calloc %p\n",data);
  memcpy(data,c,s);
  size=s;
  return true;
}

KMessageEntry::KMessageEntry()
{
  // printf("KMessageEntry construct %p\n",this);
  size=0;
  type=(KGM_TYPE)0;
  data=(char *)0;
}

KMessageEntry::KMessageEntry(KMessageEntry &entry)
{
  // printf("KMessageEntry copy constructor from %p to %p\n",&entry,this);
  *this=entry;
}
KMessageEntry &KMessageEntry::operator=(KMessageEntry &entry)
{
  // printf("KMessageEntry operator= from %p to %p\n",&entry,this);
  SetType(entry.type);
  CopyData(entry.size,entry.data);
  return *this;
}

KMessageEntry::~KMessageEntry()
{
  // printf("MessageEntry destructor %p\n",this);
  // printf("  MessageEntry free %p\n",data);
  if (data) free(data);
  data=(char *)0;
}

