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
#include "displaygame.h"

// Standard includes
#include <math.h>
#include <assert.h>

// Qt includes
#include <QColor>
#include <QPixmap>
#include <QPoint>

// KDE includes
#include <klocale.h>
#include <kdebug.h>

// Local includes
#include "thememanager.h"
#include "piecesprite.h"
#include "pixmapsprite.h"
#include "scoresprite.h"
#include "spritenotify.h"
#include "kwin4doc.h"
#include "kwin4view.h"

#include "reflectiongraphicsscene.h"



// Constructor for the display
DisplayGame::DisplayGame(ReflectionGraphicsScene* scene, ThemeManager* theme, QGraphicsView* parent)
           : Themeable("gamedisplay",theme), QObject(parent)
{
  // Store arguments as attributes
  mScene         = scene;
  mView          = parent;
  mTheme         = theme;
  
    // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));

  // Clear storage of all sprites
  mSprites.clear();

  // Create piece sprites 
  for (int i=0; i<42; i++)
  {
    PieceSprite* sprite = new PieceSprite("piece", mTheme, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite" << "piece";
    mSprites.append(sprite);
    mPieces.append(sprite);
    sprite->hide();
  }

  // Create stars
  for (int i=0;i<4;i++)
  {
    PixmapSprite* sprite = new PixmapSprite("star", mTheme, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite" << "star";
    mSprites.append(sprite);
    mStars.append(sprite);
    sprite->hide();
  }

  // Create board
  mBoard = new PixmapSprite("board", mTheme, 0, mScene);
  if (!mBoard) kFatal() << "Cannot load sprite" << "board";
  mSprites.append(mBoard);
  mBoard->hide();

  // Create hint
  mHint = new PixmapSprite("hint", mTheme, 0, mScene);
  if (!mHint) kFatal() << "Cannot load sprite" << "hint";
  mSprites.append(mHint);
  mHint->hide();

  // Create Game Over
  // mGameOver = new PixmapSprite("gameover", mTheme, 0, mScene);
  // if (!mGameOver) kFatal() << "Cannot load sprite" << "gameover";
  // mSprites.append(mGameOver);
  // mGameOver->hide();

  // Create score board
  mScoreBoard = new ScoreSprite("scoreboard", mTheme, 0, mScene);
  if (!mScoreBoard) kFatal() << "Cannot load sprite" << "scoreboard";
  mSprites.append(mScoreBoard);
  mScoreBoard->hide();


  // Create movement indication arrows
  for (int i=0; i<7; i++)
  {
    PixmapSprite* arrow = new PixmapSprite("arrow", mTheme, i, mScene);
    if (!arrow) kFatal() << "Cannot load sprite" << "arrow";
    mSprites.append(arrow);
    mArrows.append(arrow);
    arrow->hide();
  }

  // Static decoration
  KConfigGroup config = thememanager()->config(id());
  QStringList deco = config.readEntry("decoration", QStringList());
  for (int i = 0; i < deco.size(); i++)
  {
    PixmapSprite* sprite = new PixmapSprite(deco.at(i), mTheme, i, mScene);
    if (!sprite) kFatal() << "Cannot load sprite" << deco.at(i);
    mSprites.append(sprite);
    sprite->show();
  }

  // Animation timer
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(advance()));

  // Redraw
  if (theme) theme->updateTheme(this);

}


// Destructor: clean up
DisplayGame::~DisplayGame()
{
  delete mTimer;
  while (!mSprites.isEmpty())
  {
    delete mSprites.takeFirst();
  }
}


// Called by thememanager when theme or theme geometry changes. Redraw and resize
// this display.
void DisplayGame::changeTheme()
{
  // Retrieve theme data
  KConfigGroup config = thememanager()->config(id());
  QPointF board_pos    = config.readEntry("board-pos", QPointF(1.0,1.0));
  QPointF arrow_pos    = config.readEntry("arrow-pos", QPointF(1.0,1.0));
  QPointF board_spread = config.readEntry("board-spread", QPointF(1.0,1.0));

  // Retrieve background pixmap
  QString bgsvgid = config.readEntry("background-svgid");
  QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, mScene->sceneRect().size().toSize());
  mScene->setBackgroundBrush(pixmap);
  QRectF pos = mBoard->sceneBoundingRect();
  

  // Move movement arrows
  for (int i=0; i<7; i++)
  {
    PixmapSprite* arrow = mArrows.value(i);
    QPointF to   = QPointF(board_spread.x()*i + arrow_pos.x(),
                           board_spread.y()*0 + arrow_pos.y());
    arrow->setPosition(to);

  }
  
  // Move piece sprites 
  for (int i=0; i<42; i++)
  {
    PieceSprite* piece = mPieces.value(i);
    int x = piece->logicalPos().x();
    int y = piece->logicalPos().y();
    QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                           board_spread.y()*y + board_pos.y());
    piece->setPosition(to);
  }
  
  // Move hint
  {
    int x = mHint->logicalPos().x();
    int y = mHint->logicalPos().y();
    QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                           board_spread.y()*y + board_pos.y());
    mHint->setPosition(to);
  }
  
  
  // Move stars
  for (int i=0;i<4;i++)
  {
    PixmapSprite* star = mStars.value(i);
    int x = star->logicalPos().x();
    int y = star->logicalPos().y();
    QPointF to  = QPointF(board_spread.x()*x    + board_pos.x(),
                          board_spread.y()*y    + board_pos.y());
    star->setPosition(to);
  }
  

  // Check whether the theme uses reflection handling
  if( config.readEntry("use-reflection", false))
  {
     dynamic_cast<KWin4View*>(mView)->setReflection((int)pos.x(), (int)(pos.y()+pos.height()), 
                          (int)(mScene->sceneRect().width() - pos.x()), (int)(pos.height()*0.3));
  }
  else
  {
    // Zero width disables the reflections
     dynamic_cast<KWin4View*>(mView)->setReflection(0,0,0,0);
  }
  
  mView->update();
}


