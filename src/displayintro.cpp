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
#include "displayintro.h"

// Local includes
#include "introsprite.h"
#include "pixmapsprite.h"
#include "buttonsprite.h"
#include "prefs.h"

// Standard includes
#include <math.h>

// Qt includes
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QPoint>
#include <QGraphicsView>
#include <QStringList>
#include <QEvent>
#include <QTimer>
#include <QMouseEvent>

// KDE includes
#include <klocale.h>
#include <kdebug.h>



// Constructor for the intro display
DisplayIntro::DisplayIntro(QGraphicsScene* scene, ThemeManager* theme, QGraphicsView* parent)
          : Themeable("introdisplay",theme), QObject(parent)
{
  // Choose a background color
  scene->setBackgroundBrush(QColor(0,0,128));
  
  // Store the theme manager and other attributes
  mLastMoveEvent = 0;
  mTheme         = theme;
  mScene         = scene;
  mView          = parent;

  // Storage of all sprites
  mSprites.clear();

  // Create all sprites used for intro
  for (int i=0; i<42; i++)
  {
    IntroSprite* sprite = new IntroSprite("intro_piece", mTheme, i, mScene);
    mSprites.append(sprite);
    if ((i/1)%2==0) sprite->setFrame(0);
    else  sprite->setFrame(1);
    sprite->setZValue(i);
    sprite->hide();
  }

  // Create board
  PixmapSprite* pixmap = new PixmapSprite("introboard", mTheme, 0, mScene);
  mSprites.append(pixmap);
  pixmap->show();

  // Create quicklaunch
  mQuickLaunch = new PixmapSprite("quicklaunch", mTheme, 0, mScene);
  mSprites.append(mQuickLaunch);
  mQuickLaunch->show();
  mTextQuicklaunch  = new QGraphicsTextItem(mQuickLaunch, scene);
  mTextQuicklaunch->setPlainText(i18nc("Name of quicklaunch field", "Quick Launch"));
  mTextQuicklaunch->show();
  mTextStartplayer  = new QGraphicsTextItem(mQuickLaunch, scene);
  mTextStartplayer->setPlainText(i18nc("Ask player who should start game", "Who starts?"));
  mTextStartplayer->show();
  mTextColor        = new QGraphicsTextItem(mQuickLaunch, scene);
  mTextColor->setPlainText(i18nc("Ask player which color he wants to play", "Your color?"));
  mTextColor->show();



  // Static decoration
  KConfigGroup config = thememanager()->config(id());
  QStringList deco = config.readEntry("decoration", QStringList());
  for (int i = 0; i < deco.size(); i++)
  {
    PixmapSprite* sprite = new PixmapSprite(deco.at(i), mTheme, i, mScene);
    mSprites.append(sprite);
    sprite->show();
  }


  // Color buttons
  mStartButton[0] = new ButtonSprite(false, "button0_start", mTheme, 0, mScene);
  mSprites.append(mStartButton[0]);
  mStartButton[0]->show();
  connect(mStartButton[0]->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));
  if (Prefs::startcolouryellow()) mStartButton[0]->setStatus(true);

   
  mStartButton[1] = new ButtonSprite(false, "button1_start", mTheme, 1, mScene);
  mSprites.append(mStartButton[1]);
  mStartButton[1]->show();
  connect(mStartButton[1]->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));
  if (Prefs::startcolourred()) mStartButton[1]->setStatus(true);

  mPlayerButton[0] = new ButtonSprite(false, "button0_color", mTheme,  2, mScene);
  mSprites.append(mPlayerButton[0]);
  mPlayerButton[0]->show();
  connect(mPlayerButton[0]->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));
  if (Prefs::input0mouse()|| Prefs::input0key()) mPlayerButton[0]->setStatus(true); 

  mPlayerButton[1] = new ButtonSprite(false, "button1_color", mTheme, 3, mScene);
  mSprites.append(mPlayerButton[1]);
  mPlayerButton[1]->show();
  connect(mPlayerButton[1]->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));
  if ((Prefs::input1mouse()|| Prefs::input1key()) &&
       !mPlayerButton[0]->status()) mPlayerButton[0]->setStatus(true); 

  // Start game buttons
  ButtonSprite* button = new ButtonSprite(true, "button_aieasy", mTheme,  10, mScene);
  mSprites.append(button);
  button->setText(i18nc("quick start button - player versus AI level easy", "Easy Game"));
  button->show();
  connect(button->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));

  button = new ButtonSprite(true, "button_ainormal", mTheme, 11, mScene);
  mSprites.append(button);
  button->setText(i18nc("quick start button - player versus AI level normal", "Normal Game"));
  button->show();
  connect(button->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));

  button = new ButtonSprite(true, "button_aihard", mTheme,  12, mScene);
  mSprites.append(button);
  button->setText(i18nc("quick start button - player versus AI level hard", "Hard Game"));
  button->show();
  connect(button->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));

  button = new ButtonSprite(true, "button_player", mTheme, 13, mScene);
  mSprites.append(button);
  button->setText(i18nc("quick start button - player versus player", "Two Player Game"));
  button->show();
  connect(button->notify(),SIGNAL(signalNotify(QGraphicsItem*,int)),
          this,SLOT(buttonPressed(QGraphicsItem*,int)));



  // Animation timer
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(advance()));

  // Redraw
  if (theme) theme->updateTheme(this);
  kDebug() << "CONSTRUCTOR DONE";
}


