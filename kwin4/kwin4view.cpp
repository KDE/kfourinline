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
#include <qprinter.h>
#include <qpainter.h>
#include <stdio.h>

#include <klocale.h>
#include <kmessagebox.h>

// application specific includes
#include "kwin4view.h"
#include "kwin4doc.h"
#include "kwin4.h"
#include "KEInput.h"
#include "KEMessage.h"
#include "geom.h"


// lightGray
#define COL_STATUSBORDER black
//#define COL_STATUSFIELD  lightGray
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)
#define COL_PLAYER       QColor(255,255,0)
#define COL_RED          red
#define COL_YELLOW       yellow



Kwin4View::Kwin4View(QWidget *parent, const char *name) : QWidget(parent, name)
{
  setBackgroundMode(PaletteBase);
  setBackgroundColor(QColor(0,0,128));


  setBackgroundPixmap( getDocument()->m_PixBackground );


  /*
  QFontInfo font=fontInfo();
  printf("Fontinfo: pnt=%d family=%s exact=%d\n",
      font.pointSize(),font.family().latin1(),font.exactMatch());
  */
}

Kwin4View::~Kwin4View()
{
}

Kwin4Doc *Kwin4View::getDocument() const
{
  Kwin4App *theApp=(Kwin4App *) parentWidget();

  return theApp->getDocument();
}

void Kwin4View::print(QPrinter *pPrinter)
{
  QPainter printpainter;
  printpainter.begin(pPrinter);
	
  // TODO: add your printing code here

  printpainter.end();
}
/** Interface to the Paint Event */
void Kwin4View::Paint(QPainter *p){

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


}
/** QT Paint Event */
void Kwin4View::paintEvent( QPaintEvent * p){
    QPainter paint( this );
    paint.setClipRect(p->rect());
    Paint( &paint );			

}

/** Updates the table area */
void Kwin4View::updateTable(){
  update(geom.table_rect);
}

/** Updates the status area */
void Kwin4View::updateStatus(){
  update(geom.status_rect);
}

/** Update XY area */
void Kwin4View::updateXY(int x,int y){
  QPoint point1,point2;
  y=geom.field_my-y;
  point1=geom.field_origin+geom.field_offset;
  point1+=QPoint(geom.field_dx*x,geom.field_dy*y);
  point2=point1+QPoint(geom.field_dx,geom.field_dy);

  QRect rect(point1,point2);

  update(rect);
}

/** Updates on of the column arrows */
void Kwin4View::updateArrow(int x){
  QPoint point1,point2;

  if (x<0 || x>=geom.field_mx) return; // got -1 or so

  point1=geom.field_origin+geom.field_offset;
  point1+=QPoint(x*geom.field_dx,geom.field_arrow_dy);

  point2=point1+QPoint(geom.field_dx,geom.field_dy);
  QRect rect(point1,point2);

  update(rect);
}

