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
#include "kwin4view.h"

// Standard includes
#include <math.h>

// Qt includes
#include <QTimer>
#include <QColor>
#include <QEvent>
#include <QTime>

// KDE includes
#include <klocale.h>
#include <kdebug.h>
#include <kplayer.h>

// Local includes
#include "kwin4global.h"
#include "displayintro.h"
#include "displaygame.h"
#include "spritenotify.h"
#include "score.h"
#include "reflectiongraphicsscene.h"

// How many time measurements for average
#define MEASUREMENT_LIST_SIZE  50
// How many warnings until reflections are switched off
#define WARNING_MAX_COUNT      10


// Constructor for the view
KWin4View::KWin4View(int updateTime, 
                     const QSize &size, 
                     ReflectionGraphicsScene* scene,
                     ThemeManager* theme, 
                     QWidget* parent)
          : QGraphicsView(scene, parent)
{
  // Store attributes    
  mScene         = scene;
  mTheme         = theme;

  // We do not need scrolling so switch it off
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameStyle(QFrame::NoFrame);
  setCacheMode(QGraphicsView::CacheBackground);

  //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  setOptimizationFlags(QGraphicsView::DontClipPainter |
                       QGraphicsView::DontSavePainterState |
                       QGraphicsView::DontAdjustForAntialiasing );

  viewport()->setMouseTracking(true);
  setMouseTracking(true);

  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));


  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateAndAdvance()));
  timer->start(updateTime);

  // Game status
  mIsRunning = false;

  // Queue
  mThemeQueue.clear();
  
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
  mIntroDisplay  = 0;


  // Reflections
  mReflectionSprite = new QGraphicsPixmapItem(0);
  scene->addItem(mReflectionSprite);
  mReflectionSprite->setZValue(1000.0);
  mReflectionSprite->hide();

  // Debug                      
  mDisplayUpdateTime = updateTime;
  mFrameSprite = new QGraphicsTextItem(0, scene);
  mFrameSprite->setPos(QPointF(0.0, 0.0));
  mFrameSprite->setZValue(1000.0);
  if (global_debug > 0) mFrameSprite->show();
  else mFrameSprite->hide();

  
  // Skip the intro?
  if (!global_skip_intro)
  {
    mIntroDisplay = new DisplayIntro(scene, mTheme, this);
    connect(mIntroDisplay, SIGNAL(signalQuickStart(COLOUR,KGameIO::IOMode,KGameIO::IOMode,int)), 
            this, SIGNAL(signalQuickStart(COLOUR,KGameIO::IOMode,KGameIO::IOMode,int)));
    mIntroDisplay->start();
  }
}


// Destruct the view object
KWin4View::~KWin4View()
{
  if (mIntroDisplay) delete mIntroDisplay;
  if (mGameDisplay) delete mGameDisplay;
  if (global_debug>0) kDebug() << "TRACKING" << hasMouseTracking() << "and" << viewport()->hasMouseTracking();
  if (mFrameSprite) delete mFrameSprite;
  if (mReflectionSprite) delete mReflectionSprite;
}


