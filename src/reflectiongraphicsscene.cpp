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

#include <QPainter>
#include <QRectF>
#include <QGraphicsItem>
#include <QTransform>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QLinearGradient>
#include <klocale.h>
#include <QTime>

ReflectionGraphicsScene::ReflectionGraphicsScene(QObject * parent) : QGraphicsScene(parent)
{
  mX = mY = mWidth = mHeight = 0;
  mAllowReflections = true;
  mDisplayUpdateTime = 0;
  mFrameSprite = new QGraphicsTextItem(0, this);
  mFrameSprite->setPos(QPointF(0.0, 0.0));
  mFrameSprite->setZValue(1000.0);
  if (global_debug > 0) mFrameSprite->show();
  else mFrameSprite->hide();
}

ReflectionGraphicsScene::~ReflectionGraphicsScene()
{
  delete mFrameSprite;
}

void ReflectionGraphicsScene::displayUpdateTime(int time)
{
  mDisplayUpdateTime = time;
}

void ReflectionGraphicsScene::setReflection(int x, int y, int width, int height)
{
  mX = x;
  mY = y;
  mWidth = width;
  mHeight = height*2;

  QPoint p1, p2;
  p2.setY(mHeight);
  mGradient = QLinearGradient(p1, p2);
  mGradient.setColorAt(0, QColor(0, 0, 0, 100));
  mGradient.setColorAt(1, Qt::transparent);

  kDebug() << "Set reflection "<< x << " " << y << " " << width << " " << height ;

  mGradientImage = QImage(mWidth, mHeight, QImage::Format_ARGB32);
  mGradientImage.fill(Qt::transparent);
  QPainter p( &mGradientImage );
  p.fillRect(0,0,mWidth, mHeight, mGradient);
  p.end();
}

void ReflectionGraphicsScene::drawItems(QPainter *painter, int numItems,
                                        QGraphicsItem *items[],
                                        const QStyleOptionGraphicsItem options[],
                                        QWidget *widget)
{
  QTime time;
  time.start();
  
  if (mAllowReflections)
  {
   if(mWidth == 0) 
   {  //No reflection
     for (int i = 0; i < numItems; ++i) 
     {
       QTransform sceneTransform = items[i]->sceneTransform();
       painter->setTransform(sceneTransform, false);
       items[i]->paint(painter, &options[i], widget);
     }
   } 
   else
   {  //With reflection
     QImage image(mWidth, mHeight, QImage::Format_ARGB32);
     image.fill(Qt::transparent);
     QPainter imagePainter(&image);
     //Turn on all optimizations
     imagePainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, false);
     imagePainter.setClipping(true);
     painter->save();
  
     for (int i = 0; i < numItems; ++i) 
     {
       QTransform sceneTransform = items[i]->sceneTransform();
       painter->setTransform(sceneTransform, false);
       items[i]->paint(painter, &options[i], widget);
       if(sceneTransform.dy() + items[i]->boundingRect().height() > mY - mHeight) 
       {
         sceneTransform = QTransform(sceneTransform.m11(), sceneTransform.m12(), sceneTransform.m21(), -sceneTransform.m22(), sceneTransform.dx() - mX, mY - sceneTransform.dy());
         imagePainter.setTransform(sceneTransform, false);
         items[i]->paint(&imagePainter, &options[i], widget);
       }
     }
     imagePainter.setTransform(QTransform());
     imagePainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
     imagePainter.drawImage(0,0,mGradientImage);
     painter->restore();
     painter->drawImage( mX,mY,image);
   }
  }
  else QGraphicsScene::drawItems(painter, numItems, items, options, widget);

  // Time display
  int elapsed = time.elapsed();
  mDrawTimes.append(elapsed);
  if (mDrawTimes.size() > 50) mDrawTimes.removeFirst();
  double avg = 0.0;
  for (int i=0; i<mDrawTimes.size(); i++) avg += mDrawTimes[i];
  avg /= mDrawTimes.size();


  if (global_debug > 0)
     mFrameSprite->setHtml(QString("<b>Draw: %1 ms  Average %2 ms  Update: %3 ms </b>").arg(elapsed).arg(int(avg)).arg(mDisplayUpdateTime));

   if (avg > 50.0 && mDrawTimes.size() == 50)
   {
     mAllowReflections = false;   
     kDebug() << "DISABLING REFLECTIONS DUE TO SLOW COMPUTER";
   }
}

#include "reflectiongraphicsscene.moc"

