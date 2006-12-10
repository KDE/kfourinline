#ifndef PIXMAP_SPRITE_H
#define PIXMAP_SPRITE_H
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


/**
 * The sprite for a pixmap Pixmap on the canvas.
 */
class PixmapSprite : public QGraphicsPixmapItem, public virtual Themable
{

  public:
    /** Constructor for the sprite.
     */
    PixmapSprite(QString id, ThemeManager* theme, int advancePeriod, int no, QGraphicsScene* canvas);
    // Possible animation states of the sprite

    enum AnimationState {Idle, Animated};

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+3
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+3;}

    /** Retrieve the sprite numbers (which PixmapSprite e.g. 0-42)
     *  @return The sprite numbers.
     */
    int number() {return mNo;}

    virtual void changeTheme();

    void setFrame(int no, bool force=false);
    void setAnimation(int start, int end, int delay);
    void setAnimation(bool status);
    void setPosition(QPointF pos);

  protected:

    // The sprite number
    int mNo;

    // The advance peridod, e.g. 25 ms [ms]
    int mAdvancePeriod;

    //  The theme id
    QString mId;
    // The state of the animation
    AnimationState mAnimationState;

    // The start points of the movement [rel]
    QPointF mStart;

    int mStartFrame;
    int mEndFrame;
    int mDelay;
    int mTime;

    // The current frame
    int mCurrentFrame;
    // Store our pixmap array and its offset values
    QList<QPixmap> mFrames;
    QList<QPointF> mHotspots;
};

#endif
