/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "reflectiongraphicsscene.h"

// own
#include "kwin4global.h"
#include "kfourinline_debug.h"
// Qt
#include <QPainter>
#include <QRectF>
#include <QGraphicsItem>
#include <QElapsedTimer>

// How many time measurements for average
#define MEASUREMENT_LIST_SIZE  50
// How many warnings until reflections are switched off
#define WARNING_MAX_COUNT      10


// Construct a new scene
ReflectionGraphicsScene::ReflectionGraphicsScene(int updateTime, QObject * parent) 
                       : QGraphicsScene(parent)
{
  Q_UNUSED(updateTime)
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
  QElapsedTimer time;
  time.start();
  
  // No reflections call parent function
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

   // Disable reflections on slow computers
   if (mDrawTimes.size() >= MEASUREMENT_LIST_SIZE )
   {
     if (avg > 2*mUpdateTime )
     {
       mUpdateWarning++;
       mDrawTimes.clear();
       qCDebug(KFOURINLINE_LOG) << mUpdateWarning << ". slow computer reflection theme warning"; 
     }
     else
     {
       mUpdateWarning = 0;
     }
   }
   if (mUpdateWarning >= WARNING_MAX_COUNT )
   {
     update();
     qCDebug(KFOURINLINE_LOG) << "DISABLING REFLECTIONS DUE TO POOR COMPUTER PERFORMANCE";
   }
  // ==========================================================================
  */
}

void ReflectionGraphicsScene::drawBackground ( QPainter * painter, const QRectF & rect )
{
  if (mBackground) QGraphicsScene::drawBackground(painter, rect);
}



