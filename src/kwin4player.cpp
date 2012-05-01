/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2001 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Header includes
#include "kwin4player.h"

// KDE includes
#include <kdebug.h>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamepropertyhandler.h>


// Construct a player object
KWin4Player::KWin4Player() : KPlayer()
{
  // Register KGameProperties in KGame framework  
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

  // Clear all stats (they will be loaded from the config file later on)
  resetStats();
  connect(this,SIGNAL(signalPropertyChanged(KGamePropertyBase*,KPlayer*)),
          this,SLOT(slotPlayerPropertyChanged(KGamePropertyBase*,KPlayer*)));

  mWin.setValue(0);
  mLost.setValue(0);
  mBrk.setValue(0);
  mRemis.setValue(0);
}

// Destructor
KWin4Player::~KWin4Player()
{
  kDebug() << "~KPlayer()";
  kDebug() << "~KPlayer() done";
}


// A registered player property changed (KGame framework function). Store this property
// in the score object for the GUI.
void KWin4Player::slotPlayerPropertyChanged(KGamePropertyBase *prop, KPlayer * /*player*/)
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
  }
  else if (prop->id()==mAllRemis.id())
  {
    mStatus->setRemis(mAllRemis, userId());
  }
  else if (prop->id()==mAllLost.id())
  {
    mStatus->setLosses(mAllLost, userId());
  }
  else if (prop->id()==mAllBrk.id())
  {
    mStatus->setBreaks(mAllBrk, userId());
  }
}


// Read the player all time score from the config file
void KWin4Player::readConfig(KConfigGroup& config)
{
  mAllWin.setValue(config.readEntry("win",0));
  mAllRemis.setValue(config.readEntry("remis",0));
  mAllLost.setValue(config.readEntry("lost",0));
  mAllBrk.setValue(config.readEntry("brk",0));
}


// Write the player all time score to the config file
void KWin4Player::writeConfig(KConfigGroup& config)
{
  config.writeEntry("win",mAllWin.value());
  config.writeEntry("remis",mAllRemis.value());
  config.writeEntry("lost",mAllLost.value());
  config.writeEntry("brk",mAllBrk.value());
}


// Increase the number of wins
void KWin4Player::incWin()
{
  mWin.setValue(mWin.value()+1);
  mAllWin.setValue(mAllWin.value()+1);
}


// Increase the number of losses
void KWin4Player::incLost()
{
  mLost.setValue(mLost.value()+1);
  mAllLost.setValue(mAllLost.value()+1);
}


// Increase the number of draws
void KWin4Player::incRemis()
{
  mRemis.setValue(mRemis.value()+1);
  mAllRemis.setValue(mAllRemis.value()+1);
}


// Increase the number of aborted games
void KWin4Player::incBrk()
{
  mBrk.setValue(mBrk.value()+1);
  mAllBrk.setValue(mAllBrk.value()+1);
}


// Clear player status: For the argument true the long time
// statistics in the config file is cleared as well. For the
// argument false only the current session is cleared.
void KWin4Player::resetStats(bool all)
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



