/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "scoresprite.h"

// own
#include "kfourinline_debug.h"
// KF
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
// Qt
#include <QFont>
#include <QGraphicsScene>
// Std
#include <cmath>

// Constructor for the score sprite
ScoreSprite::ScoreSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* scene)
           :  Themeable(id, theme), PixmapSprite(no, scene)
{
	// Create all sub sprites
  for (int i=0; i<2; i++)
  {
    mWon[i]   = new QGraphicsTextItem(this);
    scene->addItem(mWon[i]);
    mDraw[i]  = new QGraphicsTextItem(this);
    scene->addItem(mDraw[i]);
    mLoss[i]  = new QGraphicsTextItem(this);
    scene->addItem(mLoss[i]);
    mBreak[i] = new QGraphicsTextItem(this);
    scene->addItem(mBreak[i]);
    mName[i]  = new QGraphicsTextItem(this);
    scene->addItem(mName[i]);
    mInput[i] = new PixmapSprite(QStringLiteral("scoreinput%1").arg(i), theme, i, scene);
    if (!mInput[i]) qCCritical(KFOURINLINE_LOG) << "Cannot load sprite" << "scoreinput"<<i;
    mInput[i]->setParentItem(this);
    mInput[i]->setOffsetStatus(false);
    mInput[i]->show();
    mInputFrame[i] = 0;
  }

  // Default turn is nobody
  mTurn  = -1;

  // Redraw us
  if (theme) theme->updateTheme(this);

}


// Destructor
ScoreSprite::~ScoreSprite()
{
	// Clean up
  for (int i=0; i<2; i++)
  {
    delete mWon[i];
    delete mDraw[i];
    delete mLoss[i];
    delete mBreak[i];
    delete mName[i];
    delete mInput[i];
  }
}


// Redraw the theme.
void ScoreSprite::changeTheme()
{
  // The main display is handled by the parent
  PixmapSprite::changeTheme();

  // Retrieve our size
  double width  = this->boundingRect().width();
  double height = this->boundingRect().height();

  // Retrieve theme data
  KConfigGroup config = thememanager()->config(id());
  QPointF posWon0     = config.readEntry("posWon0", QPointF(1.0,1.0));
  QPointF posWon1     = config.readEntry("posWon1", QPointF(1.0,1.0));
  QPointF posDraw0    = config.readEntry("posDraw0", QPointF(1.0,1.0));
  QPointF posDraw1    = config.readEntry("posDraw1", QPointF(1.0,1.0));
  QPointF posLoss0    = config.readEntry("posLoss0", QPointF(1.0,1.0));
  QPointF posLoss1    = config.readEntry("posLoss1", QPointF(1.0,1.0));
  QPointF posBreak0   = config.readEntry("posBreak0", QPointF(1.0,1.0));
  QPointF posBreak1   = config.readEntry("posBreak1", QPointF(1.0,1.0));
  QPointF posName0    = config.readEntry("posName0", QPointF(1.0,1.0));
  QPointF posName1    = config.readEntry("posName1", QPointF(1.0,1.0));
  //QPointF posAI       = config.readEntry("posAI", QPointF(1.0,1.0));

  // Calculate proper font size
  double fontHeight = config.readEntry("fontHeight", 1.0);
  fontHeight *= height;
  double fontWidth = config.readEntry("fontWidth", 1.0);
  fontWidth *= width;
  
  // Retrieve font color
  QColor fontColor[2];
  fontColor[0] = config.readEntry("fontColorPlayer0", QColor(Qt::white));
  fontColor[1] = config.readEntry("fontColorPlayer1", QColor(Qt::white));

  // Set position of sub sprites
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


  // Create and set current font
  QFont font;
  font.setPixelSize(int(fontHeight));

  // Set font and color for all text items
  for (int i=0; i<2; i++)
  {
    mWon[i]->setFont(font);   
    mDraw[i]->setFont(font); 
    mLoss[i]->setFont(font);
    mBreak[i]->setFont(font);
    mName[i]->setFont(font);
    
    mWon[i]->setDefaultTextColor(fontColor[i]);   
    mDraw[i]->setDefaultTextColor(fontColor[i]);
    mLoss[i]->setDefaultTextColor(fontColor[i]);
    mBreak[i]->setDefaultTextColor(fontColor[i]);
    mName[i]->setDefaultTextColor(fontColor[i]);

    mWon[i]->setTextWidth(fontWidth);   
    mDraw[i]->setTextWidth(fontWidth);
    mLoss[i]->setTextWidth(fontWidth);
    mBreak[i]->setTextWidth(fontWidth);
    mName[i]->setTextWidth(fontWidth);

    // Restore the frame of the input device sprite
    if (mInputFrame[i]>=0) mInput[i]->setFrame(mInputFrame[i]);
  }

  // Update next player
  if (mTurn>=0) setTurn(mTurn);
}


// QGI advance method 
void ScoreSprite::advance(int phase)
{
  // Advance time and animation etc
  PixmapSprite::advance(phase);
}


// Store and display the level of the AI
void ScoreSprite::setLevel(int level, int no)
{
  if (level >= 0)
  {
    mName[no]->setPlainText(i18nc("computer level","Level %1", level));
    update();
  }
}


// Store and display the name of a player
void ScoreSprite::setPlayerName(const QString &s,int no)
{
  mName[no]->setPlainText(s);
  update();
}


// Store and display amount of wins
void ScoreSprite::setWon(const QString &s,int no)
{
  mWon[no]->setPlainText(s);
  update();
}


// Store and display amount of draws
void ScoreSprite::setDraw(const QString &s,int no)
{
  mDraw[no]->setPlainText(s);
  update();
}


// Store and display amount of losses
void ScoreSprite::setLoss(const QString &s,int no)
{
  mLoss[no]->setPlainText(s);
  update();
}


// Store and display amount of breaks
void ScoreSprite::setBreak(const QString &s,int no)
{
  mBreak[no]->setPlainText(s);
  update();
}



// Store and display input device
void ScoreSprite::setInput(int device, int no)
{
  // Map KGameIO device numbers to sprite frames
  int frame;
  if (device == 8) frame = 2; // AI
  else if (device == 4) frame = 0; // Mouse
  else if (device == 2) frame = 1; // Key
  else frame = 3; //Network

  mInputFrame[no] = frame;
  mInput[no]->setFrame(frame);
  update();
}



// Store and display current player. This is done by coloring the
// name text sprite.
void ScoreSprite::setTurn(int no)
{
	// Retrieve theme data
  KConfigGroup config     = thememanager()->config(id());
  QColor fontColorActive  = config.readEntry("fontColorActive", QColor(Qt::white));
  QColor fontColor0       = config.readEntry("fontColorPlayer0", QColor(Qt::white));
  QColor fontColor1       = config.readEntry("fontColorPlayer1", QColor(Qt::white));

  // Store data
  mTurn = no;

  // Switch color
  if (no==0)
  {
    mName[0]->setDefaultTextColor(fontColorActive);
    mName[1]->setDefaultTextColor(fontColor1);
  }
  else
  {
    mName[0]->setDefaultTextColor(fontColor0);
    mName[1]->setDefaultTextColor(fontColorActive);
  }

  update();
}


