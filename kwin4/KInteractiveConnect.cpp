/***************************************************************************
                          KInteractiveConnect.cpp  -  description
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
#include "KInteractiveConnect.h"

#include "KInteractiveConnect.moc"

bool KInteractiveConnect::Next()
{
  // Dummy only for interactive
  KEMessage *msg=new KEMessage;
  emit signalPrepareMove(msg,KG_INPUTTYPE_INTERACTIVE); 
  delete msg;
  return true;
}
bool KInteractiveConnect::Init(int id,KEMessage *msg)
{
  // emit signalReceiveMsg(msg);
  SetID(id);
  return true;
}
bool KInteractiveConnect::Exit()
{
  return true;
}

KInteractiveConnect::~KInteractiveConnect()
{
//  printf("DESTRUCTING INTERACTIVE\n");
}

KInteractiveConnect::KInteractiveConnect()
  : KChildConnect()
{
}

bool KInteractiveConnect::SendMsg(KEMessage *msg)
{
  printf("+- Interactive::SendMessage MESSAGE::Emmiting slotReceiveMsg\n");
  emit signalReceiveMsg(msg,QueryID());
  return true;
}
// Try not to use this..prodices just unneccessary string-msg
// conversion
bool KInteractiveConnect::Send(QString str)
{
  Receive(str);
  return true;
}
