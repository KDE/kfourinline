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

// General includes
#include <math.h>

// Qt includes
#include <QSizeF>

// KDE includes
#include <kdebug.h>
#include <kconfig.h>

// Local includes
#include "piecesprite.h"

// Constructor for the view
PieceSprite::PieceSprite(QString id, ThemeManager* theme, int advancePeriod, int no, QGraphicsScene* canvas)
    :  Themable(id, theme), PixmapSprite(id, theme, advancePeriod, no, canvas)
{
  mNotify = new SpriteNotify(this);
}

PieceSprite::~PieceSprite()
{
  delete mNotify;
}

void PieceSprite::changeTheme()
{
  PixmapSprite::changeTheme();
}

// Start linear movement
void PieceSprite::startLinear(QPointF start, QPointF end, double velocity)
{
  mStart          = start;
  mEnd            = end;
  QPointF p       = mEnd-mStart;
  double dist     = sqrt(p.x()*p.x()+p.y()*p.y());
  if (dist > 0.0) mDuration = dist/velocity*1000.0; // Duration in [ms]

  else mDuration = 0.0;
  mMovementState = LinearMove;
  mTime           = 0;
  setPos(mStart.x()*getScale(), mStart.y()*getScale());
  show();
  kDebug() << "PieceSprite::startLinear from " << (start*getScale()) << " to " << (end*getScale()) << " time = " << mDuration << endl;
}


// Start linear movement from current position
void PieceSprite::startLinear(QPointF end, double velocity)
{
  mStart          = QPointF(x()/getScale(), y()/getScale());
  mEnd            = end;
  QPointF p       = mEnd-mStart;
  double dist     = sqrt(p.x()*p.x()+p.y()*p.y());
  if (dist > 0.0) mDuration = dist/velocity*1000.0; // Duration in [ms]
  else mDuration = 0.0;
  mMovementState = LinearMove;
  mTime           = 0;
  show();
}


// CanvasItem advance method 
void PieceSprite::advance(int phase)
{
  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    PixmapSprite::advance(phase);
    return ;
  }


  // Advance time and animation etc
  PixmapSprite::advance(phase);

  double scale = this->getScale();

  // Handle linear phase
  if (mMovementState == LinearMove)
  {
     if (mTime >= mDuration)
     {
       mMovementState = Idle;
       setPos(mEnd.x()*scale, mEnd.y()*scale);
       //kDebug() << "Emit signal for " << this << endl;
       mNotify->emitSignal(0);
     }
     else
     {
       double t = mTime/mDuration;
       qreal x = mStart.x() + t*(mEnd.x()-mStart.x());
       qreal y = mStart.y() + t*(mEnd.y()-mStart.y());
       setPos(x*scale, y*scale);
     }
  }
    
}
