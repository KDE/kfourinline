/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "score.h"

// own
#include "kfourinline_debug.h"
#include "scoresprite.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgameio.h>


// Construct a score object
Score::Score(QObject* parent)
     : QObject(parent)
{
  mDisplay = nullptr;
}


// Activate the score display sprite
void Score::setDisplay(ScoreSprite* s)
{
  mDisplay = s;
  update();
}


// Update the values in the sprite
void Score::update()
{
  if (!mDisplay) return;

  mDisplay->setTurn(mTurn);

  for (int i=0; i<2; i++)
  {
    mDisplay->setPlayerName(mName[i], i);
    // Call this only after set name
    if (mInputDevice[i] == (int)KGameIO::ProcessIO) mDisplay->setLevel(mLevel[i], i);
    else  mDisplay->setLevel(-1, i);
    mDisplay->setWon(QStringLiteral("%1").arg(mWin[i]), i);
    mDisplay->setDraw(QStringLiteral("%1").arg(mRemis[i]), i);
    mDisplay->setLoss(QStringLiteral("%1").arg(mLoss[i]), i);
    mDisplay->setBreak(QStringLiteral("%1").arg(mBrk[i]), i);
    mDisplay->setInput(mInputDevice[i], i);
  }
}