// One of the graphical sprite buttons was pressed. Item and its id are delivered
void DisplayIntro::buttonPressed(QGraphicsItem* item, int id)
{
  int status = id >> 8;
  int no = id & 0xff;
  //kDebug() << "Button" << no  << "pressed status="<<status;
  ButtonSprite* button = dynamic_cast<ButtonSprite*>(item);
  Q_ASSERT(button);
  if (button == mStartButton[0])
  {
    mStartButton[1]->setStatus(!button->status());
    Prefs::setStartcolouryellow(button->status());
    Prefs::setStartcolourred(!button->status());
  }
  if (button == mStartButton[1])
  {
    mStartButton[0]->setStatus(!button->status());
    Prefs::setStartcolouryellow(!button->status());
    Prefs::setStartcolourred(button->status());
  }
  if (button == mPlayerButton[0])
  {
    mPlayerButton[1]->setStatus(!button->status());
    Prefs::setStartcolouryellow(button->status());
    Prefs::setStartcolourred(!button->status());
  }
  if (button == mPlayerButton[1])
  {
    mPlayerButton[0]->setStatus(!button->status());
    Prefs::setStartcolouryellow(!button->status());
    Prefs::setStartcolourred(button->status());
  }

  // Start game from button
  if (no >=10 && no <= 13 && status == 1)
  {
    // Emit quick start with status (start player color, player color, AI level, two player?

    // Color
    COLOUR startPlayer;
    if (mStartButton[0]->status()) startPlayer = Yellow;
    else startPlayer = Red;

    // Inputs
    KGameIO::IOMode input0 = KGameIO::ProcessIO;
    KGameIO::IOMode input1 = KGameIO::ProcessIO;
    if (mPlayerButton[0]->status()) input0 = KGameIO::MouseIO;
    if (mPlayerButton[1]->status()) input1 = KGameIO::MouseIO;

    // Level
    int level = -1;
    if (no == 10) level = 2;
    else if (no == 11) level = 4;
    else if (no == 12) level = 6;
    if (no == 13) {input0=KGameIO::MouseIO;input1=KGameIO::MouseIO;}

    // Emit signal
    emit signalQuickStart(startPlayer, input0, input1, level);
  }
}


// Desctruct the intro and all sprites
DisplayIntro::~DisplayIntro()
{
  delete mTimer;
  while (!mSprites.isEmpty())
  {
    delete mSprites.takeFirst();
  }
}


