/***************************************************************************
                          kwin4view.cpp  -  View of the kwin4 program
                             -------------------
    begin                : Sun Mar 26 12:50:12 CEST 2000
    copyright            : (C) |1995-2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kwin4view.h"

#include <stdio.h>
#include <math.h>

#include <kconfig.h>
#include <klocale.h>
#include <kapplication.h>
#include <kstandarddirs.h>

#include "kwin4doc.h"
#include "scorewidget.h"
#include "statuswidget.h"
#include "kspritecache.h"

#include <qlabel.h>
#include <qlcdnumber.h>

#define COL_STATUSLIGHT  QColor(210,210,255)
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)

#define COL_STATUSBORDER black
#define COL_PLAYER       QColor(255,255,0)
#define COL_RED          red
#define COL_YELLOW       yellow

class KIntroMove : public KSpriteMove
{
  public:
  KIntroMove() : KSpriteMove() {mode=0;cnt=0;}
  virtual bool spriteMove(double tx,double ty,KSprite *sp) 
  {
    double sign=1.0;
    if (!dir) sign=-1.0;
    if (mode==0)
    {
      cnt++;
      if (sp->x()<120.0)
      {
        sp->spriteMove(tx,ty);
        return true;
      }
      else 
      {
        cnt=0;
        mode=1;
        cx=sp->x();
        cy=sp->y()-sign*50;
      }
    }
    if (mode==1)
    {
      if (cnt<360)
      {
        double x,y;
        x=cx+50*cos((sign*90.0-sign*(double)cnt)/180.0*M_PI);
        y=cy+50*sin((sign*90.0-sign*(double)cnt)/180.0*M_PI);
        sp->move(x,y);
        cnt+=5;
      }
      else 
      {
        cnt=0;
        mode=2;
      }
    }
    if (mode==2)
    {
      return sp->spriteMove(tx,ty);
    }

    return true;
  }

  void setDir(bool d) {dir=d;}

private:

  bool dir;
  int mode;
  int cnt;
  double cx,cy;

};

Kwin4View::Kwin4View(Kwin4Doc *theDoc, QWidget *parent, const char *name)
        : QCanvasView(0,parent, name), doc(theDoc)
{
  mLastArrow=-1;


  // localise data file
  QString file="kwin4/grafix/default/grafix.rc";
  QString mGrafix=kapp->dirs()->findResourceDir("data",file);
  if (mGrafix.isNull())
    mGrafix="grafix/default/";
  else
    mGrafix+="kwin4/grafix/default/";
  if (global_debug>3)
    kdDebug(12010) << "Localised grafix dir " << mGrafix << endl;

  // Allow overriding of the grafix directory
  // This is a cheap and dirty way for theming
  kapp->config()->setGroup("Themes");
  mGrafix = kapp->config()->readPathEntry("grafixdir", mGrafix);

  
  setVScrollBarMode(AlwaysOff);
  setHScrollBarMode(AlwaysOff);

  //setBackgroundMode(PaletteBase);
  setBackgroundColor(QColor(0,0,128));

  mCanvas=new QCanvas(parent);
  mCanvas->resize(parent->width(),parent->height()); 
  mCanvas->setDoubleBuffering(true);
  mCanvas->setBackgroundColor(QColor(0,0,128));
  setCanvas(mCanvas);

  mCache=new KSpriteCache(mGrafix,this);
  mCache->setCanvas(mCanvas);
  KConfig *config=mCache->config();

  QPoint pnt;
  config->setGroup("game");

  pnt=config->readPointEntry("scorewidget");
  mScoreWidget=new ScoreWidget(viewport());
  addChild(mScoreWidget);
  mScoreWidget->move(pnt);

  pnt=config->readPointEntry("statuswidget");
  mStatusWidget=new StatusWidget(this);
  mStatusWidget->move(pnt);
  QPalette pal;
  pal.setColor(QColorGroup::Light, COL_STATUSLIGHT);
  pal.setColor(QColorGroup::Mid, COL_STATUSFIELD);
  pal.setColor(QColorGroup::Dark, COL_STATUSDARK);
  mStatusWidget->setPalette(pal); 
  mStatusWidget->setBackgroundColor(COL_STATUSFIELD);

  mStatusWidget->wins->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->draws->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->loses->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->num->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->bk->setBackgroundColor(COL_STATUSFIELD);

  mStatusWidget->p1_name->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p1_w->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p1_d->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p1_l->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p1_n->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p1_b->setBackgroundColor(COL_STATUSFIELD);

  mStatusWidget->p2_name->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p2_w->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p2_d->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p2_l->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p2_n->setBackgroundColor(COL_STATUSFIELD);
  mStatusWidget->p2_b->setBackgroundColor(COL_STATUSFIELD);

  mScoreWidget->hide();
  mStatusWidget->hide();

  move(0,0);
  adjustSize();

  initView(false);  
}

void Kwin4View::initView(bool deleteall)
{
  KSprite *sprite=0;
  // mCanvas->setAdvancePeriod(period);
  mCanvas->setAdvancePeriod(15);

  KConfig *config=mCache->config();
  config->setGroup("game");
  mSpreadX=config->readNumEntry("spread_x",0);
  mSpreadY=config->readNumEntry("spread_y",0);
  //kdDebug(12010) << "Spread : x=" << mSpreadX << " y=" << mSpreadY << endl;

  QPixmap *pixmap=loadPixmap("background.png");
  if (pixmap)
    mCanvas->setBackgroundPixmap(*pixmap);
  else
    mCanvas->setBackgroundColor(QColor(0,0,128));
  delete pixmap;

  if (doc->gameStatus()==KGame::Intro)
  {
    mScoreWidget->hide();
    mStatusWidget->hide();
    drawIntro(deleteall);
  }
  else
  {
    // TODO in start functions to distinguish from intro
    kdDebug(12010) << "Clearing board" <<endl;
    drawBoard(deleteall);
    mScoreWidget->show();
    mStatusWidget->show();
    // Hide pieces in any case
    for (int i=0;i<42;i++)
    {
      sprite=(KSprite *)(mCache->getItem("piece",i));
      if (sprite)
      {
        introMoveDone(sprite,0 );
        sprite->hide();
      }
    }
    hideIntro();
  }

  // Hide stars in any case
  for (int i=0;i<8;i++)
  {
    sprite=(KSprite *)(mCache->getItem("star",i));
    if (sprite) sprite->hide();
  }
  // Hide GameOver in any case
  sprite=(KSprite *)(mCache->getItem("gameover",1));
  if (sprite) sprite->hide();


  // Hide hint in any case
  setHint(0,0,false);

  // Clear error text
  clearError();
}

QPixmap *Kwin4View::loadPixmap(QString name)
{
  if (!mCache)
    return 0;
  return mCache->loadPixmap(name);
}

/**
 * Called by the doc/app to signal the end of the game
 */
