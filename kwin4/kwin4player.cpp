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
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kgamepropertyhandler.h>

#include "kwin4player.h"
#include "statuswidget.h"


Kwin4Player::~Kwin4Player()
{
  kdDebug() << "<<<<<<<<<<<<<<<<<<<DESTRUCT kwin4player>>>>>>>>>>>>>>>>>>>>>" << endl;
}
Kwin4Player::Kwin4Player() : KPlayer()
{
  kdDebug() << "<<<<<<<<<<<<<<<<<<<Construct kwin4player>>>>>>>>>>>>>>>>>>>>>" << endl;
  sWidget=0;

  int id;
  id=mWin.registerData(dataHandler());
  id=mRemis.registerData(dataHandler());
  id=mLost.registerData(dataHandler());
  id=mBrk.registerData(dataHandler());
  id=mAllWin.registerData(dataHandler());
  id=mAllRemis.registerData(dataHandler());
  id=mAllLost.registerData(dataHandler());
  id=mAllBrk.registerData(dataHandler());

  dataHandler()->setPolicy(KGamePropertyBase::PolicyDirty,false);

  resetStats();
  connect(this,SIGNAL(signalPropertyChanged(KGamePropertyBase *,KPlayer *)),
          this,SLOT(slotPlayerPropertyChanged(KGamePropertyBase *,KPlayer *)));


}

void Kwin4Player::slotPlayerPropertyChanged(KGamePropertyBase *prop,KPlayer *player)
{
  if (!sWidget) return ;
  if (!isActive()) return ;
  if (prop->id()==KGamePropertyBase::IdName)
  {
    sWidget->setPlayer(name(),userId());
  }
  else if (prop->id()==mWin.id())
  {
    sWidget->setWin(mWin,userId());
    sWidget->setSum(mWin+mRemis+mLost,userId());
  }
  else if (prop->id()==mRemis.id())
  {
    sWidget->setRemis(mRemis,userId());
    sWidget->setSum(mWin+mRemis+mLost,userId());
  }
  else if (prop->id()==mLost.id())
  {
    sWidget->setLost(mLost,userId());
    sWidget->setSum(mWin+mRemis+mLost,userId());
  }
  else if (prop->id()==mBrk.id())
  {
    sWidget->setBrk(mBrk,userId());
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



