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
#include "displaygame.h"
#include "thememanager.h"
#include "piecesprite.h"
#include "pixmapsprite.h"
#include "scoresprite.h"
#include "spritenotify.h"
#include "kwin4doc.h"



// Constructor for the view
DisplayGame::DisplayGame(int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QGraphicsView* parent)
           : Themable("gamedisplay",theme), QObject(parent)
{
  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));
  mScene = scene;
  mView  = parent;
  mAdvancePeriod = advancePeriod;
  // Store the theme manager
  mTheme = theme;

  // Store all sprites
  mSprites.clear();

  // Create piece sprites 
  for (int i=0; i<42; i++)
  {
    PieceSprite* sprite = new PieceSprite("piece", mTheme, mAdvancePeriod, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite " << "piece" << endl;
    mSprites.append(sprite);
    mPieces.append(sprite);
    sprite->setZValue(sprite->zValue()+i);
    sprite->hide();
  }
  // Create stars
  for (int i=0;i<4;i++)
  {
    PixmapSprite* sprite = new PixmapSprite("star", mTheme, mAdvancePeriod, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite " << "star" << endl;
    mSprites.append(sprite);
    mStars.append(sprite);
    sprite->hide();
  }

  // Board
  mBoard = new PixmapSprite("board", mTheme, mAdvancePeriod, 0, mScene);
  mSprites.append(mBoard);
  mBoard->hide();

  // Board holes
  mBoardHoles = new PixmapSprite("boardholes", mTheme, mAdvancePeriod, 0, mScene);
  mSprites.append(mBoardHoles);
  mBoardHoles->hide();

  // Movement bar
  mBar = new PixmapSprite("movebar", mTheme, mAdvancePeriod, 0, mScene);
  mSprites.append(mBar);
  mBar->hide();

  // Score board
  mScoreBoard = new ScoreSprite("scoreboard", mTheme, mAdvancePeriod, 0, mScene);
  mSprites.append(mScoreBoard);
  mScoreBoard->hide();


  // Movement indication arrows
  mArrow = new PixmapSprite("arrow", mTheme, mAdvancePeriod, 0, mScene);
  mSprites.append(mArrow);
  mArrow->hide();

  // Animation timer
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(run()));

  theme->updateTheme(this);

}

DisplayGame::~DisplayGame()
{
  kDebug() << "~DisplayGame"<<endl;
  delete mTimer;
  while (!mSprites.isEmpty())
  {
    delete mSprites.takeFirst();
  }
  kDebug() << "~DisplayGame done"<<endl;
}

// Called by thememanager when theme or theme geometry changes
void DisplayGame::changeTheme()
{
  kDebug() << "DisplayGame::changeTheme +++++ "<<endl;

  // Retrieve theme data
  KConfig* config = thememanager()->config(id());

  // Retrieve background pixmap
  QString bgsvgid = config->readEntry("background-svgid");
  QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, mScene->sceneRect().size().toSize());
  mScene->setBackgroundBrush(pixmap);
  mView->update();

}

// Start a new game. Initialize graphics.
void DisplayGame::start()
{
  mTimer->setSingleShot(true);
  mTimer->start(0);

  // Show decoration
  mBoard->show();
  mBoardHoles->show();
  mBar->show();
  mScoreBoard->show();
  mArrow->hide();

  // Hide piece sprites 
  for (int i=0; i<42; i++)
  {
    mPieces.value(i)->hide();
  }
  // Hide stars
  for (int i=0;i<4;i++)
  {
    mStars.value(i)->setAnimation(false);
    mStars.value(i)->hide();
  }
}

void DisplayGame::run()
{
}


// Set the movement indicator arrows above the game board
void DisplayGame::setArrow(int x,int color)
{
  int y=0;

  // Check for removal of sprite
  if (color==Niemand)
  {
    mArrow->hide();
    return ;
  }

  // Retrieve theme data
  KConfig* config      = thememanager()->config(id());
  QPointF arrow_pos    = config->readEntry("arrow-pos", QPointF(1.0,1.0));
  QPointF board_spread = config->readEntry("board-spread", QPointF(1.0,1.0));

  // Make sure the frames are ok
  int frame;
  if (color==Gelb) frame = 0;
  else frame = 1;

  QPointF to   = QPointF(board_spread.x()*x + arrow_pos.x(),
                         board_spread.y()*y + arrow_pos.y());
  mArrow->setFrame(frame);
  mArrow->setPosition(to);
  mArrow->show();
}


// Set a game piece, red or white or hidden depending on 'color'
SpriteNotify* DisplayGame::setPiece(int x, int y, int color, int no, bool animation)
{
  y=5-y;

  PieceSprite *sprite = mPieces.value(no);
  // kDebug() << " setPiece("<<x<<","<<y<<","<<color<<","<<no<<") sprite=" << sprite<<endl;

  // Check for removal of sprite
  if (color==Niemand)
  {
    sprite->hide();
    return 0;
  }

  // Retrieve theme data
  KConfig* config      = thememanager()->config(id());
  QPointF board_pos    = config->readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread = config->readEntry("board-spread", QPointF(1.0,1.0));
  double velocity      = config->readEntry("move-velocity", 0.1);

  // Make sure the frames are ok
  int frame;
  if (color==Gelb) frame = 0;
  else frame = 1;

  if (!sprite)
     return 0;
  
  if (animation)
  {
    QPointF from = QPointF(board_spread.x()*x    + board_pos.x(),
                           board_spread.y()*(-1) + board_pos.y());
    QPointF to   = QPointF(board_spread.x()*x    + board_pos.x(),
                           board_spread.y()*y    + board_pos.y());
    sprite->setFrame(frame);
    sprite->startLinear(from, to, velocity);
  }
  else
  {
    QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                           board_spread.y()*y + board_pos.y());
    sprite->setFrame(frame);
    sprite->setPosition(to);
  }

  sprite->show();
  return sprite->notify();
}


// Return the mouse mapped to the board or bar item so that a
// move 0..6 is generated. -1 means an illegal position
int DisplayGame::mapMouseToMove(QPoint pos)
{
  if (!mBoard) return -1;

  KConfig* config      = thememanager()->config(id());
  QPointF board_spread = config->readEntry("board-spread", QPointF(1.0,1.0));

  double scale = thememanager()->getScale();
  QPointF p1 = mBoardHoles->mapFromParent(QPointF(pos));
  p1 = p1/scale;
  double xPos = p1.x() / board_spread.x();
  // kDebug() <<"Board holes map " << p1 << " is "<<xPos << endl;

  int x = int(xPos);

  if (x< 0 || x>=FIELD_SIZE_X) return -1;
  return int(xPos);
}


// Draw Star Sprites as winning indicator
void DisplayGame::drawStar(int x,int y,int no)
{
  y=5-y;
  PixmapSprite* star = mStars.value(no-1);
  // kDebug() << " setStar("<<x<<","<<y<<","<<no<<") star=" << star<<endl;

  // Retrieve theme data
  KConfig* config      = thememanager()->config(id());
  QPointF board_pos    = config->readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread = config->readEntry("board-spread", QPointF(1.0,1.0));
  double velocity      = config->readEntry("move-velocity", 0.1);


  if (star)
  {
    QPointF pos  = QPointF(board_spread.x()*x    + board_pos.x(),
                           board_spread.y()*y    + board_pos.y());
    star->setAnimation(true);
    star->setPosition(pos);
    star->show();
  }
}

#include "displaygame.moc"
