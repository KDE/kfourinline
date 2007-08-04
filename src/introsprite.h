#ifndef INTRO_SPRITE_H
#define INTRO_SPRITE_H
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

// Local includes
#include "thememanager.h"
#include "pixmapsprite.h"


/** The sprite to display the introduction pieces and their animation.
 */
class IntroSprite : public PixmapSprite
{

  public:
    /** Constructor for the sprite.
      * @param id            The theme id
      * @param theme         The theme manager
      * @param no            The sprite number [0..41]
      * @param scene        The scene
      */
    IntroSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* scene);

    /** Destructor 
    */
    ~IntroSprite();

    /** Start an intro move. 
     *  The first half of the move is is on a straight line from start to end. The second
     *  part of the move is a circle with the given radius and the final part of the move
     *  is again a straight line to the end location. The sprite movement is started delayed
     *  with the given delay factor.
     *  @param start    The start point [rel. coord]
     *  @param end      The end point [rel. coord]
     *  @param radius   The radius of the circle [rel. coord]
     *  @param duration The duration of the move [ms]
     *  @param delay    The initial movement delay [ms]
     */
    void startIntro(QPointF start, QPointF end, double radius, double duration, double delay);

    /** Start a linear movement from start to end with the given duration.
     *  @param start    The start point [rel. coord]
     *  @param end      The end point [rel. coord]
     *  @param duration The duration of the move [ms]
     */
    void startLinear(QPointF start, QPointF end, double duration);

    /** Start a linear movement from the current position to end with the given duration.
     *  @param end      The end point [rel. coord]
     *  @param duration The duration of the move [ms]
     */
    void startLinear(QPointF end, double duration);

    /** Start a linear manhatte movement (edge following) from the current position to end with the given duration.
     *  @param end      The end point [rel. coord]
     *  @param velocity The velocity of the move [rel]
     *  @param delay    Delay start of movement by this [ms]
     */
    void startManhatten(QPointF end, double velocity, double delay);

    // Possible animation states of the sprite
    enum MovementState {Idle, IntroDelay, IntroLinear1, IntroCircle, IntroLinear2, LinearMove, ManhattenDelay, ManhattenMove};

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+1
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+1;}

    /** Retrieve the sprite numbers (which introsprite e.g. 0-42)
     *  @return The sprite numbers.
     */
    int number() {return mNo;}

    /** Main theme notification method. Is called for any theme changes. It must be
      * implemented so that the item redraws correctly when the theme changed.
      */
    virtual void changeTheme();

    /** Retrieve the duration of an animation.
      * @return The duration [ms]
      */
    double duration() {return mDuration;}

  private:

    /// The duration of the movement animation 
    double mDuration;

    /// The current running time for the animation.
    QTime mTime;

    // The start delay before movement starts
    double mDelay;
    // The velocity [rel]
    double mVelocity;

    // The start points of the movement [rel]
    QPointF mStart;
    // The end points of the movement [rel]
    QPointF mEnd;
    // The radius of the circular movement [rel]
    double mRadius;

    // The state of the animation
    MovementState mAnimationState;

};

#endif