void Kwin4View::EndGame()
{
  KSprite *sprite;
  sprite=(KSprite *)(mCache->getItem("gameover",1));
  KConfig *config=mCache->config();
  int dest=config->readNumEntry("destY",150);
  int src=config->readNumEntry("y",0);
  //kdDebug(12010) << "MOVING gameover to " << dest << endl;

  if (sprite)
  {
    sprite->show();
    sprite->setY(src);
    sprite->moveTo(sprite->x(),dest);
  }
}

/**
 * Draw Sprites
 */
void Kwin4View::drawStar(int x,int y,int no)
{
  int dx,dy;
  y=5-y;
  KSprite *piece=(KSprite *)(mCache->getItem("piece",0));
  if (piece)
  {
    dx=piece->width();
    dy=piece->height();
  }
  else
  {
    dx=0;
    dy=0;
  }

  KSprite *sprite=(KSprite *)(mCache->getItem("star",no));
  //kdDebug(12010) << " setStar("<<x<<","<<y<<","<<no<<") sprite=" << sprite<<endl;
  if (sprite)
  {
    sprite->move(dx/2-sprite->width()/2+x*(dx+mSpreadX)+mBoardX,
                 dy/2-sprite->height()/2+y*(dy+mSpreadY)+mBoardY);
    sprite->show();
    sprite->setAnimation(0);
  }
}