/** Draw the game intro */
void Kwin4View::drawIntro(QPainter *p){
  p->drawPixmap(geom.intro_origin,getDocument()->m_PixAbout);
}
/** Draw the game board */
void Kwin4View::drawField(QPainter *p){
  int x,y;
  QPoint point;

  p->drawPixmap(geom.field_origin,getDocument()->m_PixBoard);

  for (x=0;x<geom.field_mx;x++)
  {
    point=geom.field_origin+geom.field_offset;
    point+=QPoint(x*geom.field_dx,geom.field_arrow_dy);

    if (getDocument()->QueryLastcolumn()==x &&
        getDocument()->QueryLastcolour()==Rot)
    {
       p->drawPixmap(point,getDocument()->m_PixArrowRed);
    }
    else if (getDocument()->QueryLastcolumn()==x &&
        getDocument()->QueryLastcolour()==Gelb)
    {
       p->drawPixmap(point,getDocument()->m_PixArrowYellow);
    }
    else
    {
       p->drawPixmap(point,getDocument()->m_PixArrow);
    }
  }/* end arrows */

  FARBE colour;
  for (y=geom.field_my-1;y>=0;y--)
  {
    for (x=0;x<geom.field_mx;x++)
    {
      point=geom.field_origin+geom.field_offset;
      point+=QPoint(geom.field_dx*x,geom.field_dy*y);

      colour=getDocument()->QueryColour(x,geom.field_my-1-y);
      switch(colour)
      {
        case Gelb:
           p->drawPixmap(point,getDocument()->m_PixFieldYellow);
        break;
        case Rot:
           p->drawPixmap(point,getDocument()->m_PixFieldRed);
        break;
        case Tip:
           p->drawPixmap(point,getDocument()->m_PixFieldHint);
        break;
        // Could be improved by another piece for the blinking
        // e.g. a flashing stone
        case GelbWin:
        case RotWin:
           p->drawPixmap(point,getDocument()->m_PixFieldEmpty);
        break;
        default:
           p->drawPixmap(point,getDocument()->m_PixFieldEmpty);
        break;
      }
    }
  }

}
/** Draw the status window */
void Kwin4View::drawStatus(QPainter *p){
  QPoint p1,p2;
  FARBE beginner,second;
  QFont font11(p->font());
  font11.setPointSize(11);
  p->setFont(font11);

	beginner=getDocument()->QueryPlayerColour(0);
	second=getDocument()->QueryPlayerColour(1);

  // Draw border and field
  p->setPen(COL_STATUSBORDER);
  p->setBrush(COL_STATUSFIELD);
  p->drawRect(geom.status_rect);
  drawBorder(p,geom.status_rect,0,4,0);
	drawBorder(p,geom.status_rect,10,1,1);

  // draw text
  QString ld;

	p->setPen(black);
	p1=geom.status_rect.topLeft();
	p2=geom.status_rect.bottomRight();
	int width=6*(p2.x()-p1.x())/10;

  ld=i18n("versus","vs");
	p->drawText(p1.x()+geom.table_cols[0],p1.y()+geom.table_rows[0],ld);

  ld=i18n("Level:");
	p->drawText(p1.x()+geom.table_cols[1],p1.y()+geom.table_rows[1],
	            width,32,QPainter::AlignLeft|QPainter::AlignTop ,ld);
  ld.sprintf("%d",getDocument()->QueryLevel());
	p->drawText(p1.x()+geom.table_cols[1],p1.y()+geom.table_rows[1],
	            width,32,QPainter::AlignRight|QPainter::AlignTop ,ld);

  ld=i18n("Move:");
//     getDocument()->QueryCurrentMove());
	p->drawText(p1.x()+geom.table_cols[1],p1.y()+geom.table_rows[2],
	            width,32,QPainter::AlignLeft|QPainter::AlignTop ,ld);

  ld.sprintf("%2d",getDocument()->QueryCurrentMove());
	p->drawText(p1.x()+geom.table_cols[1],p1.y()+geom.table_rows[2],
	            width,32,QPainter::AlignRight|QPainter::AlignTop ,ld);

/*
     (const char *)(getDocument()->QueryName(getDocument()->QueryCurrentPlayer())));
*/

  ld=i18n("Chance:");
	p->drawText(p1.x()+geom.table_cols[1],p1.y()+geom.table_rows[3],
	            width,32,QPainter::AlignLeft|QPainter::AlignTop ,ld);
	
  if (getDocument()->QueryScore()>=WIN_VALUE) ld=i18n("Loser");
  else if (getDocument()->QueryScore()<=-WIN_VALUE) ld=i18n("Winner");
  else ld.sprintf("%ld",-getDocument()->QueryScore()/10000);
	p->drawText(p1.x()+geom.table_cols[1],p1.y()+geom.table_rows[3],
	            width,32,QPainter::AlignRight|QPainter::AlignTop ,ld);

	
  if (getDocument()->QueryCurrentPlayer()==beginner)
  {
    if (beginner==Rot) p->setPen(COL_RED);
    else p->setPen(COL_YELLOW);
  }
  else
  {
    p->setPen(black);
  }

  if (getDocument()->IsComputer(getDocument()->QueryPlayerColour(0)))
  {
    ld=i18n("Computer");
	  p->drawText(p1.x()+geom.table_cols[2],p1.y()+geom.table_rows[4],ld);
  }
  else if (getDocument()->IsUser(getDocument()->QueryPlayerColour(0)))
  {
    ld=i18n(" Human");
	  p->drawText(p1.x()+geom.table_cols[3],p1.y()+geom.table_rows[4],ld);
  }
  else
  {
    ld=i18n("Remote");
	  p->drawText(p1.x()+geom.table_cols[3],p1.y()+geom.table_rows[4],ld);
  }

  if (getDocument()->QueryCurrentPlayer()==second)
  {
    if (second==Rot) p->setPen(COL_RED);
    else p->setPen(COL_YELLOW);
  }
  else
  {
    p->setPen(black);
  }

  if (getDocument()->IsComputer(getDocument()->QueryPlayerColour(1)))
  {
    ld=i18n("Computer");
	  p->drawText(p1.x()+geom.table_cols[2],p1.y()+geom.table_rows[5],ld);
  }
  else if (getDocument()->IsUser(getDocument()->QueryPlayerColour(1)))
  {
    ld=i18n(" Human");
	  p->drawText(p1.x()+geom.table_cols[3],p1.y()+geom.table_rows[5],ld);
  }
  else
  {
    ld=i18n("Remote");
	  p->drawText(p1.x()+geom.table_cols[3],p1.y()+geom.table_rows[5],ld);
  }
}

