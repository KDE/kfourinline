/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BUTTON_SPRITE_H
#define BUTTON_SPRITE_H

// own
#include "thememanager.h"
#include "pixmapsprite.h"
#include "spritenotify.h"
// Qt
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>


/** The sprite for a interactive buttons. The button can either be
  * a push button which reacts on mouse press events or a toggle button
  * which is toggled on mouse release.
  *
  * NOTE: An own event handler is implemented currently in the DisplayIntro
  *       class which uses this buttons because the Qt4.3 QGraphicsView event
  *       handling system is faulty!!!!
 */
class ButtonSprite : public PixmapSprite
{
  public:
    /** Constructor for the score sprite.
      * @param pushButton      True: push button, False: Toggle button
      * @param id              The theme id
      * @param theme           The theme manager
      * @param no              A used defined number (unused)
      * @param scene           The graphics scene
      */
    ButtonSprite(bool pushButton, const QString &id, ThemeManager* theme, int no, QGraphicsScene* scene);
    
    /** Destructor 
      */
    ~ButtonSprite() override;

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    void advance(int phase) override;

    /** Retrieve the type of QGI. This item is UserType+10
     *  @return The type of item.
     */
    int type() const override {return QGraphicsItem::UserType+100;}

    /** Main theme change function. On call of this the item needs to redraw and
      * resize.
      */
    void changeTheme() override;

    /** Store the button text.
      * @param s  The text
      */ 
    void setText(const QString &s);

    /** Set the status of the button to on or off.
      * @param status The status
      */
    void setStatus(const bool status);  

    /** Retrieve the status of the button (on or off).
      * @return The status
      */
    bool status() const;

    /** Allow the sprite to emit a signal. Used for button events.
      * @return The notify object.
      */
    SpriteNotify* notify() {return mSignal;}

    /** A mouse press event is received.
      * @param event The mouse event
      */
    void mousePressEvent(QMouseEvent* event);

    /** A mouse release event is received.
      * @param event The mouse event
      */
    void mouseReleaseEvent(QMouseEvent* event);

    /** A hover in event is received (mouse tracking must be on).
      * @param event The mouse event
      */
    void hoverEnterEvent(QMouseEvent* event);

    /** A hover out event is received (mouse tracking must be on).
      * @param event The mouse event
      */
    void hoverLeaveEvent(QMouseEvent* event);

    // Silence gcc warnings
    using QGraphicsPixmapItem::mousePressEvent;
    using QGraphicsPixmapItem::mouseReleaseEvent;
    using QGraphicsPixmapItem::hoverEnterEvent;
    using QGraphicsPixmapItem::hoverLeaveEvent;

  protected:
    /** Change the frame of the sprite to match the button state.
      */
    void changeFrame();

    
  private:
    // Button text 
    QGraphicsTextItem* mText;
    // State of button
    bool mButtonPressed;
    // State of hover
    bool mHover;
    // Is push button?
    bool mPushButton;

    // The notification object
    SpriteNotify* mSignal;

};

#endif