void Kwin4View::hideIntro()
{
  KSprite *sprite=0;
  sprite=(KSprite *)(mCache->getItem("about",1));
  if (sprite) sprite->hide();
  sprite=(KSprite *)(mCache->getItem("win4about",1));
  if (sprite) sprite->hide();
  sprite=(KSprite *)(mCache->getItem("win4about",2));
  if (sprite) sprite->hide();

  QCanvasText *text;
  text=(QCanvasText *)(mCache->getItem("intro1",1));
  if (text) text->hide();
  text=(QCanvasText *)(mCache->getItem("intro2",1));
  if (text) text->hide();
  text=(QCanvasText *)(mCache->getItem("intro3",1));
  if (text) text->hide();
}

void Kwin4View::drawIntro(bool /*remove*/)
{
  KSprite *sprite=0;
  // background
  sprite=(KSprite *)(mCache->getItem("about",1));
  if (sprite) sprite->show();

  sprite=(KSprite *)(mCache->getItem("win4about",1));
  if (sprite) sprite->show();
  sprite=(KSprite *)(mCache->getItem("win4about",2));
  if (sprite)
  {
    KConfig *config=mCache->config();
    double dest=config->readDoubleNumEntry("x2",250.0);
    sprite->setX(dest);
    sprite->show();
  }

  QCanvasText *text;
  text=(QCanvasText *)(mCache->getItem("intro1",1));
  if (text)
  {
    text->setText(i18n("1. intro line, welcome to win4","Welcome")); 
    text->show();
  }
  text=(QCanvasText *)(mCache->getItem("intro2",1));
  if (text)
  {
    text->setText(i18n("2. intro line, welcome to win4","to")); 
    text->show();
  }
  text=(QCanvasText *)(mCache->getItem("intro3",1));
  if (text)
  {
    text->setText(i18n("3. intro line, welcome to win4","KWin4")); 
    text->show();
  }
  // text
  
  // animation
  for (int no=0;no<42;no++)
  {
    sprite=(KSprite *)(mCache->getItem("piece",no));
    if (sprite)
    {
      KIntroMove *move=new KIntroMove;
      connect(sprite->createNotify(),SIGNAL(signalNotify(QCanvasItem *,int)),
              this,SLOT(introMoveDone(QCanvasItem *,int)));
      sprite->setMoveObject(move);
      if (no%2==0)
      {
        sprite->move(0-20*no,0);
        sprite->moveTo(150+2*no,105+4*no);
        move->setDir(true);
        sprite->setFrame((no/2)%2);
      }
      else
      {
        sprite->move(0-20*no,height());
        sprite->moveTo(340-2*(no-1),105+4*(no-1));
        move->setDir(false);
        sprite->setFrame(((no-1)/2)%2);
      }
      // Increase the nz coord for consecutive peices
      // to allow proper intro
      // Carefule: The number must be more then the
      // z coord of [empty] and less than [empty2]
      sprite->setZ(sprite->z()+no/2);
      // kdDebug(12010) << "Z("<<no<<")="<<sprite->z()<<endl;
      sprite->show();
    }
  }
}

/** 
 * received after the movment of an intro sprite is finished
 **/
void Kwin4View::introMoveDone(QCanvasItem *item,int )
{
  KSprite *sprite=(KSprite *)item;
  sprite->deleteNotify();
  KIntroMove *move=(KIntroMove *)sprite->moveObject();
  if (move)
  {
    delete move;
    sprite->setMoveObject(0);
  }
}

