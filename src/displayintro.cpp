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

// Header includes
#include "displayintro.h"

// Standard includes
#include <math.h>

// Qt includes
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QPoint>
#include <QGraphicsView>
#include <QStringList>

// KDE includes
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "introsprite.h"
#include "pixmapsprite.h"



// Constructor for the intro display
DisplayIntro::DisplayIntro(int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QGraphicsView* parent)
          : Themable("introdisplay",theme), QObject(parent)
{
  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));
  
  // Store the theme manager and other attributes
  mTheme         = theme;
  mScene         = scene;
  mView          = parent;
  mAdvancePeriod = advancePeriod;

  // Storage of all sprites
  mSprites.clear();

  // Create all sprites used for intro
  for (int i=0; i<42; i++)
  {
    IntroSprite* sprite = new IntroSprite("intro_piece", mTheme, mAdvancePeriod, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite " << "intro_piece" << endl;
    mSprites.append(sprite);
    if ((i/2)%2==0) sprite->setFrame(0);
    else  sprite->setFrame(1);
    sprite->setZValue(i);
    sprite->hide();
  }

  // Static decoration
  KConfigGroup config = thememanager()->config(id());
  QStringList deco = config.readEntry("decoration", QStringList());
  for (int i = 0; i < deco.size(); i++)
  {
    PixmapSprite* sprite = new PixmapSprite(deco.at(i), mTheme, mAdvancePeriod, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite " << deco.at(i) << endl;
    mSprites.append(sprite);
    sprite->show();
  }


  // Animation timer
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(advance()));

  // Redraw
  if (theme) theme->updateTheme(this);
}


// Desctruct the intro and all sprites
DisplayIntro::~DisplayIntro()
{
  delete mTimer;
  while (!mSprites.isEmpty())
  {
    delete mSprites.takeFirst();
  }
}


// Master theme change function. Redraw the display
void DisplayIntro::changeTheme()
{
  // Retrieve theme data
  KConfigGroup config = thememanager()->config(id());
  
  // Retrieve background pixmap
  QString bgsvgid = config.readEntry("background-svgid");
  QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, mScene->sceneRect().size().toSize());
  mScene->setBackgroundBrush(pixmap);
  mView->update();
}


// Start the animation
void DisplayIntro::start()
{
  // Begin with the first state
  mIntroState = IntroMoveIn;

  // Tun the timer
  mTimer->setSingleShot(true);
  mTimer->start(0);
}


// Animation main routine to advance the aniamtion. Called
// by a timer
void DisplayIntro::advance()
{
	// Local variables
  double dura, delay, rad;
  QPointF start, end;

  // First part of intro animation. Move sprites into window
  if (mIntroState == IntroMoveIn)
  {
  	// Loop all sprites
    for (int i = 0; i < mSprites.size(); ++i) 
    {
    	// Move only intro sprites
      if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
      
      IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
      int no = sprite->number();
      {
        if (no%2==0)
        {
          start = QPointF(1.05, 0.5);
          end   = QPointF(0.35 + no/300.0, 105.0/800.0+no/125.0);
          dura  = 3000.0;
          delay = 80.0*no;
          rad   = 0.1;
        }
        else
        {
          start = QPointF(-0.05, 0.5);
          end   = QPointF(0.65-(no-1)/300.0, 105.0/800.0+(no-1)/125.0);
          dura  = 3000.0;
          delay = 80.0*(no-1);
          rad   = 0.1;
        }
        sprite->setZValue(no);
        sprite->startIntro(start, end, rad, dura, delay);
      }
    }// end list loop

    // Continue with next intro state
    mIntroState = IntroCollapse;
    mTimer->start(9000); // [ms]
    return;
  }// end IntroMoveIn

  // Second part of intro animation. Move sprites inwards
  if (mIntroState == IntroCollapse)
  {
  	// Loop all sprites
    for (int i = 0; i < mSprites.size(); ++i) 
    {
    	// Move only intro sprites
      if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
      
      IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
      sprite->startLinear(QPointF(0.5, 0.3), 200);
    }// end for

    // Continue with next intro state
    mIntroState = IntroExplode;
    mTimer->start(500);  // [ms]
    return;
  }// end IntroCollapse

  // Third part of intro animation. Move sprites outwards
  if (mIntroState == IntroExplode)
  {
  	// Loop all sprites
    for (int i = 0; i < mSprites.size(); ++i) 
    {
    	// Move only intro sprites
      if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
      
      IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
      int no = sprite->number();
      double x = 0.5 + 1.50*cos(no/42.0*2.0*M_PI);
      double y = 0.3 + 1.50*sin(no/42.0*2.0*M_PI);
      sprite->startLinear(QPointF(x,y), 800);
    }// end for

    // Start again with first intro state
    mIntroState = IntroMoveIn;
    mTimer->start(1900);  // [ms]
    return;
  }// end IntroCollapse
}


#include "displayintro.moc"