// Master theme change function. Redraw the display
void DisplayIntro::changeTheme()
{
  kDebug() << "THEME CHANGE " << thememanager()->themefileChanged();
  // Measure time for resize
  QTime time;
  time.restart();

  // Retrieve theme data
  KConfigGroup config = thememanager()->config(id());
  
  // Retrieve background pixmap
  QString bgsvgid = config.readEntry("background-svgid");
  QPixmap pixmap  = thememanager()->getPixmap(bgsvgid, mScene->sceneRect().size().toSize());
  mScene->setBackgroundBrush(pixmap);
  mView->update();

  // Process quicklaunch sprite (could be own class...)
  double width  = mQuickLaunch->boundingRect().width();
  double height = mQuickLaunch->boundingRect().height();
 // Retrieve theme data
  config = thememanager()->config(mQuickLaunch->id());
  QPointF posQuickstart   = config.readEntry("posQuickstart", QPointF(1.0,1.0));
  QPointF posStartplayer  = config.readEntry("posStartplayer", QPointF(1.0,1.0));
  QPointF posColor        = config.readEntry("posColor", QPointF(1.0,1.0));

  // HEADER
  // Calculate proper font size
  double fontHeight = config.readEntry("fontHeightHeader", 1.0);
  fontHeight *= height;
  double fontWidth = config.readEntry("fontWidthHeader", 1.0);
  fontWidth *= width;
  
  // Retrieve font color
  QColor fontColor;
  fontColor = config.readEntry("fontColorHeader", QColor(Qt::white));

  // Create and set current font
  QFont font;
  font.setPixelSize(int(fontHeight));

  // Set font and color for all text items
  mTextQuicklaunch->setFont(font);   
  mTextQuicklaunch->setDefaultTextColor(fontColor);   
  mTextQuicklaunch->setTextWidth(fontWidth);   

  // Set position of sub sprites
  mTextQuicklaunch->setPos(posQuickstart.x()*width, posQuickstart.y()*height);



  // TEXT
  // Calculate proper font size
  fontHeight = config.readEntry("fontHeight", 1.0);
  fontHeight *= height;
  fontWidth = config.readEntry("fontWidth", 1.0);
  fontWidth *= width;
  
  // Retrieve font color
  fontColor = config.readEntry("fontColor", QColor(Qt::white));

  // Create and set current font
  font.setPixelSize(int(fontHeight));

  // Set font and color for all text items
  mTextStartplayer->setFont(font);   
  mTextStartplayer->setDefaultTextColor(fontColor);   
  mTextStartplayer->setTextWidth(fontWidth);   

  mTextColor->setFont(font);   
  mTextColor->setDefaultTextColor(fontColor);   
  mTextColor->setTextWidth(fontWidth);   

  // Set position of sub sprites
  mTextStartplayer->setPos(posStartplayer.x()*width, posStartplayer.y()*height);
  mTextColor->setPos(posColor.x()*width, posColor.y()*height);

  int elapsed = time.elapsed();
  kDebug() << "THEME CHANGE took " << elapsed << " ms";

  // Renew animation on theme change?
  if (thememanager()->themefileChanged())
  {  
    start(0);
  }
  else
  {
    delaySprites(elapsed);
  }
}


// Start the animation
void DisplayIntro::start(int delay)
{
  kDebug() << "START TIMER";
  // Do the timer
  mTimer->stop();
  mTimer->setSingleShot(true);
  mTimer->start(delay);
}


// Delay all sprite animation by the given time
void DisplayIntro::delaySprites(int duration)
{
  // Setup sprites
  for (int i = 0; i < mSprites.size(); ++i) 
  {
     // Use only intro sprites
    if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
    
    IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
    sprite->delayAnimation(duration);
  }
}


