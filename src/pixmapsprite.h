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
#include <QGraphicsPixmapItem>
#include <QTime>

// Local includes
#include "thememanager.h"


/** This sprite is used to display a pixmap on the canvas.
 *  It can be animated and it is themeable.
 */
class PixmapSprite : public QGraphicsPixmapItem, public virtual Themeable
{

  public:
    /** Constructor for the sprite.
     * @param no            A user defined ID number
     * @param scene        The graphics scene   
     */
    PixmapSprite(int no, QGraphicsScene* scene);
    
    /** Constructor for the sprite.
     * @param id            The theme file ID string
     * @param theme         The theme manager 
     * @param no            A user defined ID number
     * @param scene        The graphics scene   
     */
    PixmapSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* scene);
    
    /** Possible animation states of the sprite
     */
    enum AnimationState {Idle, Animated};

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+3
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+3;}

    /** Retrieve the user defined sprite number (i.e. which PixmapSprite)
     *  @return The sprite numbers.
     */
    int number() {return mNo;}

    /** Main theme manager function. Called when any theme change like
      * a new theme or a theme size change occurs. This object needs to
      * resiez and redraw then.
      */
    virtual void changeTheme();

    /** Choose a pixmap frame of this sprite. If the setting is forced a
      * redraw is even performed when the frame number stays the same. This
      * is important for theme changes.
      * @param no    The new frame number (0..)
      * @param force Force redraw
      */
    void setFrame(int no, bool force=false);
    
    /** Initialize and start a frame animation between the start and end frame.
      * The delay between the frames is given in [ms]. After the last frame
      * is displayed the animation starts with the first frame.
      * @param start   The start frame number
      * @param end     The end frame number
      * @param delay   Delay between a frame change [ms]
      */
    void setAnimation(int start, int end, int delay);
    
    /** Start or stop an animation. 
      * @param status True to start and false to stop the animation
      */
    void setAnimation(bool status);
    
    /** Set/Move the sprite to the given position. The position is
      * given in relative coordinates [0..1, 0..1] to its parent object.
      * Thus the position is scaled with the scale from the theme manager.
      * @param pos  The position of the sprite [0..1, 0..1]
      */
    void setPosition(QPointF pos);
    
   /** Store the logical  coordinates (e.g 0-6, 0-5) for theme changes. Need to
     *  manually be called after all setPosition or set Linear calls.
     *  @param pos The logical position
     */
    void setLogicalPos(QPoint pos);
    
    /** Retrieve the logical  coordinates.
     *  @return The logical position.
     */
    QPoint logicalPos();

    /** Reads a theme configuration item of type double and of the given 
      * name.
      * @deprecated Thsi is debug only
      * @param item  The theme configuration item name
      * @return The read double value.
      */
    double getDoubleValue(const QString &item);

    /** Set whether the sprite should respect a theme offset 
      * (default: true) or not (false), that is, it is handled
      * by its parent item.
      * @param status True: Handle theme offset, False: do not
      */
    void setOffsetStatus(bool status);

  protected:

    /** The user defined sprite number.
     */
    int mNo;

    /** The state of the animation.
     */
    AnimationState mAnimationState;

    /** The position of the sprite [rel 0..1, rel 0..1]
     */
    QPointF mStart;

    /** The first frame in the frame animation.
     */
    int mStartFrame;
    
    /** The last frame in the frame animation.
     */
    int mEndFrame;
    
    /** The delay between two frames in the frame animation.
     */
    int mDelay;
    
    /** The current running time for the animation.
     */
    QTime mTime;

    /** The current running frame number for the animation.
     */
    int mCurrentFrame;
    
    /** Storage of the frame pixmaps.
     */
    QList<QPixmap> mFrames;
    
    /** Storage of the frame pixmap offset points.
     */
    QList<QPointF> mHotspots;
    
    /** The logical sprite pos for theme changs
     */
    QPoint mLPos;

    /** Offset status.
     */
    bool mOffsetStatus;
};

#endif
