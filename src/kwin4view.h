/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWIN4_KWIN4VIEW_H
#define KWIN4_KWIN4VIEW_H

// own
#include "thememanager.h"
#include "kwin4global.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgameio.h>
// Qt
#include <QDataStream>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QImage>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>
#include <QResizeEvent>
#include <QSize>
#include <QTimer>
#include <QWidget>

// Forward declaration
class DisplayIntro;
class DisplayGame;
class Score;
class ReflectionGraphicsScene;

/**
 * The view object which shows the graphics for the game.
 */
class KWin4View : public QGraphicsView, public virtual Themeable
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param updateTime    The canvas advance rate
     *  @param size          The canvas size
     *  @param scene         The graphics scene
     *  @param theme         The theme manager
     *  @param parent        The parent window
     */
    KWin4View(int updateTime, 
              const QSize &size, 
              ReflectionGraphicsScene* scene, 
              ThemeManager* theme, 
              QWidget* parent = nullptr);

    /** Destructor
      */
    ~KWin4View() override;

    /** Main theme manager function. Called when any theme change like
      * a new theme or a theme size change occurs. This object needs to
      * resize and redraw then.
      */
    void changeTheme() override;

    /** Initial setup of the game view.
      */
    void initGame(Score *scoreData);

    /** Finalize (end) game.
      */
    void endGame();  

    /** Displays a move on the game board. This means a piece of
      * the given number is moved to the given position, the move
      * indicator arrow is switched on and any hints are disabed.
      * The move can be performed animated or not.
      * @param x          The x position on the game board [0-6]
      * @param y          The y position on the game board [0-5]
      * @param color      The color [Red,Yellow,Nobody]
      * @param xarrow     The x position of the arrow [0-6]
      * @param colorarrow The color or the arrow [Red,Yellow,Nobody]
      * @param no         The sprite number / move number
      * @param animation  True to make an animated move
      */
    void displayMove(int x, int y, int color, int xarrow, int colorarrow, int no, bool animation);

    /** Displays a star on the game board to indicate victorious pieces.
      * @param x          The x position on the game board [0-6]
      * @param y          The y position on the game board [0-5]
      * @param no         The sprite number / move number
      */
    void displayStar(int x, int y, int no);

    /** Displays a hint on the game board to indicate a good move.
      * @param x          The x position on the game board [0-6]
      * @param y          The y position on the game board [0-5]
      */
    void displayHint(int x, int y);

    /** Enable reflections on the given position. If width or height is zero
      * the reflections are disabled.
      * @param x      The x position of the reflection  [screen coord]
      * @param y      The y position of the reflection  [screen coord]
      * @param width  The width of the reflection  [screen coord]
      * @param height The height of the reflection  [screen coord]
      */
    void setReflection(int x, int y, int width, int height);

  Q_SIGNALS:
    /** Emit this signal if a sprite animation move is finished.
      * @param mode  A user-defined parameter.
      */
    void signalMoveDone(int mode);

    /** Emit this signal if a new game is started from the intro display.
      * @param startPlayer Color of the starting player
      * @param input0      Input device of player 1
      * @param input1      Input device of player 2
      * @param aiLevel     Level for AI (-1: no change)
      */
    void signalQuickStart(COLOUR startPlayer, KGameIO::IOMode input0, KGameIO::IOMode input1, int aiLevel);

  public Q_SLOTS:  
    /** The update and advance for the canvas. 
     *  This is called by a timer at regular intervals.
     */
    void updateAndAdvance();

    /** Handle mouse inputs for the KGame framework.
      * @param input     The IO device
      * @param stream    The KGame message stream
      * @param mouse     The mouse event
      * @param eatevent  Set to true if the event was processed
      */
    void mouseInput(KGameIO* input, QDataStream& stream, QMouseEvent* mouse, bool* eatevent);

    /** Handle key inputs for the KGame framework.
      * @param input     The IO device
      * @param stream    The KGame message stream
      * @param key       The key event
      * @param eatevent  Set to true if the event was processed
      */
    void keyInput(KGameIO* input, QDataStream& stream, QKeyEvent* key, bool* eatevent);

     /** Animation of a sprite is finished.
       * @param item   The item
       * @param mode   A user defined mode
       */
     void moveDone(QGraphicsItem* item, int mode);

     /** Rescale the theme (update theme SVG graphics).
      */
    void rescaleTheme();


  protected:
    /**
     * Will be called when the widgets contents
     * are resized. Resized and rescale game.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e) override;

    /** Widget viewport event.
      * @param event The event.
      */
    bool viewportEvent ( QEvent * event )   override;

   /** Overwritten Qt function.
    */
    void drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[]) override;


  private:
    // The theme manager 
    ThemeManager* mTheme;

    // Theme queue
    QList<int> mThemeQueue;
    // Theme offset queque
    QList<QPoint> mThemeOffset;
    
    // The scene to plot to
    ReflectionGraphicsScene* mScene;
    
    // The advance period of the scene [ms]
    int mAdvancePeriod;
    
    // The intro display engine
    DisplayIntro* mIntroDisplay;
    
    // The game display engine
    DisplayGame* mGameDisplay;
    
    // Status of the game (running or not)
    bool mIsRunning;

    // Gradient for the reflection
    QLinearGradient mGradient;
    // Image for the reflection
    QImage mGradientImage;
    // Reflection sprite
    QGraphicsPixmapItem* mReflectionSprite;
    // Refection size
    QRect mReflectionRect;
    // Paint image of reflection
    QImage mReflectImage;
    // Phase of reflection drawing
    int mReflectPhase;

    // Debug frame rate sprite
    QGraphicsTextItem* mFrameSprite;
    // Average update times
    QList<int> mDrawTimes;
    
    // Update and advance timer
    QTimer* mTimer;
    // Default update time [ms]
    int mDefaultUpdateTime;
    // Update slow down factor
    double mSlowDownFactor;
    // Slow incident counter
    int mSlowCnt;
};

#endif // KWIN4_KWIN4VIEW_H
