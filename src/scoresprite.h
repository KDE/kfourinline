#ifndef SCORE_SPRITE_H
#define SCORE_SPRITE_H
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
#include <QGraphicsTextItem>

#include "thememanager.h"
#include "pixmapsprite.h"


/**
 * The sprite for a score board on the canvas.
 */
class ScoreSprite : public PixmapSprite
{

  public:
    /** Constructor for the sprite.
     */
    ScoreSprite(QString id, ThemeManager* theme, int advancePeriod, int no, QGraphicsScene* canvas);
    ~ScoreSprite();

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+2
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+10;}

    virtual void changeTheme();

    void setLevel(int i);
    void setPlayerName(QString s,int no);
    void setWon(QString s,int no);
    void setDraw(QString s,int no);
    void setLoss(QString s,int no);
    void setBreak(QString s,int no);
    void setTurn(int i);
    void setFrame(int i);

  private:
    QGraphicsTextItem* mWon[2];
    QGraphicsTextItem* mDraw[2];
    QGraphicsTextItem* mLoss[2];
    QGraphicsTextItem* mBreak[2];
    QGraphicsTextItem* mName[2];
    int mTurn;
    int mFrame;
};

#endif
