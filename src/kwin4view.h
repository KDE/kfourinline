#ifndef KWIN4_VIEW_H
#define KWIN4_VIEW_H
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
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSize>
#include <QPoint>
#include <QResizeEvent>
#include <QDataStream>
#include <QMouseEvent>

// KDE includes
#include <kgameio.h>

// Local includes
#include "thememanager.h"

// Forward declaration
class DisplayIntro;
class DisplayGame;
class Score;

/**
 * The view object which shows the graphics for the game.
 */
class KWin4View : public QGraphicsView
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param size          The canvas size
     *  @param advancePeriod The canvas advance period
     *  @param scene         The graphics scene
     *  @param theme         The theme manager
     *  @param parent        The parent window
     */
    KWin4View(QSize size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent = 0);

    /** Desstructor
      */
    ~KWin4View();

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

  signals:
    /** Emit this signal if a sprite animation move is finished.
      * @param mode  A user defined parameter.
      */
    void signalMoveDone(int mode);

  public slots:  
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

     /** Animation of a sprite is finised.
       * @param item   The item
       * @param mode   A user defined mode
       */
     void moveDone(QGraphicsItem* item, int mode);

  protected:
    /**
     * Will be called when the widgets contents
     * are resized. Resized and rescale game.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);
    /** Paint function for the widget, temporary fix while we wait for QGV 4.3
      * @param event The paint event
      */
    void paintEvent(QPaintEvent* event);

  private:
    // The theme manager 
    ThemeManager* mTheme;
    
    // The scene to plot to
    QGraphicsScene* mScene;
    
    // The advance period of the scene [ms]
    int mAdvancePeriod;
    
    // The intro display engine
    DisplayIntro* mIntroDisplay;
    
    // The game display engine
    DisplayGame* mGameDisplay;
    
    // Status of the game (running or not)
    bool mIsRunning;
};

#endif
