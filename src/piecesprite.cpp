/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "piecesprite.h"

// own
#include "kfourinline_debug.h"
// Std
#include <cmath>

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

