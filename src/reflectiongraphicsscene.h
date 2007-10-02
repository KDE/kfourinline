#ifndef REFLECTION_GRAPHICS_SCENE_H
#define REFLECTION_GRAPHICS_SCENE_H
/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2007 John Tapsel <tapsell@kde.org>

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

// Qt includes
#include <QWidget>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QLinearGradient>
#include <QPixmap>
#include <QTransform>
#include <QGraphicsTextItem>
#include <QList>


class QPainter;
class QGraphicsItem;
class QWidget;


/**
 * A scene that reflects what is in it, to look pretty
 */
class ReflectionGraphicsScene : public QGraphicsScene
{
  Q_OBJECT

  public:
    ReflectionGraphicsScene(QObject * parent = 0);
    
    virtual ~ReflectionGraphicsScene();
    virtual void drawItems(QPainter *painter, int numItems,
		           QGraphicsItem *items[],
		           const QStyleOptionGraphicsItem options[],
			   QWidget *widget=0);

    void setReflection(int x, int y, int width, int height);

    void displayUpdateTime(int time);

  private:
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    QLinearGradient mGradient;
    QImage mGradientImage;
    QTransform mTransform;
    // Allow reflections scene?
    bool mAllowReflections;
    QGraphicsTextItem* mFrameSprite;
    int mDisplayUpdateTime;
    QList<int> mDrawTimes;

};

#endif
