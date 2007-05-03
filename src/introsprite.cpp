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

#include "introsprite.h"
#include "introsprite.h"

// General includes
#include <math.h>

// Qt includes

// KDE includes
#include <kdebug.h>
#include <kconfig.h>

// Constructor for the sprite
IntroSprite::IntroSprite(const QString &id, ThemeManager* theme, int advancePeriod, int no, QGraphicsScene* canvas)
    :  Themable(id, theme), PixmapSprite(advancePeriod, no, canvas)
{
  hide();

  mAdvancePeriod = advancePeriod;
  mNo = no;
  mAnimationState = Idle;

  if (theme) theme->updateTheme(this);
}


// Destructor
IntroSprite::~IntroSprite()
{
}


// Change the theme
void IntroSprite::changeTheme()
{
  PixmapSprite::changeTheme();
}


// Start the intro movement: linear, circle, linear
void IntroSprite::startIntro(QPointF start, QPointF end, double radius, double duration, double delay)
{
  mStart          = start;
  mEnd            = end;
  mDuration       = duration;
  mRadius         = radius;
  mDelay = delay;
  mAnimationState = IntroDelay;
  mTime           = 0;
  setPos(mStart.x()*getScale(), mStart.y()*getScale());
  show();
}


// Start linear movement
void IntroSprite::startLinear(QPointF start, QPointF end, double duration)
{
  mStart          = start;
  mEnd            = end;
  mDuration       = duration;
  mAnimationState = LinearMove;
  mTime           = 0;
  setPos(mStart.x()*getScale(), mStart.y()*getScale());
  show();
}


// Start linear movement from current position
void IntroSprite::startLinear(QPointF end, double duration)
{
  mStart          = QPointF(x()/getScale(), y()/getScale());
  mEnd            = end;
  mDuration       = duration;
  mAnimationState = LinearMove;
  mTime           = 0;
  show();
}


// CanvasItem advance method 
void IntroSprite::advance(int phase)
{
  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    QGraphicsItem::advance(phase);
    return ;
  }

  double scale = this->getScale();

  // Increase time
  mTime += mAdvancePeriod;

  // Delay animation
  if (mAnimationState == IntroDelay)
  {
     if (mTime >= mDelay)
     {
       mAnimationState = IntroLinear1;
       mTime = 0.0;
     }
  }

  // Handle first linear phase
  if (mAnimationState == IntroLinear1)
  {
     if (mTime >= 0.25*mDuration)
     {
       mAnimationState = IntroCircle;
     }
     else
     {
       double t = 2.0*mTime/mDuration;
       qreal x = mStart.x() + t*(mEnd.x()-mStart.x());
       qreal y = mStart.y() + t*(mEnd.y()-mStart.y());
       setPos(x*scale, y*scale);
     }
  }

  // Handle circle phase
  if (mAnimationState == IntroCircle)
  {
     if (mTime >= 0.75*mDuration)
     {
       mAnimationState = IntroLinear2;
     }
     else
     {
       double t = 2.0*(mTime/mDuration-0.25);
       double sign = 1.0;
       if (mStart.x() > 0.5) sign = -1.0; // Direction of turn
       qreal cx = mStart.x() + 0.5*(mEnd.x()-mStart.x());
       qreal cy = mStart.y() + 0.5*(mEnd.y()-mStart.y());
       qreal x = cx + mRadius*sin(sign*t*2.0*M_PI);
       qreal y = cy-mRadius + mRadius*cos(sign*t*2.0*M_PI);
       setPos(x*scale, y*scale);
     }
  }

  // Handle second linear phase
  if (mAnimationState == IntroLinear2)
  {
     if (mTime >= 1.00*mDuration)
     {
       mAnimationState = Idle;
       setPos(mEnd.x()*scale, mEnd.y()*scale);
     }
     else
     {
       double t = 2.0*(mTime/mDuration-0.75)+0.5;
       qreal x = mStart.x() + t*(mEnd.x()-mStart.x());
       qreal y = mStart.y() + t*(mEnd.y()-mStart.y());
       setPos(x*scale, y*scale);
     }
  }

  // Handle linear phase
  if (mAnimationState == LinearMove)
  {
     if (mTime >= mDuration)
     {
       mAnimationState = Idle;
       setPos(mEnd.x()*scale, mEnd.y()*scale);
     }
     else
     {
       double t = mTime/mDuration;
       qreal x = mStart.x() + t*(mEnd.x()-mStart.x());
       qreal y = mStart.y() + t*(mEnd.y()-mStart.y());
       setPos(x*scale, y*scale);
     }
  }
    
  QGraphicsItem::advance(phase);
}

