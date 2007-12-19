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
#include "reflectiongraphicsscene.h"
#include "kwin4global.h"

#include <kdebug.h>

#include <QPainter>
#include <QRectF>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <klocale.h>
#include <QTime>

// How many time measurements for average
#define MEASUREMENT_LIST_SIZE  50
// How many warnings until reflections are switched off
#define WARNING_MAX_COUNT      10


// Construct a new scene
ReflectionGraphicsScene::ReflectionGraphicsScene(int updateTime, QObject * parent) 
                       : QGraphicsScene(parent)
{
  // Initialize
  mBackground = true;
}


// Destruct scene
ReflectionGraphicsScene::~ReflectionGraphicsScene()
{
}


// QGV basic function to draw all items of a scene
void ReflectionGraphicsScene::drawItems(QPainter *painter, int numItems,
                                        QGraphicsItem *items[],
                                        const QStyleOptionGraphicsItem options[],
                                        QWidget *widget)
{
  QTime time;
  time.start();
  
  // No relfections call parent function
  QGraphicsScene::drawItems(painter, numItems, items, options, widget);

  /*
  // ==========================================================================
  // Update time measurement and display
  int elapsed = time.elapsed();
  mDrawTimes.append(elapsed);
  if (mDrawTimes.size() > MEASUREMENT_LIST_SIZE) mDrawTimes.removeFirst();
  double avg = 0.0;
  for (int i=0; i<mDrawTimes.size(); i++) avg += mDrawTimes[i];
  avg /= mDrawTimes.size();


  if (global_debug > 0)
     mFrameSprite->setPlainText(QString("Draw: %1 ms  Average %2 ms  Update: %3 ms").arg(elapsed).arg(int(avg)).arg(mDisplayUpdateTime));

   // Disable relfections on slow computers
   if (mDrawTimes.size() >= MEASUREMENT_LIST_SIZE )
   {
     if (avg > 2*mUpdateTime )
     {
       mUpdateWarning++;
       mDrawTimes.clear();
       kDebug() << mUpdateWarning << ". slow computer reflection theme warning"; 
     }
     else
     {
       mUpdateWarning = 0;
     }
   }
   if (mUpdateWarning >= WARNING_MAX_COUNT )
   {
     update();
     kDebug() << "DISABLING REFLECTIONS DUE TO POOR COMPUTER PERFORMANCE";
   }
  // ==========================================================================
  */
}

void ReflectionGraphicsScene::drawBackground ( QPainter * painter, const QRectF & rect )
{
  if (mBackground) QGraphicsScene::drawBackground(painter, rect);
}

#include "reflectiongraphicsscene.moc"

