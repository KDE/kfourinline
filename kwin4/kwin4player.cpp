/***************************************************************************
                            KWin4Player
                            -------------------
    begin                :  August 2001
    copyright            : (C) |1995-2001 by Martin Heni
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


// include files for KDE
#include <kdebug.h>
#include <kconfig.h>
#include <kgamepropertyhandler.h>

#include "kwin4player.h"
#include "statuswidget.h"

Kwin4Player::Kwin4Player() : KPlayer(), sWidget(0)
{
  int id;
  id=mWin.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mWin"));
  id=mRemis.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mRemis"));
  id=mLost.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mLost"));
  id=mBrk.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mBrk"));
  id=mAllWin.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mAllWin"));
  id=mAllRemis.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mAllRemis"));
  id=mAllLost.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mAllLost"));
  id=mAllBrk.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mAllBrk"));

  dataHandler()->setPolicy(KGamePropertyBase::PolicyDirty,false);

  resetStats();
  connect(this,SIGNAL(signalPropertyChanged(KGamePropertyBase *,KPlayer *)),
          this,SLOT(slotPlayerPropertyChanged(KGamePropertyBase *,KPlayer *)));
}

#include <qlabel.h>
#include <qlcdnumber.h>

void Kwin4Player::slotPlayerPropertyChanged(KGamePropertyBase *prop, KPlayer * /*player*/)
{
  if (!sWidget) return ;
  if (!isActive()) return ;
  if (prop->id()==KGamePropertyBase::IdName)
  {
    if(userId())
      sWidget->p1_name->setText(name());
    else
      sWidget->p2_name->setText(name());
  }
  else if (prop->id()==mWin.id())
  {
    if(userId()){
      sWidget->p1_w->display(mWin);
      sWidget->p1_n->display(mWin+mRemis+mLost);
    }
    else{
      sWidget->p2_w->display(mWin);
      sWidget->p2_n->display(mWin+mRemis+mLost);
    }  
  }
  else if (prop->id()==mRemis.id())
  {
    if(userId()){
      sWidget->p1_d->display(mRemis);
      sWidget->p1_n->display(mWin+mRemis+mLost);
    }
    else{
      sWidget->p2_d->display(mRemis);
      sWidget->p2_n->display(mWin+mRemis+mLost);
    }  
  }
  else if (prop->id()==mLost.id())
  {
    if(userId()){
      sWidget->p1_l->display(mLost);
      sWidget->p1_n->display(mWin+mRemis+mLost);
    }
    else{
      sWidget->p2_l->display(mLost);
      sWidget->p2_n->display(mWin+mRemis+mLost);
    }
  }
  else if (prop->id()==mBrk.id())
  {
    if(userId())
      sWidget->p1_b->display(mBrk);
    else
      sWidget->p2_b->display(mBrk);
  }
}

void Kwin4Player::readConfig(KConfig *config)
{
  mAllWin.setValue(config->readNumEntry("win",0));
  mAllRemis.setValue(config->readNumEntry("remis",0));
  mAllLost.setValue(config->readNumEntry("lost",0));
  mAllBrk.setValue(config->readNumEntry("brk",0));
}

void Kwin4Player::writeConfig(KConfig *config)
{
  config->writeEntry("win",mAllWin.value());
  config->writeEntry("remis",mAllRemis.value());
  config->writeEntry("lost",mAllLost.value());
  config->writeEntry("brk",mAllBrk.value());
}

void Kwin4Player::incWin()
{
  mWin.setValue(mWin.value()+1);
  mAllWin.setValue(mAllWin.value()+1);
}

void Kwin4Player::incLost()
{
  mLost.setValue(mLost.value()+1);
  mAllLost.setValue(mAllLost.value()+1);
}

void Kwin4Player::incRemis()
{
  mRemis.setValue(mRemis.value()+1);
  mAllRemis.setValue(mAllRemis.value()+1);
}

void Kwin4Player::incBrk()
{
  mBrk.setValue(mBrk.value()+1);
  mAllBrk.setValue(mAllBrk.value()+1);
}

void Kwin4Player::resetStats(bool all)
{
  mWin=0;
  mLost=0;
  mBrk=0;
  mRemis=0;
  if (all)
  {
    mAllWin=0;
    mAllLost=0;
    mAllBrk=0;
    mAllRemis=0;
  }
}

#include "kwin4player.moc"