// Advance and update canvas/scene
void KWin4View::updateAndAdvance()
{
  scene()->advance();
  // QGV takes care of updating dirty rects, no need to call update or the whole scene is dirtied and repainted
  // scene()->update();

  // Reflections need to be done in the view otherwise the update's go wrong
  if (mReflectionRect.width() >0 && mReflectionRect.height() > 0)
  {
    QImage image(mReflectionRect.width(), mReflectionRect.height(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter imagePainter(&image);
    // imagePainter.fillRect(image.rect(),QBrush(Qt::red));

    //Turn on all optimizations
    imagePainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, false);
    imagePainter.setClipping(true);
    imagePainter.setWorldMatrix(QMatrix(1.0,0.0,0.0,-1.0,0.0,image.height()));
    QRect source = QRect(mReflectionRect.x(),mReflectionRect.y()-image.height(),   image.width(), image.height());

    bool vis = mReflectionSprite->isVisible();
    mReflectionSprite->hide();
    dynamic_cast<ReflectionGraphicsScene*>(scene())->setBackground(false);
    scene()->render(&imagePainter, image.rect(), source, Qt::IgnoreAspectRatio);
    dynamic_cast<ReflectionGraphicsScene*>(scene())->setBackground(true);
    if (vis) mReflectionSprite->show();

     // Semi transparent
    imagePainter.setTransform(QTransform());
    imagePainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    imagePainter.drawImage(0,0,mGradientImage);
    
     // Set to sprite
    QPixmap pm = QPixmap::fromImage(image);
    mReflectionSprite->setPixmap(pm);
    mReflectionSprite->update();
  }

  
}


// Define the reflection
void KWin4View::setReflection(int x, int y, int width, int height)
{
  mReflectionRect = QRect(x,y,width,height);

  QPoint p1, p2;
  p2.setY(height);
  mGradient = QLinearGradient(p1, p2);
  mGradient.setColorAt(0, QColor(0, 0, 0, 100));
  mGradient.setColorAt(1, Qt::transparent);

  kDebug() << "Set reflection "<< x << " " << y << " " << width << " " << height ;

  mGradientImage = QImage(width, height, QImage::Format_ARGB32);
  mGradientImage.fill(Qt::transparent);
  QPainter p( &mGradientImage );
  p.fillRect(0,0,width, height, mGradient);
  p.end();

  mReflectionSprite->setPos(x,y);
  if (width >0 && height > 0)
  {
    mReflectionSprite->show();
  }
  else
  {
    mReflectionSprite->hide();
  }
}

// QGV drawItems function
void KWin4View::drawItems(QPainter* painter, int numItems, QGraphicsItem* items[], const QStyleOptionGraphicsItem options[])
{
  static bool first = true;
  static QTime time;
  int elapsed = time.elapsed();
  if (first) {elapsed = 0;first=false;}
  time.restart();
  QGraphicsView::drawItems(painter, numItems, items, options);

  // Time display
  mDrawTimes.append(elapsed);
  if (mDrawTimes.size() > MEASUREMENT_LIST_SIZE) mDrawTimes.removeFirst();
  double avg = 0.0;
  for (int i=0; i<mDrawTimes.size(); i++) avg += mDrawTimes[i];
  avg /= mDrawTimes.size();


  if (global_debug > 0)
     mFrameSprite->setPlainText(QString("Draw: %1 ms  Average %2 ms  Update: %3 ms").arg(elapsed).arg(int(avg)).arg(mDisplayUpdateTime));

}


// Stop intro display and init game display
void KWin4View::initGame(Score* scoreData)
{
  kDebug() << "KWin4View::initGame";

  // For better performance disable mouse tracking now
  viewport()->setMouseTracking(false);
  setMouseTracking(false);

  if (mIntroDisplay) delete mIntroDisplay;
  mIntroDisplay = 0;
  if (!mGameDisplay)
  {
     mGameDisplay = new DisplayGame(mScene, mTheme, this);
  }
  mGameDisplay->start();

  // Connect score and score sprite
  scoreData->setDisplay(mGameDisplay->score());

  mIsRunning = true;
}


// End the game
void  KWin4View::endGame()
{
  mIsRunning = false;
  mGameDisplay->displayEnd();
}


// Slot called by the framework when the view is resized.
void KWin4View::resizeEvent (QResizeEvent* e)
{
 if (global_debug > 2) kDebug() <<"RESIZE EVENT" << e->size() << "oldSize="<< e->oldSize();

  // Test to prevent double resizing
  // if (QWidget::testAttribute(Qt::WA_PendingResizeEvent))
  // {
  //   return;
  // }

  double diffW = double(e->oldSize().width()-e->size().width());
  double diffH = double(e->oldSize().height()-e->size().height());
  double delta = fabs(diffW) + fabs(diffH); 



  // Adapt the canvas size to the window size
  if (scene())
  {
    scene()->setSceneRect(0, 0, e->size().width(), e->size().height());
  }
  QSizeF size = QSizeF(e->size());

  // Rescale on minimum fitting aspect ratio either width or height limiting
  double width = 0.0;
  double aspect = size.width() / size.height();
  //if (aspect > mTheme->aspectRatio()) mTheme->rescale(int(e->size().height()*mTheme->aspectRatio()));
  //else mTheme->rescale(int(e->size().width()));

  if (aspect > mTheme->aspectRatio()) width = e->size().height()*mTheme->aspectRatio();
  else width = e->size().width();

  // Pixel rescale
  double oldScale = mTheme->getScale();
  resetTransform();
  if (width > oldScale) scale(double(width/oldScale), double(width/oldScale));
  mThemeQueue.prepend(int(width));
  if (global_debug > 2) kDebug() << "Quequed resize, aspect=" << aspect << "theme aspect="<< mTheme->aspectRatio();

  long queueDelay = 0;
  if (delta < 15) queueDelay = 750;
  else if (delta < 35) queueDelay = 500;

  QTimer::singleShot(queueDelay, this, SLOT(rescaleTheme()) );
}

// Rescale the theme (update theme SVG graphics) from the theme list
void KWin4View::rescaleTheme()
{
  if (mThemeQueue.size() == 0)
  {
    if (global_debug > 2) kDebug() << "***************** Swallowing rescale event ***********************";
    return;
  }

  QTime t;
  t.start();

  resetTransform();
  int width = mThemeQueue.first();
  if (global_debug > 2) kDebug() << "Theme queue size=" << mThemeQueue.size() << "Rescale width to" << width;
  mThemeQueue.clear();
  mTheme->rescale(width);

   if (global_debug > 2) kDebug() << "Time elapsed: "<< t.elapsed() << "ms";
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
    kDebug() <<" Kwin4View::TODO wrongPlayer";
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


// This slot is called when a key event is received. It then produces a
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
    kDebug() <<" Kwin4View::TODO wrongPlayer";
   // *eatevent=wrongPlayer(player,KGameIO::KeyIO);
    return;
  }

  // Create a valid game move (player id and movement position)
  qint32 move = code-Qt::Key_1;
  qint32 pl   = player->userId();
  stream << pl << move;
  *eatevent=true;
}



