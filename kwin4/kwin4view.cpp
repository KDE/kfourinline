/***************************************************************************
                          FILENAME|  -  description
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

// include files for Qt
#include <qpainter.h>
#include <stdio.h>

#include <klocale.h>
#include <kmessagebox.h>

// application specific includes
#include "kwin4view.h"
#include "kwin4doc.h"
#include "kwin4.h"
#include "geom.h"
#include "scorewidget.h"
#include "statuswidget.h"



// lightGray
#define COL_STATUSBORDER black
//#define COL_STATUSFIELD  lightGray
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)
#define COL_PLAYER       QColor(255,255,0)
#define COL_RED          red
#define COL_YELLOW       yellow



Kwin4View::Kwin4View(QString grafixdir,QWidget *parent, const char *name)
        : QCanvasView(0,parent, name)

{
  mLastArrow=-1;


  mGrafix=grafixdir;
  kdDebug() << "Kwin4View:: grafixdir=" << grafixdir << endl;
  setVScrollBarMode(AlwaysOff);
  setHScrollBarMode(AlwaysOff);

  //setBackgroundMode(PaletteBase);
  setBackgroundColor(QColor(0,0,128));

  mCanvas=new QCanvas(parent);
  mCanvas->resize(parent->width(),parent->height()); 
  mCanvas->setDoubleBuffering(true);
  setCanvas(mCanvas);

  mCache=new KSpriteCache(mGrafix,this);
  mCache->setCanvas(mCanvas);
  KConfig *config=mCache->config();

  // Write a few i18n Strings as text sprites
  config->setGroup("text1");
  config->writeEntry("text",i18n("Hold on..the other was not yet gone...")); 
  config->setGroup("text2");
  config->writeEntry("text",i18n("Hold your horses...")); 
  config->setGroup("text3");
  config->writeEntry("text",i18n("Ah ah ah...only one go at a time...")); 
  config->setGroup("text4");
  config->writeEntry("text",i18n("Please wait... it is not your turn.")); 
  config->sync();


  QPoint pnt;
  config->setGroup("game");

  pnt=config->readPointEntry("scorewidget");
  mScoreWidget=new ScoreWidget(viewport());
  addChild(mScoreWidget);
  mScoreWidget->move(pnt);
  mScoreWidget->show();

  pnt=config->readPointEntry("statuswidget");
  mStatusWidget=new StatusWidget(this);
  mStatusWidget->move(pnt);
  mStatusWidget->show();

  move(0,0);
  adjustSize();

  initView(false);  

  // setBackgroundPixmap( getDocument()->m_PixBackground );
  //setBackground( getDocument()->m_PixBackground );
  
  //QTimer::singleShot(10000,this,SLOT(slotTest()));
  kdDebug() << "****************** UPDATE="<<isUpdatesEnabled() << endl;

}

void Kwin4View::slotTest()
{
  kdDebug() << "slotTest" << endl;
  kdDebug() << "slotTest DONE" << endl;
}


Kwin4View::~Kwin4View()
{
  if (mCanvas) delete mCanvas;
  if (mCache) delete mCache;

}

Kwin4Doc *Kwin4View::getDocument() const
{
  Kwin4App *theApp=(Kwin4App *) parentWidget();

  return theApp->getDocument();
}

void Kwin4View::initView(bool deleteall)
{
   // mCanvas->setAdvancePeriod(period);
   mCanvas->setAdvancePeriod(15);

  KConfig *config=mCache->config();
  config->setGroup("game");
  mSpreadX=config->readNumEntry("spread_x",0);
  mSpreadY=config->readNumEntry("spread_y",0);
  kdDebug() << "Spread : x=" << mSpreadX << " y=" << mSpreadY << endl;

  QPixmap *pixmap=loadPixmap("background.png");
  if (pixmap) mCanvas->setBackgroundPixmap(*pixmap);
  else mCanvas->setBackgroundColor(QColor(0,0,128));
  delete pixmap;

  // TODO in start functions to distinguish from intro
  drawBoard(deleteall);

  // Hide stars in any case
  KSprite *sprite=0;
  for (int i=0;i<4;i++)
  {
    sprite=(KSprite *)(mCache->getItem("star",i));
    if (sprite) sprite->hide();
  }
  // Hide GameOver in any case
  sprite=(KSprite *)(mCache->getItem("gameover",1));
  if (sprite) sprite->hide();

  // Hide pieces in any case
  for (int i=0;i<42;i++)
  {
    sprite=(KSprite *)(mCache->getItem("piece",i));
    if (sprite) sprite->hide();
  }

  // Clear error text
  clearError();
}

QPixmap *Kwin4View::loadPixmap(QString name)
{
  if (!mCache) return 0;
  return mCache->loadPixmap(name);
}

void Kwin4View::EndGame()
{
  KSprite *sprite;
  sprite=(KSprite *)(mCache->getItem("gameover",1));
  KConfig *config=mCache->config();
  int dest=config->readNumEntry("destY",150);
  kdDebug() << "MOVING gameover to " << dest << endl;

  if (sprite)
  {
    sprite->show();
    sprite->moveTo(sprite->x(),dest);
  }
}

// -------------- Draw Sprites ----------------------
void Kwin4View::drawStar(int x,int y,int no)
{
  int x_pos,y_pos;
  int dx,dy;
  y=5-y;
  KSprite *board=(KSprite *)(mCache->getItem("board",1));
  if (board)
  {
    x_pos=board->x();
    y_pos=board->y();
  }
  else
  {
    x_pos=0;
    y_pos=0;
  }
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
  kdDebug() << " setStar("<<x<<","<<y<<","<<no<<") sprite=" << sprite<<endl;
  if (sprite)
  {
    sprite->move(dx/2-sprite->width()/2+x*(dx+mSpreadX)+x_pos,
                 dy/2-sprite->height()/2+y*(dy+mSpreadY)+y_pos);
    sprite->show();
    sprite->setAnimation(0);
  }
}

void Kwin4View::drawBoard(bool remove)
{
  KSprite *sprite=0;
  KSprite *board=0;
  int x,y;

  {
    int x_pos=0;
    int y_pos=0;
    // Board
    // TODO: Without number as it is unique item
    board=(KSprite *)(mCache->getItem("board",1));
    if (board)
    {
      if (remove) board->hide();
      else if (!board->isVisible()) board->show();
      x_pos=board->x();
      y_pos=board->y();
    }

    // Arrows
    for (x=0;x<7;x++)
    {
      sprite=(KSprite *)(mCache->getItem("arrow",x));
      if (sprite)
      {
        sprite->setFrame(0);
        sprite->setX(x*(sprite->width()+mSpreadX)+x_pos);
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
          sprite->move(x*(sprite->width()+mSpreadX)+x_pos,
                       y*(sprite->height())+y_pos);
          if (remove) sprite->hide();
          else if (!sprite->isVisible()) sprite->show();
        }
        // upper layer
        sprite=(KSprite *)(mCache->getItem("empty",x+7*y));
        if (sprite)
        {
          sprite->move(x*(sprite->width()+mSpreadX)+x_pos,
                       y*(sprite->height())+y_pos);
          if (remove) sprite->hide();
          else if (!sprite->isVisible()) sprite->show();
        }
      }
    }/* end field */
  }
}