/** Draw the table */
void Kwin4View::drawTable(QPainter *p){
  QPoint p1,p2;
  FARBE beginner,second;
  QFont font11(p->font());
  font11.setPointSize(11);
  p->setFont(font11);
	beginner=getDocument()->QueryPlayerColour(0);
	second=getDocument()->QueryPlayerColour(1);

  // Draw border and field
  p->setPen(COL_STATUSBORDER);
  p->setBrush(COL_STATUSFIELD);
	p->drawRect(geom.table_rect);
  drawBorder(p,geom.table_rect,0,2,0);
	drawBorder(p,geom.table_rect,6,1,1);

  // draw lines
	p1=geom.table_rect.topLeft();
	p2=geom.table_rect.bottomRight();
	p->setPen(black);
	p->moveTo(p1.x()+12,p1.y()+30);
	p->lineTo(p2.x()-12,p1.y()+30);

	p->moveTo(p1.x()+87,p1.y()+15);p->lineTo(p1.x()+87,p2.y()-12);
	p->moveTo(p1.x()+107,p1.y()+15);p->lineTo(p1.x()+107,p2.y()-12);
	p->moveTo(p1.x()+127,p1.y()+15);p->lineTo(p1.x()+127,p2.y()-12);

	p->moveTo(p1.x()+187,p1.y()+15);p->lineTo(p1.x()+187,p2.y()-12);
	p->moveTo(p1.x()+207,p1.y()+15);p->lineTo(p1.x()+207,p2.y()-12);

  // draw Headertext
  QString ld;
  ld=i18n("1_letter_abbr_won","W"); // abbr for "Won"
	p->drawText(p1.x()+geom.status_cols[1]-4,p1.y()+geom.status_rows[0],ld);
  ld=i18n("1_letter_abbr_drawn","D"); // abbr for "drawn"
	p->drawText(p1.x()+geom.status_cols[2]-4,p1.y()+geom.status_rows[0],ld);
  ld=i18n("1_letter_abbr_lost","L"); // abbr for "lost"
	p->drawText(p1.x()+geom.status_cols[3]-4,p1.y()+geom.status_rows[0],ld);
  ld=i18n("1-2_letter_abbr_number","No"); // abbr for "number"
	p->drawText(p1.x()+geom.status_cols[4]-4,p1.y()+geom.status_rows[0],ld);
  ld=i18n("1-2_letter_abbr_breaks/aborted","Bk"); // abbr for "breaks"
	p->drawText(p1.x()+geom.status_cols[5]-4,p1.y()+geom.status_rows[0],ld);


  // draw status lines
  int sum,j;
  FARBE col;

  for (j=0;j<2;j++)
  {
    col=getDocument()->QueryPlayerColour(j);
    if (col==Rot) p->setPen(COL_RED);
    else p->setPen(COL_YELLOW);

    sum=getDocument()->QueryTable(col,TSum);

    ld=getDocument()->QueryName(col);
    p->drawText(p1.x()+geom.status_cols[0] ,p1.y()+geom.status_rows[1+j],ld);
    ld.sprintf("%2d",getDocument()->QueryTable(col,TWin));
    p->drawText(p1.x()+geom.status_cols[1],p1.y()+geom.status_rows[1+j],ld);
    ld.sprintf("%2d",getDocument()->QueryTable(col,TRemis));
    p->drawText(p1.x()+geom.status_cols[2],p1.y()+geom.status_rows[1+j],ld);
    ld.sprintf("%2d",getDocument()->QueryTable(col,TLost));
    p->drawText(p1.x()+geom.status_cols[3],p1.y()+geom.status_rows[1+j],ld);
    ld.sprintf("%2d",sum);
    p->drawText(p1.x()+geom.status_cols[4],p1.y()+geom.status_rows[1+j],ld);
    ld.sprintf("%2d",getDocument()->QueryTable(col,TBrk));
    p->drawText(p1.x()+geom.status_cols[5],p1.y()+geom.status_rows[1+j],ld);
  }
}

