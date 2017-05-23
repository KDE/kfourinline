/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

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
#include "score.h"


#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgameio.h>

// Local includes
#include "kfourinline_debug.h"
#include "scoresprite.h"

// Construct a score object
Score::Score(QObject* parent)
     : QObject(parent)
{
  mDisplay = 0;
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


