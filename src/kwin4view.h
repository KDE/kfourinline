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


/** Temporary view class for the KWin4 game.
  * @TODO  Temporary fix suggested by Cyril Bailly, to reduce QGV cpu usage during intro animation
  * @TODO  Cuts cpu usage in half for this particular case, removing hiccups
  * @TODO  This issue needs to be re-evaluated against Qt4.3 when/if it lands in SVN
  */
class KWinGraphicsView :public QGraphicsView
{
 	Q_OBJECT
 	
  public:
    /** Create a view.
      * @param scene  The graphics scene to use
      * @param parent The parent widget
      */
	KWinGraphicsView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent) { };
	    
  protected:
    /** Paint function for the widget.
      * @param event The paint event
      */
	void paintEvent(QPaintEvent* event);
};


/**
 * The view object which shows the graphics for the game.
 */
class KWin4View : public KWinGraphicsView
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
    void initGame();

    /** Retrieve the display engine for the game.
      * @return The display.
      */
    DisplayGame* display() {return mGameDisplay;} 


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


  protected:
    /**
     * Will be called when the widgets contents
     * are resized. Resized and rescale game.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);

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
