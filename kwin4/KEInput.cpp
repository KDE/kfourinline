/***************************************************************************
                          KEInput.cpp  -  description
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
#include "KEInput.h"
#include "KConnectEntry.h"

#define K_INPUT_DELAY 25     // delay following non interactive moves

#include "KEInput.moc"

KEInput::KEInput(QObject * parent)
  : QObject(parent,0)
{
  number_of_inputs=0;
  locked=FALSE;
  previous_input=-1;
  next_input=-1;
  cTimer=0;
//  mMsg=(KEMessage *)0;
}

KEInput::~KEInput()
{
  int i;
  for (i=number_of_inputs-1;i>=0;i--)
  {
    RemoveInput(i);
  }
  if (cTimer) delete cTimer;
}

int KEInput::QueryNumberOfInputs()
{
  return number_of_inputs;
}

int KEInput::QueryNext()
{
  return next_input;
}

int KEInput::QueryPrevious()
{
  return previous_input;
}

bool KEInput::IsInput(int no)
{
  if (no>=number_of_inputs || no<0) return FALSE;
  if (QueryType(no)==0) return FALSE;
  return TRUE;
}

// Default is the type of the current player
bool KEInput::IsInteractive(int no)
{
  return QueryType(no)==KG_INPUTTYPE_INTERACTIVE;
}
bool KEInput::IsProcess(int no)
{
  return QueryType(no)==KG_INPUTTYPE_PROCESS;
}
bool KEInput::IsRemote(int no)
{
  return QueryType(no)==KG_INPUTTYPE_REMOTE;
}
KG_INPUTTYPE KEInput::QueryType(int no)
{
  if (no==-1) no=QueryNext();
  if (no>=number_of_inputs || no<0) return KG_INPUTTYPE_INVALID;
  return playerArray[no].QueryType();
}

KR_STATUS KEInput::QueryStatus(int no)
{
  if (no==-1) no=QueryNext();
  if (no>=number_of_inputs || no<0)
  {
    return KR_INVALID;
  }
  return playerArray[no].QueryStatus();
}
bool KEInput::SendMsg(KEMessage *msg,int no)
{
  if (no==-1) no=QueryNext();
  if (no>=number_of_inputs || no<0)
  {
    return false;
  }
  return playerArray[no].SendMsg(msg);
}


bool KEInput::SetInputDevice(int no, KG_INPUTTYPE type,KEMessage *msg)
{
  bool result;
  // Grow if necessary
  if (no<0) return false;
  else if (no<number_of_inputs)
  {
    RemoveInput(no);
  }
  if (no>=number_of_inputs)
  {
    playerArray.resize(no+1);
    number_of_inputs=no+1;
  }
  result=playerArray[no].Init(type,no,msg);
  // if (result)
  // Connect even if remote connection is not yet build
  if (result || playerArray[no].QueryStatus()==KR_WAIT_FOR_CLIENT)
  {
    switch(QueryType(no))
    {
        case KG_INPUTTYPE_INTERACTIVE:
          connect(playerArray[no].QueryInteractiveConnect(),SIGNAL(signalReceiveMsg(KEMessage *,int )),
                  this,SLOT(slotSetInput(KEMessage *,int )));
          connect(playerArray[no].QueryInteractiveConnect(),SIGNAL(signalPrepareMove(KEMessage *,KG_INPUTTYPE)),
                  this,SLOT(slotPrepareMove(KEMessage *,KG_INPUTTYPE)));
        break;
        case KG_INPUTTYPE_REMOTE:
          connect(playerArray[no].QueryRemoteConnect(),SIGNAL(signalReceiveMsg(KEMessage *,int )),
                  this,SLOT(slotSetInput(KEMessage *,int )));
          connect(playerArray[no].QueryRemoteConnect(),SIGNAL(signalPrepareMove(KEMessage *,KG_INPUTTYPE)),
                  this,SLOT(slotPrepareMove(KEMessage *,KG_INPUTTYPE)));
        break;
        case KG_INPUTTYPE_PROCESS:
          connect(playerArray[no].QueryProcessConnect(),SIGNAL(signalReceiveMsg(KEMessage *,int )),
                  this,SLOT(slotSetInput(KEMessage *,int )));
          connect(playerArray[no].QueryProcessConnect(),SIGNAL(signalPrepareMove(KEMessage *,KG_INPUTTYPE)),
                  this,SLOT(slotPrepareMove(KEMessage *,KG_INPUTTYPE)));
        break;
      default:  
        break;
    }
  }
  return result;
}

bool KEInput::RemoveInput(int no)
{
  bool result;
  if (no>=number_of_inputs || no<0) return FALSE;
  result=playerArray[no].Exit();
  // shrink if last entry is removed
  if (no==number_of_inputs-1)
  {
    playerArray.resize(no);
    number_of_inputs=no;
  }
  return result;
}


// Sets a new player and sends it a message
bool KEInput::Next(int number, bool force)
{
  if (locked && !force) return FALSE;
  if (!IsInput(number)) return FALSE;
  locked=TRUE; 

  // printf("KEInput::Next %d OK  ... lock set!!\n",number);

  previous_input=next_input;
  next_input=number;

  switch(QueryType(number))
  {
    case KG_INPUTTYPE_INTERACTIVE:
      playerArray[number].QueryInteractiveConnect()->Next();
    break;
    case KG_INPUTTYPE_REMOTE:
      if (QueryType(previous_input)!=0 &&
          QueryType(previous_input)!=KG_INPUTTYPE_INTERACTIVE)
      {
        // delay non interactive move to allow interactive inout
        if (cTimer) delete cTimer; // Ouch...
        cTimer=new QTimer(this);
        connect(cTimer,SIGNAL(timeout()),this,SLOT(slotTimerNextRemote()));
        cTimer->start(K_INPUT_DELAY,TRUE);
      }
      else    
      {
        playerArray[number].QueryRemoteConnect()->Next();
      }
    break;
    case KG_INPUTTYPE_PROCESS:
      if (QueryType(previous_input)!=0 &&
          QueryType(previous_input)!=KG_INPUTTYPE_INTERACTIVE)
      {
        // delay non interactive move to allow interactive inout
        cTimer=new QTimer(this);
        connect(cTimer,SIGNAL(timeout()),this,SLOT(slotTimerNextProcess()));
        cTimer->start(K_INPUT_DELAY,TRUE);
      }
      else    
        playerArray[number].QueryProcessConnect()->Next();
    break;
    default: return FALSE;
  }
  return TRUE;
}

void KEInput::slotTimerNextRemote()
{
  delete cTimer;
  cTimer=0;
  if (next_input>=0 && next_input<number_of_inputs)
    playerArray[next_input].QueryRemoteConnect()->Next();
}

void KEInput::slotTimerNextProcess()
{
  delete cTimer;
  cTimer=0;
  if (next_input>=0 && next_input<number_of_inputs)
    playerArray[next_input].QueryProcessConnect()->Next();
}

// called to prepare a move which is send to a remote/computer
// should fill in message data
void KEInput::slotPrepareMove(KEMessage *msg,KG_INPUTTYPE type)
{
  // just forward it
  switch(type)
  {
    case KG_INPUTTYPE_INTERACTIVE:
      emit signalPrepareInteractiveMove(msg);
    break;
    case KG_INPUTTYPE_PROCESS:
      emit signalPrepareProcessMove(msg);
    break;
    case KG_INPUTTYPE_REMOTE:
      emit signalPrepareRemoteMove(msg);
    break;
    default: // Do nothing
    break;
  }
}
// called by ReceiveMsg
void KEInput::slotSetInput(KEMessage *msg,int id)
{
  if (!msg) return ;
  SetInput(msg,id);
}

bool KEInput::SetInput(KEMessage *msg,int number)
{
  /*
  if (!locked)
  {
    printf("KEINput:SetInput not locked(should be) returning FALSE\n");
    return FALSE;
  }
  */
  if (number<0 || number>=number_of_inputs) number=next_input; // automatically select player
//  KEMessage *mMsg= new KEMessage;
  // evt if (mMsg) delete mMsg;
//  *mMsg=*msg;
  // locked=FALSE;
  // printf("**** KEInput: emitting signalReceiveInput 474\n");
  emit signalReceiveInput(msg,number);
//  emit signalReceiveInput(mMsg);
//  delete mMsg;
  return TRUE;
}

void KEInput::Lock()
{
  locked=true;
}

void KEInput::Unlock()
{
  locked=false;
}

bool KEInput::IsLocked()
{
  return locked;
}

