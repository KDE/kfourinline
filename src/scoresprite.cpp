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

// General includes
#include <math.h>

// Qt includes
#include <QSizeF>
#include <QFont>

// KDE includes
#include <kdebug.h>
#include <kconfig.h>

// Local includes
#include "scoresprite.h"

// Constructor for the view
ScoreSprite::ScoreSprite(QString id, ThemeManager* theme, int advancePeriod, int no, QGraphicsScene* canvas)
    :  Themable(id, theme), PixmapSprite(advancePeriod, no, canvas)
{
  kDebug() << "SCore consruct started" <<endl;
  for (int i=0; i<2; i++)
  {
    mWon[i]   = new QGraphicsTextItem(this, canvas);
    mDraw[i]  = new QGraphicsTextItem(this, canvas);
    mLoss[i]  = new QGraphicsTextItem(this, canvas);
    mBreak[i] = new QGraphicsTextItem(this, canvas);
    mName[i]  = new QGraphicsTextItem(this, canvas);
  }
    mAI  = new QGraphicsTextItem(this, canvas);

  // TODO: ALL HARDCODED
  mWon[0]->setPlainText("0000");
  mWon[1]->setPlainText("0001");
  mDraw[0]->setPlainText("0002");
  mDraw[1]->setPlainText("0003");
  mLoss[0]->setPlainText("0004");
  mLoss[1]->setPlainText("0005");
  mBreak[0]->setPlainText("0006");
  mBreak[1]->setPlainText("0007");
  mName[0]->setPlainText("Bob");
  mName[1]->setPlainText("Alice");
  mAI->setPlainText("9.34");

  if (theme) theme->updateTheme(this);
  kDebug() << "SCore consruct done" <<endl;

}

ScoreSprite::~ScoreSprite()
{
}

void ScoreSprite::changeTheme()
{
  kDebug() << "SCORE SPRITE CHANGE THEME ************************ " << endl;
  PixmapSprite::changeTheme();
  double width  = this->boundingRect().width();
  double height = this->boundingRect().height();
  kDebug() << "Score (w,h)=" << width << ", " << height << endl;

  // TODO: HARDCODED
  setFrame(0);

  // Retrieve theme data
  KConfig* config = thememanager()->config(id());
  QPointF posWon0 = config->readEntry("posWon0", QPointF(1.0,1.0));
  QPointF posWon1 = config->readEntry("posWon1", QPointF(1.0,1.0));
  QPointF posDraw0 = config->readEntry("posDraw0", QPointF(1.0,1.0));
  QPointF posDraw1 = config->readEntry("posDraw1", QPointF(1.0,1.0));
  QPointF posLoss0 = config->readEntry("posLoss0", QPointF(1.0,1.0));
  QPointF posLoss1 = config->readEntry("posLoss1", QPointF(1.0,1.0));
  QPointF posBreak0 = config->readEntry("posBreak0", QPointF(1.0,1.0));
  QPointF posBreak1 = config->readEntry("posBreak1", QPointF(1.0,1.0));
  QPointF posName0 = config->readEntry("posName0", QPointF(1.0,1.0));
  QPointF posName1 = config->readEntry("posName1", QPointF(1.0,1.0));
  QPointF posAI   = config->readEntry("posAI", QPointF(1.0,1.0));

  double fontHeight = config->readEntry("fontHeight", 1.0);
  fontHeight *= height;
  QColor fontColor = config->readEntry("fontColor", Qt::white);

  kDebug() << " Font is " << fontHeight << " pixel " << endl;

  // Set position
  mWon[0]->setPos(posWon0.x()*width, posWon0.y()*height);
  mWon[1]->setPos(posWon1.x()*width, posWon1.y()*height);
  mDraw[0]->setPos(posDraw0.x()*width, posDraw0.y()*height);
  mDraw[1]->setPos(posDraw1.x()*width, posDraw1.y()*height);
  mLoss[0]->setPos(posLoss0.x()*width, posLoss0.y()*height);
  mLoss[1]->setPos(posLoss1.x()*width, posLoss1.y()*height);
  mBreak[0]->setPos(posBreak0.x()*width, posBreak0.y()*height);
  mBreak[1]->setPos(posBreak1.x()*width, posBreak1.y()*height);
  mName[0]->setPos(posName0.x()*width, posName0.y()*height);
  mName[1]->setPos(posName1.x()*width, posName1.y()*height);
  mAI->setPos(posAI.x()*width, posAI.y()*height);


  // Create and set font
  QFont font;
  font.setPixelSize(fontHeight);

  for (int i=0; i<2; i++)
  {
    mWon[i]->setFont(font);   
    mDraw[i]->setFont(font); 
    mLoss[i]->setFont(font);
    mBreak[i]->setFont(font);
    mName[i]->setFont(font);
    mWon[i]->setDefaultTextColor(fontColor);   
    mDraw[i]->setDefaultTextColor(fontColor);
    mLoss[i]->setDefaultTextColor(fontColor);
    mBreak[i]->setDefaultTextColor(fontColor);
    mName[i]->setDefaultTextColor(fontColor);
  }
  mAI->setFont(font);
  mAI->setDefaultTextColor(fontColor);   

  kDebug() << "SCORE SPRITE CHANGE THEME ****** DONE ************ " << endl;
}


// CanvasItem advance method 
void ScoreSprite::advance(int phase)
{
  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    PixmapSprite::advance(phase);
    return ;
  }


  // Advance time and animation etc
  PixmapSprite::advance(phase);

}