/** Draw a Borderframe */
void Kwin4View::drawBorder(QPainter *p,QRect rect,int offset,int width,int mode){
QPen graypen;
int i;
QPoint p1,p2;

	if (mode!=0 && mode!=1) return;

	p1=rect.topLeft();
	p2=rect.bottomRight();

	if (mode==0) p->setPen(COL_STATUSLIGHT);
	else if (mode==1) p->setPen(COL_STATUSDARK);
	
	for (i=0;i<width;i++)
	{
		p->moveTo(p1.x()+offset+i,p2.y()-offset-i);
		p->lineTo(p1.x()+offset+i,p1.y()+offset+i);
		p->lineTo(p2.x()-offset-i,p1.y()+offset+i);
	}
	if (mode==1) p->setPen(COL_STATUSLIGHT);
	else if (mode==0) p->setPen(COL_STATUSDARK);
	for (i=0;i<width;i++)
	{
		p->moveTo(p1.x()+offset+i,p2.y()-offset-i);
		p->lineTo(p2.x()-offset-i,p2.y()-offset-i);
		p->lineTo(p2.x()-offset-i,p1.y()+offset+i);
	}
}

// mouse click event
void Kwin4View::mousePressEvent( QMouseEvent *mouse )
{
  if (mouse->button()!=LeftButton) return ;
  if (!getDocument()->IsRunning()) return ;


  QPoint point;
  int x,y;

  point=mouse->pos()-geom.field_origin-geom.field_offset;
  if (point.x()<0) return ;

  x=point.x()/geom.field_dx;
  y=point.y()/geom.field_dx;

  if (y>=geom.field_my) return ;
  if (x<0 || x>=geom.field_mx) return;

  if (getDocument()->QueryInputHandler()->IsInteractive())
  {
    KEMessage *msg=new KEMessage;
    msg->AddData(QCString("Move"),(short)x);
    getDocument()->QueryInputHandler()->SetInput(msg);
    delete msg;
  }
  else
  {
    QString m;
    switch(getDocument()->Random(4))
    {
      case 0:
        m=i18n("Hold on..the other was not yet gone...");
      break;
      case 1:
        m=i18n("Hold your horses...");
      break;
      case 2:
        m=i18n("Ah ah ah...only one go at a time...");
      break;
      default:
        m=i18n("Please wait... it is not your turn.");
    }
    Kwin4App *theApp=(Kwin4App *) parentWidget();
    KMessageBox::information(this,m, theApp->appTitle());
  }
}
#include "kwin4view.moc"
