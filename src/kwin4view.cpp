/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kwin4view.h"

// own
#include "displayintro.h"
#include "displaygame.h"
#include "spritenotify.h"
#include "score.h"
#include "reflectiongraphicsscene.h"
#include "kfourinline_debug.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kplayer.h>
// Qt
#include <QColor>
#include <QEvent>
#include <QElapsedTimer>
// Std
#include <cmath>


// How many time measurements for average
#define MEASUREMENT_LIST_SIZE  50
// How many warnings until reflections are switched off
#define WARNING_MAX_COUNT      5
// How many milliseconds rounding error
#define MEASUREMENT_ROUNDING_ERROR 5


// Constructor for the view
KWin4View::KWin4View(int updateTime, 
                     const QSize &size, 
                     ReflectionGraphicsScene* scene,
                     ThemeManager* theme, 
                     QWidget* parent)
          : Themeable(QStringLiteral("theview"), theme), QGraphicsView(scene, parent)
{
  // Store attributes    
  mScene             = scene;
  mTheme             = theme;
  mDefaultUpdateTime = updateTime;
  mSlowDownFactor    = 1.0;
  mSlowCnt           = 0;
  mReflectPhase      = 0; 

  // We do not need scrolling so switch it off
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameStyle(QFrame::NoFrame);
  setCacheMode(QGraphicsView::CacheBackground);
  //setAlignment(Qt::AlignHCenter);

  //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  setOptimizationFlags(
                       QGraphicsView::DontSavePainterState |
                       QGraphicsView::DontAdjustForAntialiasing );

  viewport()->setMouseTracking(true);
  setMouseTracking(true);

  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));


  mTimer = new QTimer(this);
  connect(mTimer, &QTimer::timeout, this, &KWin4View::updateAndAdvance);
  mTimer->start(mDefaultUpdateTime);

  // Game status
  mIsRunning = false;

  // Queue
  mThemeQueue.clear();
  mThemeOffset.clear();
  
  // Set size and position of the view and the canvas:
  // they are resized once a level is loaded
  resize(size);
  scene->setSceneRect(0, 0, this->width(), this->height()); 
  adjustSize();

  // Interact with user
  setInteractive(true);
  
  // Scale theme
  mTheme->rescale(this->width(), QPoint(0,0));

  // Start with the intro display
  mGameDisplay  = nullptr;
  mIntroDisplay  = nullptr;


  // Reflections
  mReflectionSprite = new QGraphicsPixmapItem();
  scene->addItem(mReflectionSprite);
  mReflectionSprite->setZValue(1000.0);
  mReflectionSprite->hide();

  // Debug
  mFrameSprite = new QGraphicsTextItem();
  scene->addItem(mFrameSprite);
  mFrameSprite->setPos(QPointF(0.0, 0.0));
  mFrameSprite->setZValue(1000.0);
  if (global_debug > 0) mFrameSprite->show();
  else mFrameSprite->hide();

  
  // Skip the intro?
  if (!global_skip_intro)
  {
    mIntroDisplay = new DisplayIntro(scene, mTheme, this);
    connect(mIntroDisplay, &DisplayIntro::signalQuickStart, this, &KWin4View::signalQuickStart);
    mIntroDisplay->start();
  }
}


// Destruct the view object
KWin4View::~KWin4View()
{
  delete mIntroDisplay;
  delete mGameDisplay;
  if (global_debug>0) qCDebug(KFOURINLINE_LOG) << "TRACKING" << hasMouseTracking() << "and" << viewport()->hasMouseTracking();
  delete mFrameSprite;
  delete mReflectionSprite;
}

// Main themeable function. Called for any theme change. 
void KWin4View::changeTheme()
{
  if (global_debug > 0) qCDebug(KFOURINLINE_LOG) << "CHANGE THEME IN VIEW ... resetting slow counter";
  mDrawTimes.clear();
  mSlowDownFactor = 1.0;
  mSlowCnt = 0;
  mTimer->setInterval(int(mDefaultUpdateTime*mSlowDownFactor)); 
}


