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
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QWidget>


class QPainter;
class QGraphicsItem;
class QWidget;


/**
 * A scene that reflects what is in it, to look pretty.
 */
class ReflectionGraphicsScene : public QGraphicsScene
{
  Q_OBJECT

  public:
    /** Construct a new scene.
      * @param updateTime The update interval of the canvas (kind of debug parameter)
      * @param parent     The parent window.
      */
    explicit ReflectionGraphicsScene(int updateTime, QObject * parent = nullptr);
    
    /** Destruct the scene.
      */
    ~ReflectionGraphicsScene() override;

    /** Standard QGV command to draw all items of a scene.
      * @param painter    The painter
      * @param numItems   The amount of items
      * @param items      The items to draw
      * @param options    The draw options
      * @param  widget    The widget
      */
    void drawItems(QPainter *painter, 
                           int numItems,
		           QGraphicsItem *items[],
		           const QStyleOptionGraphicsItem options[],
               QWidget *widget=nullptr) override;

    /** Should the background be drawn or not.
      * @ param status True to draw the background.
      */
    void setBackground(bool status) {mBackground = status;}


    protected:
    /** QGV background drawing.
      * @param painter The painter
      * @param rect    The clipping rect
      */
    void drawBackground ( QPainter * painter, const QRectF & rect ) override;

  private:
    // Draw background?
    bool mBackground;

};

#endif
