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
#include <QTime>
#include <QList>
#include <QGraphicsPixmapItem>

// Local includes
#include "thememanager.h"
#include "pixmapsprite.h"

class AnimationCommand;

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

    /** Clear the animation script.
      * @param restartTime Reset the timer too (default)
      */
    void clearAnimation(bool restartTime = true);

    /** Create a set position command. The sprite is directly set to the given
      * position.
      * @param start The position [rel coord.]
      * @return The command.
      */
    AnimationCommand* addPosition(QPointF start);

    /** Create a relative pause command. The sprite does nothing for the given
      * amount of [ms].
      * @param duration The pause duration [ms]
      * @return The command.
      */
    AnimationCommand* addPause(int duration);  

    /** Create an absolute wait command. The sprite does nothing until the given
      * animation time. If the time is in the past nothing is done. Used to
      * synchronize several sprites.
      * @param duration The time until when to pause [ms]
      * @return The command.
      */
    AnimationCommand* addWait(int duration);   

    /** Create a command to show the sprite.
      * @return The command.
      */
    AnimationCommand* addShow();

    /** Create an command to hide the sprite.
      * @return The command.
      */
    AnimationCommand* addHide();

    /** Create a linear movement from start to end with the given duration.
      * @param start    The start point [rel. coord]
      * @param end      The end point [rel. coord]
      * @param duration The duration of the move [ms]
      * @return The command.
      */
    AnimationCommand* addLinear(QPointF start, QPointF end, int duration);

    /** Create a linear movement from the current position to end with the given duration.
      * @param end      The end point [rel. coord]
      * @param duration The duration of the move [ms]
      * @return The command.
      */
    AnimationCommand* addRelativeLinear(QPointF end, int duration);

    /** Create a command to move in a full circle around the given center position.
      * The circle will have the given radius and take the given amount of time to
      * perform.
      * @param start   The center of the circle [rel. coord]
      * @param radius  The circle radius [rel. coord]
      * @param duration The duration of the move [ms]
      * @return The command.
      */
    AnimationCommand* addCircle(QPointF start, double radius, int duration);

    /** Create a linear manhatte movement (edge following) from the current position to end
      * with the given duration.
      * @param end      The end point [rel. coord]
      * @param velocity The velocity of the move  times 1000 [rel]
      * @return The command.
     */
    AnimationCommand* addRelativeManhatten(QPointF end, double velocity);

    /** Retrieve the time the whole animation script takes.
      * @return The animation duration  [ms]
      */
    int animationDuration();

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
    int duration(AnimationCommand* anim);

    /** Delay the animation of this sprite by the given amount in [ms].
      * Used to compensate for long delays in SVG resizes etc.
      * @param duration The delay [ms]
      */
    void delayAnimation(int duration);

  protected:
    /** Execute a given animation command for the given time relative to the
      * start of the command.
      * @param anim    The command
      * @param elapsed The time since the begin of the command [ms]
      */
    void executeCmd(AnimationCommand* anim, int elapsed);

    /** Retrieve the start position of the last added animation command.
      * @return The position [rel. coord].
      */
    QPointF previousStart();

    /** Retrieve the end position of the last added animation command.
      * @return The position [rel. coord].
      */
    QPointF previousEnd();

  private:
    /// The current running time for the animation.
    QTime mTime;

    // List of animation commands
    QList<AnimationCommand*> mAnimList;

    // Time since start of an animation command [ms]
    int mDeltaT;

    // Global animation delay [ms]
    int mDelayT;

    // Start animation
    bool mStartAnimation;

    // Debug timer
    QTime mDebugTime;
};

#endif
