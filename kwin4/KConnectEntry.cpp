/***************************************************************************
                          KConnectEntry.cpp  -  description
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
#include <stdio.h>
#include <stdlib.h>
#include "KConnectEntry.h"

KG_INPUTTYPE KConnectEntry::QueryType()
{
  return type;
}

KRemoteConnect *KConnectEntry::QueryRemoteConnect()
{
  return connect.r;
}

KProcessConnect *KConnectEntry::QueryProcessConnect()
{
  return connect.p;
}

KInteractiveConnect *KConnectEntry::QueryInteractiveConnect()
{
  return connect.i;
}

KR_STATUS KConnectEntry::QueryStatus()
{
  switch(type)
  {
      case KG_INPUTTYPE_INTERACTIVE:
        return connect.i->QueryStatus();
      break;
      case KG_INPUTTYPE_REMOTE:
        return connect.r->QueryStatus();
      break;
      case KG_INPUTTYPE_PROCESS:
        return connect.p->QueryStatus();
      break;
    default:  
        return KR_INVALID;
      break;
  }
}
bool KConnectEntry::SendMsg(KEMessage *msg)
{
  switch(type)
  {
      case KG_INPUTTYPE_INTERACTIVE:
        return connect.i->SendMsg(msg);
      break;
      case KG_INPUTTYPE_REMOTE:
        return connect.r->SendMsg(msg);
      break;
      case KG_INPUTTYPE_PROCESS:
        return connect.p->SendMsg(msg);
      break;
    default:  
        return false;
      break;
  }
}

bool KConnectEntry::Exit()
{
  bool result;
  result=FALSE;
  switch(type)
  {
      case KG_INPUTTYPE_INTERACTIVE:
      result=connect.i->Exit();
      delete connect.i;
      break;
      case KG_INPUTTYPE_REMOTE:
      result=connect.r->Exit();
      delete connect.r;
      break;
      case KG_INPUTTYPE_PROCESS:
      result=connect.p->Exit();
      delete connect.p;
      break;
    default:  result=FALSE;
      break;
  }
  return result;
}

bool KConnectEntry::Init(KG_INPUTTYPE stype,int id,KEMessage *msg)
{
  bool result;
  type=stype;
  ID=id;
  switch(stype)
  {
      case KG_INPUTTYPE_INTERACTIVE:
        connect.i=new KInteractiveConnect;
        result=connect.i->Init(id,msg);
      break;
      case KG_INPUTTYPE_REMOTE:
        connect.r=new KRemoteConnect;
        result=connect.r->Init(id,msg);
      break;
      case KG_INPUTTYPE_PROCESS:
        connect.p=new KProcessConnect;
        result=connect.p->Init(id,msg);
      break;
    default: result=FALSE;
      break;
  }
  return result;
}
KConnectEntry::KConnectEntry()
{
  type=(KG_INPUTTYPE)0;
  connect.r=0;
}

KConnectEntry::~KConnectEntry()
{
//  printf("DESTRUCTING KCONNECTENTRY\n");
  Exit(); 
}

KConnectEntry::KConnectEntry(KConnectEntry &msg)
{
  *this=msg;
}
KConnectEntry &KConnectEntry::operator=(KConnectEntry &entry)
{
  // printf("&KConnectEntry::operator=:: I am not sure whether this is really a good idea...\n");
  Init(entry.QueryType(),entry.ID);

  return *this;
}