void Kwin4View::setPiece(int x,int y,int color,int no)
{
  KSprite *sprite=0;
  KSprite *board=0;

  y=5-y;

  int x_pos,y_pos;
  board=(KSprite *)(mCache->getItem("board",1));
  if (board)
  {
    x_pos=board->x();
    y_pos=board->y();
  }
  else
  {
    x_pos=0;
    y_pos=0;
  }

  sprite=(KSprite *)(mCache->getItem("piece",no));

  kdDebug() << " setPiece("<<x<<","<<y<<","<<color<<","<<no<<") sprite=" << sprite<<endl;

  // Make sure the frames are ok
  int c;
  if (color==Gelb) c=0;
  else c=1;

  if (sprite)
  {
    sprite->move(x*(sprite->width()+mSpreadX)+x_pos,
                 y_pos-sprite->height()-mSpreadY);
    sprite->moveTo(sprite->x(),
                   sprite->y()+y*(sprite->height()+mSpreadY)+y_pos);
    sprite->setFrame(c);
    sprite->show();
    connect(sprite->createNotify(),SIGNAL(signalNotify(QCanvasItem *,int)),
        getDocument(),SLOT(moveDone(QCanvasItem *,int)));

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

  kdDebug() << " setArrow("<<x<<","<<color<<") sprite=" << sprite<<endl;

  // Make sure the frames are ok
  int c;
  if (color==Gelb) c=1;
  else if (color==Rot) c=2;
  else c=0;

  if (sprite)
  {
    sprite->setFrame(c);
  }
  mLastArrow=x;
}








// This slot is called when a mouse key is pressed. As the mouse is used as input for all players
// this slot is called to generate a player move out of a mouse input, i.e.
// it converts a QMouseEvent into a move for the game. We do here some simple nonsense
// and use the position of the mouse to generate
// moves containing the keycodes
void Kwin4View::slotMouseInput(KGameIO *input,QDataStream &stream,QMouseEvent *mouse,bool *eatevent)
{
  // Only react to key pressed not released
  if (mouse->type() != QEvent::MouseButtonPress ) return ;
  if (!getDocument()->isRunning()) return;
  kdDebug() << "Kwin4View::slotMouseInput" << endl;

  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn())
  {
    // Hack to find out whether there is a IO Device whose turn it is
    KGame::KGamePlayerList *playerList=getDocument()->playerList();
    KPlayer *p;

    bool flag=false;
    for ( p=playerList->first(); p!= 0; p=playerList->next() )
    {
      if (p==player) continue;
      if (p->hasRtti(KGameIO::MouseIO) && p->myTurn()) flag=true;
    }

    kdDebug() << "$$$$$$$$$$$$$$$$$$$GameWindow:: not my turn. $$$$$$$$$$$ FLAG=" << flag << endl;
    // If there is another MouseIO we whose turn it is we assume 
    // that the mouse event goes to them. Otherwise we issue an error
    if (flag) return;
    
    clearError();
    QString m;
    m=QString("text%1").arg(getDocument()->Random(4)+1);
    kdDebug() << "Loading sprite "<< m << endl;
    // TODO MH can be unique
    QCanvasText *text=(QCanvasText *)(mCache->getItem(m,1));
    if (text) text->show();


    // We eat the event as we are sure it is nonsense
    *eatevent=true;
    return ;
  }

  if (mouse->button()!=LeftButton) return ;
  //if (!getDocument()->IsRunning()) return ;

  QPoint point;
  int x,y;

  point=mouse->pos()-geom.field_origin-geom.field_offset;
  if (point.x()<0) return ;

  x=point.x()/geom.field_dx;
  y=point.y()/geom.field_dx;

  if (y>=geom.field_my) return ;
  if (x<0 || x>=geom.field_mx) return;

  // Create a move
  Q_INT32 move,pl;
  move=x;
  pl=player->userId();
  stream << pl << x;
  *eatevent=true;
  kdDebug() << "Mouse input done..eatevent=true" << endl;
}

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