// Advance and update canvas/scene
void KWin4View::updateAndAdvance()
{
  // Time measurement (maybe remove static at some point)  
  static bool first = true;
  static QElapsedTimer time;
  int elapsed = time.elapsed();
  if (first) {elapsed = 0;first=false;}
  time.restart();

  // Time display
  mDrawTimes.append(elapsed);
  if (mDrawTimes.size() > MEASUREMENT_LIST_SIZE) mDrawTimes.removeFirst();
  double avg = 0.0;
  for (int i=0; i<mDrawTimes.size(); i++) avg += mDrawTimes[i];
  avg /= mDrawTimes.size();

  // Set debug sprite
  if (global_debug > 0)
  {
     mFrameSprite->setPlainText(QStringLiteral("CurrentUpdate: %1 ms  AverageUpdate%2 ms  DefaultUpdate: %3*%4 ms").
                  arg(elapsed).arg(int(avg)).arg(mDefaultUpdateTime).arg(mSlowDownFactor));
  }
     
  
  // Dynamic update of the graphics advance and update speed
  if (mDrawTimes.size() == MEASUREMENT_LIST_SIZE && 
      avg > mDefaultUpdateTime*mSlowDownFactor+MEASUREMENT_ROUNDING_ERROR)
  {
    mSlowCnt++;
    qCDebug(KFOURINLINE_LOG) << "Warning " << mSlowCnt << " avg=" << avg;
    mDrawTimes.clear();
    if (mSlowCnt > WARNING_MAX_COUNT)
    {
      mSlowDownFactor = double(MEASUREMENT_ROUNDING_ERROR+avg)/double(mDefaultUpdateTime);
      mSlowCnt = 0;
      mTimer->setInterval(int(mDefaultUpdateTime*mSlowDownFactor)); 

      qCDebug(KFOURINLINE_LOG) << "SLOW COMPUTER WARNING: Decreasing graphics update speed "
               << mDefaultUpdateTime*mSlowDownFactor<<"ms. Maybe switch off reflections.";
    }
  }

    
  // Scene advance  
  scene()->advance();
  // QGV takes care of updating dirty rects, no need to call update or the whole scene is dirtied and repainted
  // scene()->update();


  // ====================================================================================
  // Reflections need to be done in the view otherwise the update's go wrong
  if (mReflectionRect.width() >0 && mReflectionRect.height() > 0)
  {
    // Draw reflection in steps to save processing power
    if (mReflectPhase == 0)
    {
      mReflectImage = QImage(mReflectionRect.width(), mReflectionRect.height(), QImage::Format_ARGB32);
      mReflectImage.fill(Qt::transparent);
      QPainter imagePainter(&mReflectImage);
      // imagePainter.fillRect(image.rect(),QBrush(Qt::red));

      //Turn on all optimizations
      imagePainter.setRenderHints(QPainter::Antialiasing |
                                  QPainter::TextAntialiasing |
                                  QPainter::SmoothPixmapTransform, false);
      imagePainter.setClipping(true);
      imagePainter.setWorldTransform(QTransform(1.0,0.0,0.0,-1.0,0.0,mReflectImage.height()));
      QRect source = QRect(mReflectionRect.x(),mReflectionRect.y()-mReflectImage.height(), 
                           mReflectImage.width(), mReflectImage.height());

      bool vis = mReflectionSprite->isVisible();
      mReflectionSprite->hide();
      dynamic_cast<ReflectionGraphicsScene*>(scene())->setBackground(false);
      scene()->render(&imagePainter, mReflectImage.rect(), source, Qt::IgnoreAspectRatio);
      dynamic_cast<ReflectionGraphicsScene*>(scene())->setBackground(true);
      if (vis) mReflectionSprite->show();
      mReflectPhase = 1;
    }
    // Draw reflection in steps to save processing power
    else if (mReflectPhase == 1)
    {
       // Semi transparent
      QPainter imagePainter(&mReflectImage);
      imagePainter.setTransform(QTransform());
      imagePainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
      imagePainter.drawImage(0,0,mGradientImage);
      mReflectPhase = 2;
    }
    // Draw reflection in steps to save processing power
    else if (mReflectPhase == 2)
    {
       // Set to sprite
      QPixmap pm = QPixmap::fromImage(mReflectImage);
      mReflectionSprite->setPixmap(pm);
      mReflectionSprite->update();
      mReflectPhase = 0;
    }
  }
  // ====================================================================================

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

  qCDebug(KFOURINLINE_LOG) << "Set reflection "<< x << " " << y << " " << width << " " << height ;

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


// QGV drawItems function (for debug time measurements)
void KWin4View::drawItems(QPainter* painter, int numItems, QGraphicsItem* items[], const QStyleOptionGraphicsItem options[])
{
  QGraphicsView::drawItems(painter, numItems, items, options);
}


// Stop intro display and init game display
void KWin4View::initGame(Score* scoreData)
{
  qCDebug(KFOURINLINE_LOG) << "KWin4View::initGame";

  // For better performance disable mouse tracking now
  viewport()->setMouseTracking(false);
  setMouseTracking(false);

  delete mIntroDisplay;
  mIntroDisplay = nullptr;
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
 if (global_debug > 2) qCDebug(KFOURINLINE_LOG) <<"RESIZE EVENT" << e->size() << "oldSize="<< e->oldSize();

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
  QPoint offset;

  // Scale width:
  // Ideal size would be: 'width'*'height'
  // Offset in width is (e->size().width()-width)/2, offset in height is zero
  if (aspect > mTheme->aspectRatio())
  {
     width = e->size().height()*mTheme->aspectRatio(); 
     offset = QPoint(int((e->size().width()-width)/2.0), 0);
  }
  // Scale height:
  // 'height' = width/mTheme->aspectRatio()
  // Ideal size would be: 'width'*'height': 
  // Offset in height is (e->size().height()-width/mTheme->aspectRatio())/2, offset in width is zero
  else
  {
     width = e->size().width();  // Scale height
     offset = QPoint(0, int((e->size().height()-width/mTheme->aspectRatio())/2.0));
  }
  
  
  // Pixel rescale
  double oldScale = mTheme->getScale();

  //resetTransform();
  QTransform transform;
  if (width > oldScale) 
  {
    transform.scale(double(width/oldScale), double(width/oldScale));
  }
  setTransform(transform);
  
  mThemeQueue.prepend(int(width));
  mThemeOffset.prepend(offset);
  if (global_debug > 2) qCDebug(KFOURINLINE_LOG) << "Quequed resize, aspect=" << aspect << "theme aspect="<< mTheme->aspectRatio();

  long queueDelay = 0;
  if (delta < 15) queueDelay = 750;
  else if (delta < 35) queueDelay = 500;

  QTimer::singleShot(queueDelay, this, &KWin4View::rescaleTheme );
}


// Rescale the theme (update theme SVG graphics) from the theme list
void KWin4View::rescaleTheme()
{
  if (mThemeQueue.size() == 0)
  {
    if (global_debug > 2) qCDebug(KFOURINLINE_LOG) << "***************** Swallowing rescale event ***********************";
    return;
  }

  QElapsedTimer t;
  t.start();

  resetTransform();
  
  int width     = mThemeQueue.first();
  QPoint offset = mThemeOffset.first();
  if (global_debug > 2) qCDebug(KFOURINLINE_LOG) << "Theme queue size=" << mThemeQueue.size() << "Rescale width to" << width 
                                 << " offset " << offset;
  mThemeQueue.clear();
  mThemeOffset.clear();
  mTheme->rescale(width, offset);

   if (global_debug > 2) qCDebug(KFOURINLINE_LOG) << "Time elapsed: "<< t.elapsed() << "ms";
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
    // qCDebug(KFOURINLINE_LOG) <<" Kwin4View::TODO wrongPlayer";
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
    //qCDebug(KFOURINLINE_LOG) <<" Kwin4View::TODO wrongPlayer";
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
    QObject::disconnect(notify,&SpriteNotify::signalNotify,
                        this,&KWin4View::moveDone);
    connect(notify, &SpriteNotify::signalNotify, this, &KWin4View::moveDone);
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
  Q_EMIT signalMoveDone(mode);
}


bool KWin4View::viewportEvent ( QEvent * event ) 
{
  if (mIntroDisplay) mIntroDisplay->viewEvent(event);
  return QGraphicsView::viewportEvent(event);
}



