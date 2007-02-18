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
#include <assert.h>

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
//    sprite->setZValue(sprite->getDoubleValue("zValueOffset")+i);
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
  if (!mBoard) kFatal() << "Cannot load sprite " << "board" << endl;
  mSprites.append(mBoard);
  mBoard->hide();

  // Hint
  mHint = new PixmapSprite("hint", mTheme, mAdvancePeriod, 0, mScene);
  if (!mHint) kFatal() << "Cannot load sprite " << "hint" << endl;
  mSprites.append(mHint);
  mHint->hide();

  // Game Over
  mGameOver = new PixmapSprite("gameover", mTheme, mAdvancePeriod, 0, mScene);
  if (!mGameOver) kFatal() << "Cannot load sprite " << "gameover" << endl;
  mSprites.append(mGameOver);
  mGameOver->hide();

  // Create board holes
  for (int i=0; i<42; i++)
  {
    PixmapSprite* boardHole = new PixmapSprite("boardholes", mTheme, mAdvancePeriod, i, mScene);
    if (!boardHole) kFatal() << "Cannot load sprite " << "boardHoles" << endl;
    mSprites.append(boardHole);
    mBoardHoles.append(boardHole);
    boardHole->hide();
  }

  // Score board
  mScoreBoard = new ScoreSprite("scoreboard", mTheme, mAdvancePeriod, 0, mScene);
  if (!mScoreBoard) kFatal() << "Cannot load sprite " << "scoreboard" << endl;
  mSprites.append(mScoreBoard);
  mScoreBoard->hide();


  // Movement indication arrows
  for (int i=0; i<7; i++)
  {
    PixmapSprite* arrow = new PixmapSprite("arrow", mTheme, mAdvancePeriod, i, mScene);
    if (!arrow) kFatal() << "Cannot load sprite " << "arrow" << endl;
    mSprites.append(arrow);
    mArrows.append(arrow);
    arrow->hide();
  }

  // Animation timer
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(run()));

  theme->updateTheme(this);

}

DisplayGame::~DisplayGame()
{
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

  // Retrieve theme data
  KConfig* config = thememanager()->config(id());
  QPointF board_pos    = config->readEntry("board-pos", QPointF(1.0,1.0));
  QPointF arrow_pos    = config->readEntry("arrow-pos", QPointF(1.0,1.0));
  QPointF board_spread = config->readEntry("board-spread", QPointF(1.0,1.0));

  // Show decoration
  mBoard->show();
  mScoreBoard->show();

  // Board holes
  for (int i=0; i<42; i++)
  {
    int x = i/6;
    int y = i%6;
    QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                           board_spread.y()*y + board_pos.y());
    mBoardHoles.value(i)->setPosition(to);
    mBoardHoles.value(i)->show();
  }

  // Movement arrows
  for (int i=0; i<7; i++)
  {
    QPointF to   = QPointF(board_spread.x()*i + arrow_pos.x(),
                           board_spread.y()*0 + arrow_pos.y());
    mArrows.value(i)->setPosition(to);
    mArrows.value(i)->setFrame(0);
    mArrows.value(i)->show();
  }

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

  // Hide game over
  mGameOver->hide();
}

// Run game
void DisplayGame::run()
{
}


// End game
void DisplayGame::end()
{
  kDebug() << "DisplayGame::end()" << endl;
  mGameOver->show();

}


// Set the movement indicator arrows above the game board
void DisplayGame::setArrow(int x,int color)
{
  // Set all arrows back to frame 0
  for (int i=0; i<7; i++)
  {
    mArrows.value(i)->setFrame(0);
  }

  // Check for no color 
  if (color==Niemand)
  {
    return ;
  }

  // Make sure the frames are ok
  if (color==Gelb) mArrows.value(x)->setFrame(1);
  else mArrows.value(x)->setFrame(2);

}

// Set a game HINT
void DisplayGame::setHint(int x, int y, bool show)
{
  //kDebug() << " setHint("<<x<<","<<y<<","<<show<<") sprite=" << mHint<<endl;
  y=5-y;

  // Check for removal of sprite
  if (!show)
  {
    mHint->hide();
    return;
  }

  // Retrieve theme data
  KConfig* config      = thememanager()->config(id());
  QPointF board_pos    = config->readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread = config->readEntry("board-spread", QPointF(1.0,1.0));

  QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                         board_spread.y()*y + board_pos.y());
  mHint->setPosition(to);
  mHint->show();
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
  // Error?
  if (!mBoard) return -1;

  // Find which arrow the mouse is closest to. This way
  // all board scaling become irrelevant. An alteratnive
  // would be to calculate the position using board_pos and
  // board_spread.
  for (int i=0; i<7; i++)
  {
    PixmapSprite* arrow = mArrows.value(i);
    int width           = int(arrow->boundingRect().width());
    int relPos          = int(arrow->mapFromParent(QPointF(pos)).x());
    // Found matching arrow
    if (relPos > 0 && relPos<= width)
    {
      // kDebug() <<"Arrow maps pos " << pos << " to "<<i << endl;
      return i;
    }
  }

  // Nothing found
  return -1;
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

ScoreSprite* DisplayGame::score()
{
  assert(mScoreBoard != 0);
  return mScoreBoard;
}

#include "displaygame.moc"
