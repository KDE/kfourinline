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
#include <math.h>

// Qt includes
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QPoint>

// KDE includes
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "displayintro.h"
#include "thememanager.h"
#include "introsprite.h"



// Constructor for the view
DisplayIntro::DisplayIntro(int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QObject* parent)
          : Themable("introdisplay",theme), QObject(parent)
{
  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));
  mScene = scene;
  mAdvancePeriod = advancePeriod;
  // Store the theme manager
  mTheme = theme;

  // Store all sprites
  mSprites.clear();

  // Create sprites for intro
  for (int i=0; i<42; i++)
  {
    QString id;
    if ((i/2)%2==0) id = "intro_piece_0";
    else  id = "intro_piece_1";
    IntroSprite* sprite = new IntroSprite(id, mTheme, mAdvancePeriod, i, mScene);
    mSprites.append(sprite);
    sprite->hide();
  }


  // Animation timer
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(run()));

   theme->updateTheme(this);

}

DisplayIntro::~DisplayIntro()
{
  //kDebug() << "~DisplayIntro"<<endl;
  delete mTimer;
  while (!mSprites.isEmpty())
  {
    delete mSprites.takeFirst();
  }
  // kDebug() << "~DisplayIntro done"<<endl;
}

void DisplayIntro::changeTheme()
{
  //kDebug() << "DisplayIntro::changeTheme"<<endl;
}

void DisplayIntro::start()
{

  mIntroState = IntroMoveIn;

  mTimer->setSingleShot(true);
  mTimer->start(0);
}


void DisplayIntro::run()
{
  double dura, delay, rad;
  QPointF start, end;
  QPixmap pixmap;

  // First part of intro animation. Move sprites in
  if (mIntroState == IntroMoveIn)
  {
    for (int i = 0; i < mSprites.size(); ++i) 
    {
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

    // Start next intro state
    mIntroState = IntroCollapse;
    mTimer->start(9000);
    return;
  }// end IntroMoveIn

  // Second part of intro animation. Move sprites in
  if (mIntroState == IntroCollapse)
  {
    for (int i = 0; i < mSprites.size(); ++i) 
    {
      if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
      IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
      sprite->startLinear(QPointF(0.5, 0.3), 200);
    }// end for

    // Start next intro state
    mIntroState = IntroExplode;
    mTimer->start(500);
    return;
  }// end IntroCollapse

  // Second part of intro animation. Move sprites in
  if (mIntroState == IntroExplode)
  {
    for (int i = 0; i < mSprites.size(); ++i) 
    {
      if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
      IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
      int no = sprite->number();
      double x = 0.5 + 1.50*cos(no/42.0*2.0*M_PI);
      double y = 0.3 + 1.50*sin(no/42.0*2.0*M_PI);
      sprite->startLinear(QPointF(x,y), 800);
    }// end for

    // Start next intro state
    mIntroState = IntroMoveIn;
    mTimer->start(1900);
    return;
  }// end IntroCollapse
}


#include "displayintro.moc"