// ------------------ OLD --------------------------


/** Interface to the Paint Event */
void Kwin4View::Paint(QPainter *p){

  /*
  if (getDocument()->IsIntro())
  {
    drawIntro(p);
  }
  else
  {
    drawField(p);
    drawStatus(p);
    drawTable(p);
  }
  */


}
/** QT Paint Event */
void Kwin4View::paintEvent( QPaintEvent * p)
{
  //kdDebug() <<" **** PAINT ******" << endl;
  /* TODO MH
  mScoreWidget->update();
  mStatusWidget->update();
  */


    /*
    QPainter paint( this );
    paint.setClipRect(p->rect());
    Paint( &paint );			
    */

}


/** Draw the game intro */
void Kwin4View::drawIntro(QPainter *p)
{
  /*
  QString ld;
  p->drawPixmap(geom.intro_origin,getDocument()->m_PixAbout);
  // QFont font(p->font());
  QFont font("Helvetica");
  font.setPixelSize(36);
  font.setBold(true);
  font.setItalic(true);
  p->setFont(font);
  p->setPen(COL_YELLOW);
  ld=i18n("Four wins for KDE","Four wins\n\nfor\n\nK D E");
  QRect rect(geom.intro_origin+QPoint(0,14),getDocument()->m_PixAbout.size());
  
	p->drawText(rect,QPainter::AlignHCenter|QPainter::AlignTop ,ld);
  */

}

/** Draw the table */
void Kwin4View::drawTable(QPainter *p){
  // draw Headertext
  QString ld;
  ld=i18n("1_letter_abbr_won","W"); // abbr for "Won"
  ld=i18n("1_letter_abbr_drawn","D"); // abbr for "drawn"
  ld=i18n("1_letter_abbr_lost","L"); // abbr for "lost"
  ld=i18n("1-2_letter_abbr_number","No"); // abbr for "number"
  ld=i18n("1-2_letter_abbr_breaks/aborted","Bk"); // abbr for "breaks"
}


#include "kwin4view.moc"