void Kwin4View::drawBoard(bool remove)
{
  KSprite *sprite=0;
  KSprite *board=0;
  int x,y;

  // Board
  // TODO: Without number as it is unique item
  board=(KSprite *)(mCache->getItem("board",1));
  if (board)
  {
    if (remove) board->hide();
    else if (!board->isVisible()) board->show();
    mBoardX=(int)(board->x());
    mBoardY=(int)(board->y());
  }
  else
  {
    mBoardX=0;
    mBoardY=0;
  }
      //kdDebug(12010) << "Board X=" << mBoardX << " y="<<mBoardY<<endl;

  // Arrows
  for (x=0;x<7;x++)
  {
    sprite=(KSprite *)(mCache->getItem("arrow",x));
    if (sprite)
    {
      sprite->setFrame(0);
      sprite->setX(x*(sprite->width()+mSpreadX)+mBoardX);
      if (remove) sprite->hide();
      else if (!sprite->isVisible()) sprite->show();
    }
  }/* end arrows */

  // Field
  for (y=5;y>=0;y--)
  {
    for (x=0;x<7;x++)
    {
      // Lower layer
      sprite=(KSprite *)(mCache->getItem("empty2",x+7*y));
      if (sprite)
      {
        sprite->move(x*(sprite->width()+mSpreadX)+mBoardX,
                      y*(sprite->height())+mBoardY);
        if (remove) sprite->hide();
        else if (!sprite->isVisible()) sprite->show();
      }
      // upper layer
      sprite=(KSprite *)(mCache->getItem("empty",x+7*y));
      if (sprite)
      {
        sprite->move(x*(sprite->width()+mSpreadX)+mBoardX,
                      y*(sprite->height())+mBoardY);
        if (remove) sprite->hide();
        else if (!sprite->isVisible()) sprite->show();
      }
    }
  }/* end field */
}

void Kwin4View::setSprite(int no, int x, int col, bool enable)
{
  KSprite *sprite;
  sprite=(KSprite *)(mCache->getItem("piece",no));
  if (sprite) sprite->setVisible(enable);
  setArrow(x,col);
}

void Kwin4View::setHint(int x,int y,bool enabled)
{
  KSprite *sprite;
  sprite=(KSprite *)(mCache->getItem("hint",1));
  y=5-y;
  if (sprite)
  {
    if (enabled)
    {
      sprite->move(x*(sprite->width()+mSpreadX)+mBoardX,
                   y*(sprite->height()+mSpreadY)+mBoardY);
    }
    sprite->setVisible(enabled);
  }
}

void Kwin4View::setPiece(int x,int y,int color,int no,bool animation)
{
  KSprite *sprite=0;

  y=5-y;

  sprite=(KSprite *)(mCache->getItem("piece",no));

  //kdDebug(12010) << " setPiece("<<x<<","<<y<<","<<color<<","<<no<<") sprite=" << sprite<<endl;

  // Check for removal of sprite
  if (color==Niemand)
  {
    sprite->hide();
    return ;
  }

  // Make sure the frames are ok
  int c;
  if (color==Gelb) c=0;
  else c=1;

  if (sprite)
  {
    if (animation)
    {
      sprite->move(x*(sprite->width()+mSpreadX)+mBoardX,
                  mBoardY-sprite->height()-mSpreadY);
      sprite->moveTo(sprite->x(),
                    sprite->y()+y*(sprite->height()+mSpreadY)+mBoardY);
      connect(sprite->createNotify(),SIGNAL(signalNotify(QCanvasItem *,int)),
          doc,SLOT(moveDone(QCanvasItem *,int)));
    }
    else
    {
      sprite->move(x*(sprite->width()+mSpreadX)+mBoardX,
                  mBoardY-sprite->height()-mSpreadY+
                  y*(sprite->height()+mSpreadY)+mBoardY);
      // Prevent moving (== speed =0)
      sprite->moveTo(sprite->x(),sprite->y());
      connect(sprite->createNotify(),SIGNAL(signalNotify(QCanvasItem *,int)),
          doc,SLOT(moveDone(QCanvasItem *,int)));
      sprite->emitNotify(3);
    }
    
    sprite->setFrame(c);
    sprite->show();
  }
}

void Kwin4View::setArrow(int x,int color)
{
  KSprite *sprite=0;

  if (mLastArrow>=0)
    sprite=(KSprite *)(mCache->getItem("arrow",mLastArrow));
  else 
    sprite=0;
  if (sprite) 
    sprite->setFrame(0);
  
  sprite=(KSprite *)(mCache->getItem("arrow",x));

  //kdDebug(12010) << " setArrow("<<x<<","<<color<<") sprite=" << sprite<<endl;

  // Make sure the frames are ok
  int c = 0;
  if (color==Gelb)
     c=1;
  else if (color==Rot)
     c=2;

  if (sprite)
    sprite->setFrame(c);
  mLastArrow=x;
}


/**
 * Error message if the wrong player moved
 */