// Displays a move on the game board. 
void KWin4View::displayMove(int x, int y, int color, int xarrow, int colorarrow, int no, bool animation)
{
  mGameDisplay->displayArrow(xarrow, colorarrow);
  // animation only if no redo
  SpriteNotify* notify = mGameDisplay->displayPiece(x, y, color, no, animation);
  if (notify && animation)
  {
    QObject::disconnect(notify,SIGNAL(signalNotify(QGraphicsItem*,int)),
                        this,SLOT(moveDone(QGraphicsItem*,int)));
    connect(notify,SIGNAL(signalNotify(QGraphicsItem*,int)),
            this,SLOT(moveDone(QGraphicsItem*,int)));
  }
  mGameDisplay->displayHint(0,0,false);
}


// Display a star of the given sprite number
void KWin4View::displayStar(int x, int y, int no)
{
  mGameDisplay->displayStar(x, y, no);
}

// Display a hint on the board
void KWin4View::displayHint(int x, int y)
{
  mGameDisplay->displayHint(x, y, true);
}

// Slot called when a sprite animation move is done.
void KWin4View::moveDone(QGraphicsItem* /*item*/, int mode)
{
  emit signalMoveDone(mode);
}


bool KWin4View::viewportEvent ( QEvent * event ) 
{
  if (mIntroDisplay) mIntroDisplay->viewEvent(event);
  return QAbstractScrollArea::viewportEvent(event);
}

#include "kwin4view.moc"

