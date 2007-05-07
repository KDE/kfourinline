#ifndef DISPLAY_INTRO_H
#define DISPLAY_INTRO_H
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
#include <QGraphicsScene>
#include <QHash>
#include <QList>
#include <QTimer>

// Local includes
#include "thememanager.h"

// Forward declaration
class QGraphicsView;


/**
 * The view object which shows the graphics of the welcome screen
 * animation.
 */
class DisplayIntro : public QObject, public virtual Themable
{
  Q_OBJECT

  public:
    /** Constructor for the intro display.
     *  @param advancePeriod The canvas advance period
     *  @param scene         The graphics scene
     *  @param theme         The theme manager
     *  @param parent        The parent window
     */
    DisplayIntro(int advancePeriod, QGraphicsScene* scene, ThemeManager* theme,  QGraphicsView* parent);
    
    /** Destructor of the display.
     */
    ~DisplayIntro();

    /** Animation states of the intro animation
      */
    enum IntroState {IntroMoveIn, IntroCollapse, IntroExplode};

    /** Start the animation.
      */
    void start();

    /** Theme change method. The object has to completely redraw
      */
    virtual void changeTheme();


  protected slots:  
     /** Animation routine, called by a timer.
       */
     void advance();


  private:
    // The theme manager
    ThemeManager* mTheme;
    
    // The graphics scene to write to
    QGraphicsScene* mScene;
    
    // The advance period of the scene [ms]
    int mAdvancePeriod;
    
    // List of all sprites used
    QList<QGraphicsItem*> mSprites;
    
    // The animation timer
    QTimer* mTimer;

    // The view
    QGraphicsView* mView;
    
    // The current state of the animation
    IntroState mIntroState;
};

#endif
