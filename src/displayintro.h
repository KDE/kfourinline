/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DISPLAY_INTRO_H
#define DISPLAY_INTRO_H

// own
#include "thememanager.h"
#include "kwin4global.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgameio.h>
// Qt
#include <QGraphicsScene>
#include <QHash>
#include <QList>
#include <QTimer>
#include <QEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>

// Forward declaration
class QGraphicsView;
class PixmapSprite;
class ButtonSprite;


/**
 * The view object which shows the graphics of the welcome screen
 * animation.
 */
class DisplayIntro : public QObject, public virtual Themeable
{
  Q_OBJECT

  public:
    /** Constructor for the intro display.
     *  @param scene         The graphics scene
     *  @param theme         The theme manager
     *  @param parent        The parent window
     */
    DisplayIntro(QGraphicsScene* scene, ThemeManager* theme,  QGraphicsView* parent);
    
    /** Destructor of the display.
     */
    ~DisplayIntro() override;

    /** Start the animation.
      * @param delay An optional delay for the animation [ms]
      */
    void start(int delay=1000);

    /** Theme change method. The object has to completely redraw
      */
    void changeTheme() override;

    /* Called from the view event viewportEvent() to handle mouse events.
     * NOTE: An own event handler is implemented  because the Qt4.3 QGraphicsView event
     *       handling system is faulty!!!! This can be moved to Qt events if they work.
     * @param event The event
     */
    void viewEvent(QEvent* event);

  protected:  
    /** Setup the sprite animation scripts.
      * @param restartTime Restart the time (debug, should be true)
      * @return The animation duration [ms].
      */
    int createAnimation(bool restartTime);

    /** Put a global delay on the animation of all sprites. Could be caused by
      * slow SVG resizes.
      * @param duration Delay [ms]
      */
    void delaySprites(int duration);

  Q_SIGNALS:
    /** Emit this signal if a new game is started from the intro display.
      * @param startPlayer Color of the starting player
      * @param input0      Input device of player 1
      * @param input1      Input device of player 2
      * @param aiLevel     Level for AI (-1: no change)
      */
    void signalQuickStart(COLOUR startPlayer, KGameIO::IOMode input0, KGameIO::IOMode input1, int aiLevel);


  protected Q_SLOTS:  
     /** Animation routine, called by a timer.
       */
     void advance();

     /** A graphic button is pressed.
       * @param item The button
       * @param id   The button id
       */
     void buttonPressed(QGraphicsItem* item, int id);

     /** Find a sprite for a given absolute widget coordinate.
       * @param pos The screen coordinate
       * @return The highest sprite under the position or null.
       */
     QGraphicsItem* findSprite(QPoint pos);


  private:
    // The theme manager
    ThemeManager* mTheme;
    
    // The graphics scene to write to
    QGraphicsScene* mScene;
    
    // List of all sprites used
    QList<QGraphicsItem*> mSprites;
    
    // The animation timer
    QTimer* mTimer;

    // The view
    QGraphicsView* mView;
    
    // Text items
    QGraphicsTextItem* mTextQuicklaunch;
    // Text items
    QGraphicsTextItem* mTextStartplayer;
    // Text items
    QGraphicsTextItem* mTextColor;

    // Buttons
    ButtonSprite* mStartButton[2];
    ButtonSprite* mPlayerButton[2];

    // Quick launch sprite
    PixmapSprite* mQuickLaunch;

    // Last event sprite
    QGraphicsItem* mLastMoveEvent;


};

#endif