// Animation main routine to advance the aniamtion. Called
// by a timer
void DisplayIntro::advance()
{
  QTime time;
  time.restart();
  int duration = createAnimation(true);

  kDebug() << "ADVANCE: Restarting timer in " << (duration+5000) <<" creation time " << time.elapsed();
  mTimer->start(duration + 5000);  // [ms]
}


// Animation main routine to advance the aniamtion. Called
// by a timer
int DisplayIntro::createAnimation(bool restartTime)
{
  // Columns for the moves in the intro. Results in a drawn game
  // Note: Once could load the last played game here or so.
  static int moves[] = {
                        3,4,4,3,3,4,4,
                        1,3,5,3,3,6,6,
                        6,6,6,0,6,4,5,
                        5,2,2,5,5,1,4,
                        5,0,0,1,1,1,2,
                        2,2,2,1,0,0,0
                        };
  // How many moves to perform (size of moves array)
  static int maxMoves = 42;                      

  // Config data
  KConfigGroup config = thememanager()->config(id());
  QPointF piece0_pos    = config.readEntry("piece0-pos", QPointF(1.0,1.0));
  QPointF piece1_pos    = config.readEntry("piece1-pos", QPointF(1.0,1.0));
  double piece_spread   = config.readEntry("piece-spread", 0.1);
  QPointF board_pos     = config.readEntry("board-pos", QPointF(1.0,1.0));
  QPointF board_spread  = config.readEntry("board-spread", QPointF(1.0,1.0));
  double move_velocity  = config.readEntry("move-velocity", 0.1);

  // Local variables
  double dura, delay, rad;
  QPointF start, end;
  int maxDuration = 0;
  int addWaitTime = 0;

  // ============================================================================
  // Setup sprites
  for (int i = 0; i < mSprites.size(); ++i) 
  {
     // Use only intro sprites
    if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
    
    IntroSprite* sprite = (IntroSprite*)mSprites.at(i);
    int no = sprite->number();
    sprite->setZValue(no);
    sprite->hide();

    sprite->clearAnimation(restartTime);
  }

  // ============================================================================
  // First part of intro animation. Move sprites into window
  // Loop all sprites
  for (int i = 0; i < mSprites.size(); ++i) 
  {
      // Move only intro sprites
    if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
    IntroSprite* sprite = (IntroSprite*)mSprites.at(i);

    int no = sprite->number();
    {
      if (no%2==0)
      {
        start = QPointF(1.05, 0.5);
        end   = QPointF(piece0_pos.x() + piece_spread*no, piece0_pos.y());
        dura  = 3000.0;
        delay = 80.0*no;
        rad   = 0.1;
      }
      else
      {
        start = QPointF(-0.05, 0.5);
        end   = QPointF(piece1_pos.x() + piece_spread*(no-1), piece1_pos.y());
        dura  = 3000.0;
        delay = 80.0*(no-1);
        rad   = 0.1;
      }

      sprite->addPosition(start);
      sprite->addShow();
      sprite->addPause(int(delay));
      sprite->addLinear(start, (start+end)*0.5, int(dura/4.0));
      sprite->addCircle((start+end)*0.5, rad, int(dura/2.0));
      sprite->addLinear((start+end)*0.5, end, int(dura/4.0));
      if (sprite->animationDuration() > maxDuration) maxDuration = sprite->animationDuration();
    }
  }// end list loop
  // ============================================================================


  // ============================================================================
  // Second part of intro animation. Move sprites inwards
  int moveCnt  = 0;
  delay = 0.0;
  // Reset height
  int height[7];
  addWaitTime = maxDuration;
  for (int i=0; i<=6; i++) height[i] = 6;
  // Loop all sprites
  for (int i = mSprites.size()-1; i >= 0 ; --i) 
  {
      // Move only intro sprites
    if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
    IntroSprite* sprite = (IntroSprite*)mSprites.at(i);

    // No more moves left
    if (moveCnt >= maxMoves) continue;
    int ix = moves[moveCnt];
    moveCnt++;
  
    int iy = height[ix];
    height[ix]--;
    double x = board_pos.x() + ix*board_spread.x();
    double y = board_pos.y() + iy*board_spread.y();
    
    sprite->addWait(addWaitTime);
    sprite->addPause(3000);
    sprite->addPause(int(delay));
    AnimationCommand* anim = sprite->addRelativeManhatten(QPointF(x, y), move_velocity);

    delay += sprite->duration(anim);

    if (sprite->animationDuration() > maxDuration) maxDuration = sprite->animationDuration();
  }// end for
  // ============================================================================


  // ============================================================================
  // Third part of intro animation. Move sprites outwards
  addWaitTime = maxDuration;
      // Loop all sprites
  for (int i = 0; i < mSprites.size(); ++i) 
  {
      // Move only intro sprites
    if (mSprites.at(i)->type() != QGraphicsItem::UserType+1) continue;
    IntroSprite* sprite = (IntroSprite*)mSprites.at(i);

    int no = sprite->number();
    double xc = board_pos.x() + 3.0*board_spread.x();
    double yc = board_pos.y() + 3.5*board_spread.y();

    double x = xc + 1.50*cos(no/42.0*2.0*M_PI);
    double y = yc + 1.50*sin(no/42.0*2.0*M_PI);

    sprite->addWait(addWaitTime);
    sprite->addPause(8000);
    sprite->addRelativeLinear(QPointF(x,y), 800);

    if (sprite->animationDuration() > maxDuration) maxDuration = sprite->animationDuration();
  }// end for
  // ============================================================================

  return maxDuration;
}