// Start a new game. Initialize graphics.
void DisplayGame::start()
{
        // Run timer (unused)
  // mTimer->setSingleShot(true);
  // mTimer->start(0);


  // Show decoration
  mBoard->show();
  mScoreBoard->show();

  
  // Show movement arrows
  for (int i=0; i<7; i++)
  {
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

}


// Run game animation
void DisplayGame::advance()
{
         // Currently unused
}


// Display end game sprite
void DisplayGame::displayEnd()
{
  // assert(mGameOver != 0);
  // mGameOver->show();
}


// Set the movement indicator arrows above the game board
void DisplayGame::displayArrow(int x,int color)
{
  // Set all arrows back to frame 0
  for (int i=0; i<7; i++)
  {
    mArrows.value(i)->setFrame(0);
  }

  // Check for no color 
  if (color==Nobody)
  {
    return ;
  }

  // Make sure the frames are chosen properly
  if (color==Yellow) mArrows.value(x)->setFrame(1);
  else mArrows.value(x)->setFrame(2);
}


// Set a game HINT sprite
void DisplayGame::displayHint(int x, int y, bool show)
{
  // Invert height
  y=5-y;

  // Check for removal of the sprite
  if (!show)
  {
    mHint->hide();
    return;
  }

  // Retrieve theme data
  KConfigGroup config   = thememanager()->config(id());
  QPointF board_pos     = config.readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread  = config.readEntry("board-spread", QPointF(1.0,1.0));

  QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                         board_spread.y()*y + board_pos.y());
  mHint->setLogicalPos(QPoint(x,y));                       
  mHint->setPosition(to);
  mHint->show();
}


// Set a game piece, red or yellow or hidden depending on 'color'
SpriteNotify* DisplayGame::displayPiece(int x, int y, int color, int no, bool animation)
{
  // Invert height
  y=5-y;

  // Get piece
  PieceSprite *sprite = mPieces.value(no);
  assert(sprite != 0);

  // Check for removal of sprite
  if (color==Nobody)
  {
    sprite->hide();
    return 0;
  }

  // Retrieve theme data
  KConfigGroup config   = thememanager()->config(id());
  QPointF board_pos     = config.readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread  = config.readEntry("board-spread", QPointF(1.0,1.0));
  double velocity       = config.readEntry("move-velocity", 0.1);

  // Make sure the frames are ok
  int frame;
  if (color==Yellow) frame = 0;
  else frame = 1;


  // Just draw the sprites or show an movement animation?
  if (animation)
  {
    QPointF from = QPointF(board_spread.x()*x    + board_pos.x(),
                           board_spread.y()*(-1) + board_pos.y());
    QPointF to   = QPointF(board_spread.x()*x    + board_pos.x(),
                           board_spread.y()*y    + board_pos.y());
    sprite->setFrame(frame);
    sprite->startLinear(from, to, velocity);
    // For theme store (x,y) board coordinate 
    sprite->setLogicalPos(QPoint(x, y));
  }
  else
  {
    QPointF to   = QPointF(board_spread.x()*x + board_pos.x(),
                           board_spread.y()*y + board_pos.y());
    sprite->setFrame(frame);
    sprite->setPosition(to);
    // For theme store (x,y) board coordinate 
    sprite->setLogicalPos(QPoint(x, y));
  }

  sprite->show();
  return sprite->notify();
}


// Draw Star Sprites as winning indicator
void DisplayGame::displayStar(int x,int y,int no)
{
  // Invert height
  y=5-y;
  PixmapSprite* star = mStars.value(no-1);
  assert(star != 0);

  // Retrieve theme data
  KConfigGroup config  = thememanager()->config(id());
  QPointF board_pos    = config.readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread = config.readEntry("board-spread", QPointF(1.0,1.0));

  QPointF pos  = QPointF(board_spread.x()*x    + board_pos.x(),
                         board_spread.y()*y    + board_pos.y());
  star->setAnimation(true);
  star->setLogicalPos(QPoint(x,y));
  star->setPosition(pos);
  star->show();
}


// Return the mouse mapped to the board or bar item so that a
// move 0..6 is generated. -1 means an illegal position.
int DisplayGame::mapMouseToMove(const QPoint &pos)
{
  // Error?
  if (!mBoard) return -1;

  // Find which arrow the mouse is closest to. This way
  // all board scaling become irrelevant. An alternative
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
      return i;
    }
  }

  // Nothing found
  return -1;
}



// Retrieve the score sprite.
ScoreSprite* DisplayGame::score()
{
  return mScoreBoard;
}


#include "displaygame.moc"
