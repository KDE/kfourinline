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
#include "pixmapsprite.h"

// General includes
#include <math.h>

// Qt includes

// KDE includes
#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>

// Constructor for the sprite
PixmapSprite::PixmapSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* canvas)
    :  Themeable(id, theme), QGraphicsPixmapItem(0, canvas)
{
  hide();
  setAcceptsHoverEvents(false);

  mAnimationState = Idle;
  mNo             = no;
  mCurrentFrame   = 0;
  mOffsetStatus   = true;

  if (theme) theme->updateTheme(this);
}


// Constructor for the sprite
PixmapSprite::PixmapSprite(int no, QGraphicsScene* canvas)
    :  Themeable(), QGraphicsPixmapItem(0, canvas)
{
  hide();

  mAnimationState = Idle;
  mNo             = no;
  mCurrentFrame   = 0;
  mOffsetStatus   = true;
}


// Main themeable function. Called for any theme change. The sprites needs to
// resiez and redraw here.
void PixmapSprite::changeTheme()
{
  // Clear data
  mFrames.clear();
  mHotspots.clear();
  
  // Get scaling change
  double oldscale = this->getScale();
  double scale = thememanager()->getScale();
  Themeable::setScale(scale);
 
  // Retrieve theme data from configuration
  KConfigGroup config = thememanager()->config(id());
  double width  = config.readEntry("width", 1.0);
  double height = config.readEntry("height", 0.0);
  width *= scale;
  height *= scale;
  QPointF pos = config.readEntry("pos", QPointF(1.0,1.0));
  pos *= scale;
  // Set fixed z value?
  if (config.hasKey("zValue"))
  {
    double zValue = config.readEntry("zValue", 0.0);
    setZValue(zValue);
  }

  // Centering
  bool center = config.readEntry("center", false);

  // Animation
  mStartFrame      = config.readEntry("start-frame", 0);
  mEndFrame        = config.readEntry("end-frame", 0);
  mDelay           = config.readEntry("animation-delay", 0);
  QString refframe = config.readEntry("ref-frame", QString());

  // Set fixed position or modify current position
  if (config.hasKey("pos"))
  {
    setPos(pos.x(), pos.y());
  }
  else
  {
    setPos(x()*scale/oldscale, y()*scale/oldscale);
  }

  // SVG graphics
  QString svgid = config.readEntry("svgid");
  // Read sequence of frame pixmaps when auto ID given
  QPixmap pixmap;
  if (svgid == "auto")
  {
    for (int i=mStartFrame;i<=mEndFrame;i++)
    {
      QString name = QString("frame%1").arg(i);
      svgid = config.readEntry(name);
      if (!refframe.isNull())
      {
        pixmap = thememanager()->getPixmap(svgid, refframe, width);
      }
      else if (config.hasKey("height"))
      {
        pixmap = thememanager()->getPixmap(svgid, QSize(int(width), int(height)));
      }
      else
      {
        pixmap = thememanager()->getPixmap(svgid, width);
      }
      mFrames.append(pixmap);
      if (center) mHotspots.append(QPointF(pixmap.width()/2,pixmap.height()/2));
      else mHotspots.append(QPointF(0.0,0.0));
    }
  }
  // Read only one named pixmap
  else
  {
    if (config.hasKey("height"))
    {
      pixmap = thememanager()->getPixmap(svgid, QSize(int(width), int(height)));
    }
    else
    {
      pixmap = thememanager()->getPixmap(svgid, width);
    }
    mFrames.append(pixmap);
    if (center) mHotspots.append(QPointF(pixmap.width()/2,pixmap.height()/2));
    else mHotspots.append(QPointF(0.0,0.0));
  }
  
  // Set theme offset (probably not really necesary here)
  QPoint offset = thememanager()->getOffset();
  resetTransform();
  if (mOffsetStatus) translate(offset.x(), offset.y());

  // Set pixmap to sprite
  setFrame(mCurrentFrame, true);
  update();
}


// Debug only: Retrieve double value from configuration file
double PixmapSprite::getDoubleValue(const QString &item)
{
  KConfigGroup config = thememanager()->config(id());
  return config.readEntry(item, 0.0);
}


// Move the sprite to the given relative position
void PixmapSprite::setPosition(QPointF pos)
{
  mStart          = pos;
  setPos(mStart.x()*getScale(), mStart.y()*getScale());
}


// Handle the offset status (true: theme offset, false: no offset)
void PixmapSprite::setOffsetStatus(bool status)
{
  mOffsetStatus = status;
  changeTheme();
}


// Store the logical board coordinates for theme changes
void PixmapSprite::setLogicalPos(QPoint pos)
{
  mLPos = pos;
}


// Retrieve the logical board coordinates for theme changes
QPoint PixmapSprite::logicalPos()
{
  return mLPos;
}


// Start or stop a frame animation
void PixmapSprite::setAnimation(bool status)
{
  if (status) mAnimationState = Animated;
  else mAnimationState = Idle;
  mTime.start();
  setFrame(mStartFrame);
}


// Specify and start a frame animation
void PixmapSprite::setAnimation(int start, int end, int delay)
{
  mDelay          = delay;
  mStartFrame     = start;
  mEndFrame       = end;
  setAnimation(true);
}


// Set a new bitmap into the sprite. If the number is the same as the
// current one, nothing is done unless forcing is set to true.
void PixmapSprite::setFrame(int no, bool force)
{
  if (!force && no == mCurrentFrame) return;
  if (no<0 || no >=mFrames.count()) return;
  setPixmap(mFrames.at(no));
  
  QPoint offset = thememanager()->getOffset();
  
  // Set new item's scene transformation: Hotspot plus gloval theme offset
  resetTransform();
  if (mOffsetStatus)
  {
    translate(-mHotspots.at(no).x()+offset.x(), -mHotspots.at(no).y()+offset.y());
  }
  else 
  {
    translate(-mHotspots.at(no).x(), -mHotspots.at(no).y());
  }
  mCurrentFrame = no;
  update();
}


// Standard QGI advance method 
void PixmapSprite::advance(int phase)
{
  int elapsed = mTime.elapsed();

  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    QGraphicsItem::advance(phase);
    return ;
  }

  // Handle animation
  if (mAnimationState == Animated)
  {
    // Frame delay passed?
    int frames = elapsed / mDelay;
    int curFrame = frames % (mEndFrame-mStartFrame)+ mStartFrame;
    setFrame(curFrame);
  }

  QGraphicsItem::advance(phase);
}

