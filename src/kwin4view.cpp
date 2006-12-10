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
#include <QPoint>
#include <QFont>
#include <QTimer>
#include <QColor>

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kplayer.h>

// Local includes
#include "kwin4view.h"
#include "displayintro.h"
#include "displaygame.h"

#define VIEW_ASPECT_RATIO 1.6



// Constructor for the view
KWin4View::KWin4View(QSize size, int advancePeriod, QGraphicsScene* scene, QWidget* parent)
          : QGraphicsView(scene, parent)
{
  // We do not need scrolling so switch it off
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setCacheMode(QGraphicsView::CacheBackground);

    // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));
  mScene = scene;



  // Update/advance every 25ms
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateAndAdvance()));
  timer->start(advancePeriod);
  mAdvancePeriod = advancePeriod;

  mIsRunning = false;
  
  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  //setFixedSize(size);
  resize(size);

  // Set size and position of the view and the canvas:
  // they are reseized once a level is loaded
  scene->setSceneRect(0, 0, this->width(), this->height()); 
  adjustSize();

  setInteractive(true);

  // Create new theme manager
  // TODO: Move to main program
  mTheme = new ThemeManager(this->size().width(), this);

  mGameDisplay  = 0;
  mIntroDisplay = new DisplayIntro(advancePeriod, scene, mTheme, this);
  mIntroDisplay->start();
}

KWin4View::~KWin4View()
{
  //kDebug() << "~KWin4View"<<endl;
  if (mIntroDisplay) delete mIntroDisplay;
  if (mGameDisplay) delete mGameDisplay;
  if (mTheme) delete mTheme;
  //kDebug() << "~KWin4View done"<<endl;
}


// Advance and update canvas
void KWin4View::updateAndAdvance()
{
  scene()->advance();
  scene()->update();
}

// Stop intro and init game view
void KWin4View::initGame()
{
  kDebug() << "initGame" << endl;
  if (mIntroDisplay) delete mIntroDisplay;
  mIntroDisplay = 0;
  if (!mGameDisplay) mGameDisplay = new DisplayGame(mAdvancePeriod, mScene, mTheme, this);
  mGameDisplay->start();

  mIsRunning = true;
}


// Slot called by the framework when the window is
// resized.
void KWin4View::resizeEvent (QResizeEvent* e)
{
  kDebug() << "++++ KWin4View::resizeEvent "<<e->size().width()<<" , "<< e->size().height() <<endl;
  // Adapt the canvas size to the window size
  if (scene())
  {
    scene()->setSceneRect(0,0, e->size().width(), e->size().height());
  }
  QSizeF size = QSizeF(e->size());

  // Rescale on minimum fitting aspect ratio not width
  double aspect = size.width() / size.height();
  if (aspect > VIEW_ASPECT_RATIO) mTheme->rescale(e->size().height()*VIEW_ASPECT_RATIO);
  else mTheme->rescale(e->size().width());
}


// mouse click event
void KWin4View::mousePressEvent(QMouseEvent *ev)
{
//  emit mouseEvent(ev);
//  if (ev->button() != Qt::LeftButton) return ;

//  QPointF point = ev->pos();
//  emit signalLeftMousePress(point.toPoint());
}

// TODO
#define FIELD_SPACING 10
#define FIELD_SIZE_X 400
#define FIELD_SIZE_Y 400


/**
 * This slot is called when a mouse key is pressed. As the mouse is used as
 * input for all players
 * this slot is called to generate a player move out of a mouse input, i.e.
 * it converts a QMouseEvent into a move for the game. We do here some
 * simple nonsense and use the position of the mouse to generate
 * moves containing the keycodes
 */
void KWin4View::mouseInput(KGameIO *input,QDataStream &stream,QMouseEvent *mouse,bool *eatevent)
{
  // Only react to key pressed not released
  if (mouse->type() != QEvent::MouseButtonPress ) return;
  if (!mIsRunning) return;

  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn())
  {
    kDebug() <<" Kwin4View::TODO wrongPlayer " << endl;
  //  *eatevent=wrongPlayer(player,KGameIO::MouseIO);
    return;
  }

  if (mouse->button()!=Qt::LeftButton) return ;

  int x = -1;
  if (mGameDisplay) x = mGameDisplay->mapMouseToMove(mouse->pos());
  if (x<0) return;

  // Create a move
  qint32 move,pl;
  move=x;
  pl=player->userId();
  stream << pl << move;
  *eatevent=true;
  kDebug(12010) << "Mouse input "<<x<<" done..eatevent=true" << endl;
}



#include "kwin4view.moc"
