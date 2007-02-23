/***************************************************************************
                            KWin4Player
                            -------------------
    begin                :  August 2001
    copyright            : (C) |1995-2007 by Martin Heni
    email                : kde@heni-online.de
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
#include <kgamepropertyhandler.h>

#include "kwin4player.h"

Kwin4Player::Kwin4Player() : KPlayer()
{
  int id;
  mStatus = 0;
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

  mWin.setValue(0);
  mLost.setValue(0);
  mBrk.setValue(0);
  mRemis.setValue(0);
}

#include <QLabel>
#include <qlcdnumber.h>

void Kwin4Player::slotPlayerPropertyChanged(KGamePropertyBase *prop, KPlayer * /*player*/)
{
  if (!mStatus) return ;
  if (!isActive()) return ;
  if (prop->id()==KGamePropertyBase::IdName)
  {
    mStatus->setPlayerName(name(), userId());
  }
  else if (prop->id()==mAllWin.id())
  {
    mStatus->setWins(mAllWin, userId());
    mStatus->setSum(mAllWin+mAllRemis+mAllLost, userId());
  }
  else if (prop->id()==mAllRemis.id())
  {
    mStatus->setRemis(mAllRemis, userId());
    mStatus->setSum(mAllWin+mAllRemis+mAllLost, userId());
  }
  else if (prop->id()==mAllLost.id())
  {
    mStatus->setLosses(mAllLost, userId());
    mStatus->setSum(mAllWin+mAllRemis+mAllLost, userId());
  }
  else if (prop->id()==mAllBrk.id())
  {
    mStatus->setBreaks(mAllBrk, userId());
  }
}

void Kwin4Player::readConfig(KConfigGroup& config)
{
  mAllWin.setValue(config.readEntry("win",0));
  mAllRemis.setValue(config.readEntry("remis",0));
  mAllLost.setValue(config.readEntry("lost",0));
  mAllBrk.setValue(config.readEntry("brk",0));
}

void Kwin4Player::writeConfig(KConfigGroup& config)
{
  config.writeEntry("win",mAllWin.value());
  config.writeEntry("remis",mAllRemis.value());
  config.writeEntry("lost",mAllLost.value());
  config.writeEntry("brk",mAllBrk.value());
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



