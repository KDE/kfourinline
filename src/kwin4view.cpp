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

// Standard includes
#include <math.h>

// Qt includes
#include <QTimer>
#include <QColor>

// KDE includes
#include <klocale.h>
#include <kdebug.h>
#include <kplayer.h>

// Local includes
#include "kwin4view.h"
#include "displayintro.h"
#include "displaygame.h"


// Aspect ratio for the Scene in the window. The game is always displayed with this ratio.
#define VIEW_ASPECT_RATIO 1.6

// Our subclassed QGraphicsView paintEvent, see header file
void KWinGraphicsView::paintEvent(QPaintEvent* event)
{
    QPaintEvent* newEvent = new QPaintEvent(event->region().boundingRect());
    QGraphicsView::paintEvent(newEvent);
    delete newEvent;
}

// Constructor for the view
KWin4View::KWin4View(QSize size, int advancePeriod, QGraphicsScene* scene, ThemeManager* theme, QWidget* parent)
          : KWinGraphicsView(scene, parent)
{
  // Store attributes    
  mScene         = scene;
  mTheme         = theme;
  mAdvancePeriod = advancePeriod;

  // We do not need scrolling so switch it off
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setCacheMode(QGraphicsView::CacheBackground);

  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));


  // Update/advance view every 25ms
  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateAndAdvance()));
  timer->start(advancePeriod);

  // Game status
  mIsRunning = false;
  
  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  resize(size);
  scene->setSceneRect(0, 0, this->width(), this->height()); 
  adjustSize();

  // Interact with user
  setInteractive(true);
  
  // Scale theme
  mTheme->rescale(this->width());

  // Start with the intro display
  mGameDisplay  = 0;
  mIntroDisplay = new DisplayIntro(advancePeriod, scene, mTheme, this);
  mIntroDisplay->start();
}


// Destruct the view object
KWin4View::~KWin4View()
{
  if (mIntroDisplay) delete mIntroDisplay;
  if (mGameDisplay) delete mGameDisplay;
}


// Advance and update canvas/scene
void KWin4View::updateAndAdvance()
{
  scene()->advance();
  // QGV takes care of updating dirty rects, no need to call update or the whole scene is dirtied and repainted
  // scene()->update();
}


// Stop intro display and init game display
void KWin4View::initGame()
{
  if (mIntroDisplay) delete mIntroDisplay;
  mIntroDisplay = 0;
  if (!mGameDisplay)
  {
     mGameDisplay = new DisplayGame(mAdvancePeriod, mScene, mTheme, this);
  }
  mGameDisplay->start();

  mIsRunning = true;
}


// Slot called by the framework when the view is resized.
void KWin4View::resizeEvent (QResizeEvent* e)
{
  kDebug() << "++++ KWin4View::resizeEvent "<<e->size().width()<<" , "<< e->size().height() <<endl;
  // Adapt the canvas size to the window size
  if (scene())
  {
    scene()->setSceneRect(0, 0, e->size().width(), e->size().height());
  }
  QSizeF size = QSizeF(e->size());

  // Rescale on minimum fitting aspect ratio either width or height limiting
  double aspect = size.width() / size.height();
  if (aspect > VIEW_ASPECT_RATIO) mTheme->rescale(int(e->size().height()*VIEW_ASPECT_RATIO));
  else mTheme->rescale(int(e->size().width()));
}




// This slot is called when a mouse key is pressed. As the mouse is used as
// input for all players. It is called to generate a player move out of a mouse input, i.e.
// it converts a QMouseEvent into a move for the game. 
void KWin4View::mouseInput(KGameIO* input, QDataStream& stream, QMouseEvent* mouse, bool* eatevent)
{
  // Only react to mouse pressed not released
  if (mouse->type()   != QEvent::MouseButtonPress ) return;
  if (mouse->button() != Qt::LeftButton) return ;
  if (!mIsRunning) return;

  // Our player
  KPlayer* player=input->player();
  if (!player->myTurn())
  {
    kDebug() <<" Kwin4View::TODO wrongPlayer " << endl;
  //  *eatevent=wrongPlayer(player,KGameIO::MouseIO);
    return;
  }

  // Calculate movement position from mouse position
  int x = -1;
  if (mGameDisplay) x = mGameDisplay->mapMouseToMove(mouse->pos());
  if (x<0) return;

  // Create a game move (pl id and move coordinate)
  qint32 move = x;
  qint32 pl   = player->userId();
  stream << pl << move;
  *eatevent=true;
}


// This slot is called when a key event is received. It then prduces a
// valid move for the game.
// This is analogous to the mouse event only it is called when a key is
// pressed.
void KWin4View::keyInput(KGameIO* input, QDataStream& stream, QKeyEvent* key, bool* eatevent)
{
  // Ignore non running
  if (!mIsRunning) return;

  // Ignore non key press
  if (key->type() != QEvent::KeyPress) return ;

  // Check key code
  int code=key->key();
  if (code< Qt::Key_1 || code> Qt::Key_7) return ;
  

  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn())
  {
    kDebug() <<" Kwin4View::TODO wrongPlayer " << endl;
   // *eatevent=wrongPlayer(player,KGameIO::KeyIO);
    return;
  }

  // Create a valid game move (player id and movement position)
  qint32 move = code-Qt::Key_1;
  qint32 pl   = player->userId();
  stream << pl << move;
  *eatevent=true;
}

#include "kwin4view.moc"
