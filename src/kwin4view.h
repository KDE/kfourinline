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
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QSize>
#include <QPoint>
#include <QHash>
#include <QList>
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

//Temporary fix suggested by Cyril Bailly, to reduce QGV cpu usage during intro animation
//Cuts cpu usage in half for this particular case, removing hiccups
//This issue needs to be re-evaluated against Qt4.3 when/if it lands in SVN
class KWinGraphicsView :public QGraphicsView
{
 	Q_OBJECT
public:
	KWinGraphicsView(QGraphicsScene * scene, QWidget * parent) : QGraphicsView(scene, parent) { };
protected:
	void paintEvent ( QPaintEvent * event );
};


/**
 * The view object which shows the graphics in a
 * canvas view.
 */
class KWin4View : public KWinGraphicsView
{
  Q_OBJECT

  public:
    /** Constructor for the canvas view.
     *  @param size The canvas fixed size
     *  @param advancePeriod The canvas advance period
     *  @param scene The graphics scene
     *  @param theme The theme manager
     *  @param parent The parent window
     */
    KWin4View(QSize size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent = 0);

    /** Desstructor
    */
    ~KWin4View();

    /** Setup the game view.
    */
    void initGame();

    DisplayGame* display() {return mGameDisplay;} 


 protected:
    /** React to mouse clicks
     *  @param ev The mouse event
     */
    void mousePressEvent(QMouseEvent *event);

  public slots:  
    /** The update and advance for the canvas. 
     *  This is called by a timer at regular intervals.
     */
    void updateAndAdvance();

    void mouseInput(KGameIO *input,QDataStream &stream,QMouseEvent *mouse,bool *eatevent);
    void keyInput(KGameIO *input,QDataStream &stream,QKeyEvent *e,bool *eatevent);

  protected slots:  

  signals:
   // void signalLeftMousePress(QPoint point);

  protected:
    /**
     * Will be called by the Qt KWin4View when its contents
     * are resized. We adapt the canvas then.
     * @param e The resize event
     */
    void resizeEvent(QResizeEvent* e);

  private:
    ThemeManager* mTheme;
    QGraphicsScene* mScene;
    int mAdvancePeriod;
    DisplayIntro* mIntroDisplay;
    DisplayGame* mGameDisplay;
    bool mIsRunning;
};

#endif