bool Kwin4View::wrongPlayer(KPlayer *player,KGameIO::IOMode io)
{
  // Hack to find out whether there is a IO Device whose turn it is
  KGame::KGamePlayerList *playerList=doc->playerList();
  KPlayer *p;

  bool flag=false;
  for ( p=playerList->first(); p!= 0; p=playerList->next() )
  {
    if (p==player) continue;
    if (p->hasRtti(io) && p->myTurn()) flag=true;
  }

  if (flag)
    return false;
    
  clearError();
  int rnd=(kapp->random()%4) +1;
  QString m;
  m=QString("text%1").arg(rnd);
  QString ms;
  if (rnd==1)      ms=i18n("Hold on... the other player has not been yet..."); 
  else if (rnd==2) ms=i18n("Hold your horses..."); 
  else if (rnd==3) ms=i18n("Ah ah ah... only one go at a time..."); 
  else             ms=i18n("Please wait... it is not your turn."); 

  // TODO MH can be unique
  QCanvasText *text=(QCanvasText *)(mCache->getItem(m,1));
  if (text)
  {
    text->setText(ms);
    text->show();
  }
  return true;
}

/**
 * This slot is called when a key event is received. It then prduces a
 * valid move for the game.
 **/
// This is analogous to the mouse event only it is called when a key is
// pressed
void Kwin4View::slotKeyInput(KGameIO *input,QDataStream &stream,QKeyEvent *e,bool *eatevent)
{
  // Ignore non running
  if (!doc->isRunning())
    return;
  // kdDebug(12010) << "KEY EVENT" << e->ascii() << endl;

  // Ignore non key press
  if (e->type() != QEvent::KeyPress) return ;

  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn())
  {
    *eatevent=wrongPlayer(player,KGameIO::KeyIO);
    return;
  }

  int code=e->ascii();
  if (code<'1' || code>'7')
  {
    //kdDebug(12010) << "Key not supported\n";
    return ;
  }

  // Create a move
  Q_INT32 move,pl;
  move=code-'1';
  pl=player->userId();
  stream << pl << move;
  *eatevent=true;
}

/**
 * This slot is called when a mouse key is pressed. As the mouse is used as
 * input for all players
 * this slot is called to generate a player move out of a mouse input, i.e.
 * it converts a QMouseEvent into a move for the game. We do here some
 * simple nonsense and use the position of the mouse to generate
 * moves containing the keycodes
 */
void Kwin4View::slotMouseInput(KGameIO *input,QDataStream &stream,QMouseEvent *mouse,bool *eatevent)
{
  // Only react to key pressed not released
  if (mouse->type() != QEvent::MouseButtonPress ) return ;
  if (!doc->isRunning())
    return;

  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn())
  {
    *eatevent=wrongPlayer(player,KGameIO::MouseIO);
    return;
  }

  if (mouse->button()!=LeftButton) return ;
  //if (!doc->IsRunning()) return ;

  QPoint point;
  int x,y;

  point=mouse->pos() - QPoint(15,40) - QPoint(20,20);
  if (point.x()<0) return ;

  x=point.x()/FIELD_SPACING;
  y=point.y()/FIELD_SPACING;

  if (y>=FIELD_SIZE_Y) return ;
  if (x<0 || x>=FIELD_SIZE_X) return;

  // Create a move
  Q_INT32 move,pl;
  move=x;
  pl=player->userId();
  stream << pl << move;
  *eatevent=true;
  // kdDebug(12010) << "Mouse input done..eatevent=true" << endl;
}

/**
 * Hide all the error sprites
 */ 
void Kwin4View::clearError()
{
  QCanvasText *text;

  text=(QCanvasText *)(mCache->getItem("text1",1));
  if (text) text->hide();
  text=(QCanvasText *)(mCache->getItem("text2",1));
  if (text) text->hide();
  text=(QCanvasText *)(mCache->getItem("text3",1));
  if (text) text->hide();
  text=(QCanvasText *)(mCache->getItem("text4",1));
  if (text) text->hide();
}

void Kwin4View::resizeEvent(QResizeEvent *e)
{
  if (mCanvas) mCanvas->resize(e->size().width(),e->size().height()); 
}

#include "kwin4view.moc"
