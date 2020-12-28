/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2001 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kwin4player.h"

// own
#include "kfourinline_debug.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamepropertyhandler.h>


// Construct a player object
KWin4Player::KWin4Player() : KPlayer()
{
  // Register KGameProperties in KGame framework  
  mStatus = nullptr;
  mWin.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mWin"));
  mRemis.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mRemis"));
  mLost.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mLost"));
  mBrk.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mBrk"));
  mAllWin.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mAllWin"));
  mAllRemis.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mAllRemis"));
  mAllLost.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mAllLost"));
  mAllBrk.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QStringLiteral("mAllBrk"));

  dataHandler()->setPolicy(KGamePropertyBase::PolicyDirty,false);

  // Clear all stats (they will be loaded from the config file later on)
  resetStats();
  connect(this, &KWin4Player::signalPropertyChanged, this, &KWin4Player::slotPlayerPropertyChanged);

  mWin.setValue(0);
  mLost.setValue(0);
  mBrk.setValue(0);
  mRemis.setValue(0);
}

// Destructor
KWin4Player::~KWin4Player()
{
  qCDebug(KFOURINLINE_LOG) << "~KPlayer()";
  qCDebug(KFOURINLINE_LOG) << "~KPlayer() done";
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





