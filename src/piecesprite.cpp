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
#include "piecesprite.h"

// General includes
#include <math.h>

// Qt includes

// KDE includes
#include <kdebug.h>

PieceSprite::PieceSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* canvas)
    :  Themeable(id, theme), PixmapSprite(no, canvas)
{
  mMovementState = Idle;
  mNotify = new SpriteNotify(this);
  if (theme) theme->updateTheme(this);
}


// Destructor
PieceSprite::~PieceSprite()
{
  delete mNotify;
}


// Standard theme change function to redraw the item
void PieceSprite::changeTheme()
{
  PixmapSprite::changeTheme();
}



// Start a linear movement
void PieceSprite::startLinear(QPointF start, QPointF end, double velocity)
{
  mStart          = start;
  mEnd            = end;
  QPointF p       = mEnd-mStart;
  double dist     = sqrt(p.x()*p.x()+p.y()*p.y());
  if (dist > 0.0) mDuration = dist/velocity*1000.0; // Duration in [ms]
  else mDuration = 0.0;
  
  mMovementState = LinearMove;
  mTime.restart();
  setPos(mStart.x()*getScale(), mStart.y()*getScale());
  show();
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
  mTime.restart();
  show();
}


// CanvasItem advance method 
void PieceSprite::advance(int phase)
{
  // Advance time and frame animation etc
  PixmapSprite::advance(phase);
  
  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    return ;
  }

  // Current scaling
  double scale = this->getScale();

  // Handle linear movement
  if (mMovementState == LinearMove)
  {
  	 // Movement over?
     if (mTime.elapsed() >= mDuration)
     {
       mMovementState = Idle;

       setPos(mEnd.x()*scale, mEnd.y()*scale);
       // Use notifier to emit signal
       mNotify->emitSignal(0);
     }
     else
     {
     	 // Continue moving
       double t = mTime.elapsed()/mDuration;
       qreal x = mStart.x() + t*(mEnd.x()-mStart.x());
       qreal y = mStart.y() + t*(mEnd.y()-mStart.y());
       setPos(x*scale, y*scale);
     }
  }
    
}

