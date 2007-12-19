#ifndef BUTTON_SPRITE_H
#define BUTTON_SPRITE_H
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
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>


// Local includes
#include "thememanager.h"
#include "pixmapsprite.h"
#include "spritenotify.h"


/** The sprite for a interactive buttons. The button can either be
  * a push button which reacts on mouse press events or a toggle button
  * which is toggled on mouse release.
  *
  * NOTE: An own event handler is implemeted currently in the DisplayIntro
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
    ~ButtonSprite();

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+10
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+100;}

    /** Main theme change function. On call of this the item needs to redraw and
      * resize.
      */
    virtual void changeTheme();

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
    virtual void mousePressEvent(QMouseEvent* event);

    /** A mouse release event is received.
      * @param event The mouse event
      */
    virtual void mouseReleaseEvent(QMouseEvent* event);

    /** A hover in event is reveived (mouse tracking must be on).
      * @param event The mouse event
      */
    virtual void hoverEnterEvent(QMouseEvent* event);  

    /** A hover out event is reveived (mouse tracking must be on).
      * @param event The mouse event
      */
    virtual void hoverLeaveEvent(QMouseEvent* event);  


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
