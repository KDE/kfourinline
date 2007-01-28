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

// KDE includes
#include <kdebug.h>

// Local includes
#include "score.h"
#include "scoresprite.h"


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
    mDisplay->setWon(QString("%1").arg(mWin[i]), i);
    mDisplay->setDraw(QString("%1").arg(mRemis[i]), i);
    mDisplay->setLoss(QString("%1").arg(mLoss[i]), i);
    mDisplay->setBreak(QString("%1").arg(mBrk[i]), i);
  }
}

#include "score.moc"