#ifndef PIECE_SPRITE_H
#define PIECE_SPRITE_H
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

// Qt includes
#include <QPointF>
#include <QSizeF>
#include <QGraphicsPixmapItem>

#include "thememanager.h"
#include "pixmapsprite.h"
#include "spritenotify.h"


/**
 * The sprite for a pixmap piece on the canvas.
 */
class PieceSprite : public PixmapSprite
{

  public:
    /** Constructor for the sprite.
     */
    PieceSprite(QString id, ThemeManager* theme, int advancePeriod, int no, QGraphicsScene* canvas);
    ~PieceSprite();

    // Possible animation states of the sprite
    enum MovementState {Idle, LinearMove};

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+2
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+2;}


    virtual void changeTheme();

    SpriteNotify* notify() {return mNotify;}

    void startLinear(QPointF end, double velocity);
    void startLinear(QPointF start, QPointF end, double velocity);

  private:

    // The duration of the movement 
    double mDuration;

    // The state of the movement
    MovementState mMovementState;

    // The end points of the movement [rel]
    QPointF mEnd;

    // The notifier
    SpriteNotify* mNotify;

};

#endif