// Find the sprite on the given poition (buttons)
QGraphicsItem* DisplayIntro::findSprite(QPoint pos)
{
   QGraphicsItem* found = 0; 

   for (int i = 0; i < mSprites.size(); ++i) 
   {
      QGraphicsItem* item = mSprites[i];
      if (!item) continue;
      if (!item->isVisible()) continue;
      // Map position to local
      QPointF p = item->mapFromScene (QPointF(pos) );
      // Take highest zValue item
      if (item->contains(p) && (!found || (found->zValue() < item->zValue()))) found = item;
   }
   return found;
}


// Handle view events and forward them to the sprites
void DisplayIntro::viewEvent(QEvent* event)
{
   // Only process some mouse events
   QEvent::Type type = event->type();
   if (type != QEvent::MouseButtonPress && 
       type != QEvent::MouseButtonRelease && 
       type != QEvent::MouseMove) return;

   // Cast items and find right sprite
   QMouseEvent* e = dynamic_cast<QMouseEvent *>(event);
   QGraphicsItem* item = findSprite(e->pos());

   // Which event type? Forward them to Button sprites only!
   switch (event->type()) 
   {
     case QEvent::MouseButtonPress:
         if (item!=0 && item->type() ==  QGraphicsItem::UserType+100)
           (dynamic_cast<ButtonSprite*>(item))->mousePressEvent(e);
     break;
     case QEvent::MouseButtonRelease:
         if (item!=0 && item->type() ==  QGraphicsItem::UserType+100)
           (dynamic_cast<ButtonSprite*>(item))->mouseReleaseEvent(e);
     break;
     case QEvent::MouseMove:
       if (item != mLastMoveEvent)
       {
         if (mLastMoveEvent!=0 && mLastMoveEvent->type() ==  QGraphicsItem::UserType+100)
         {
           (dynamic_cast<ButtonSprite*>(mLastMoveEvent))->hoverLeaveEvent(e);
         }
         if (item!=0 && item->type() ==  QGraphicsItem::UserType+100)
         {
           (dynamic_cast<ButtonSprite*>(item))->hoverEnterEvent(e);
         }
         mLastMoveEvent = item;
       }
     break;
     default:
     break;
   }
   return;

}

#include "displayintro.moc"
