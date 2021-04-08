/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2007 John Tapsel <tapsell@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REFLECTION_GRAPHICS_SCENE_H
#define REFLECTION_GRAPHICS_SCENE_H

// Qt
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QWidget>


class QPainter;
class QGraphicsItem;


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
