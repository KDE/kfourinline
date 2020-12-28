/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PIECE_SPRITE_H
#define PIECE_SPRITE_H

// own
#include "thememanager.h"
#include "pixmapsprite.h"
#include "spritenotify.h"
// Qt
#include <QGraphicsPixmapItem>
#include <QPointF>


/** The sprite for a pixmap piece on the canvas. The pixmap can be
  * animated and moving.
 */
class PieceSprite : public PixmapSprite
{

  public:
    /** Constructor for the sprite.
     */
    PieceSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* canvas);
    
    /** Destructor 
     */
    ~PieceSprite() override;

    /** Possible animation states of the sprite
    */
    enum MovementState {Idle, LinearMove};

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    void advance(int phase) override;

    /** Retrieve the type of QGI. This item is UserType+2
     *  @return The type of item.
     */
    int type() const override {return QGraphicsItem::UserType+2;}

    /** Standard Themeable function. It is called when the theme item
      * needs to completely refresh itself.
      */
    void changeTheme() override;

    /** Retrieve the sprite notification object. This object indicates the
      * end of a movement.
      * @return The notification object.
      */
    SpriteNotify* notify() {return mNotify;}

    /** Start a linear movement from the current position to the given
      * end position with the given velocity.
      * @param end      The end position in relative coordinates [rel 0..1, rel 0..1]
      * @param velocity The velocity of the move in [relative units/sec.]
      */
    void startLinear(QPointF end, double velocity);
    
    /** Start a linear movement from the start point to the given
      * end position with the given velocity.
      * @param start    The start position in relative coordinates [rel 0..1, rel 0..1]
      * @param end      The end position in relative coordinates [rel 0..1, rel 0..1]
      * @param velocity The velocity of the move in [relative units/sec.]
      */
    void startLinear(QPointF start, QPointF end, double velocity);



  private:

    /// The duration of the movement 
    double mDuration;

    /// The state of the movement
    MovementState mMovementState;

    /// The end points of the movement [rel]
    QPointF mEnd;

    /// The movement sprite notifier
    SpriteNotify* mNotify;

};

#endif
