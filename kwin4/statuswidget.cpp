/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2001 by Martin Heni
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

#include "statuswidget.h"

#include <kdebug.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocale.h>

#define COL_STATUSBORDER black
#define COL_STATUSFIELD_L  QColor(130,130,255).light(110)
#define COL_STATUSFIELD_D  QColor(130,130,255).light(120)
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)

StatusWidget::StatusWidget( QWidget* parent,  const char* name, WFlags fl )
    : QFrame( parent, name, fl )
{
  setFrameStyle( QFrame::Box | QFrame::Raised );
  setLineWidth( 2 );
  setMidLineWidth( 4 );
     
  setBackgroundColor( COL_STATUSFIELD );
  
  kdDebug(12010) << "StatusWidget:" << this->layout() << endl;

   int row=0;
   int col=0;

    LayoutB = new QGridLayout( this);
    LayoutB->setSpacing( 3 );
    LayoutB->setMargin( 15 );

    col=1;
    QSpacerItem *Spacer0=new QSpacerItem(16,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer0, row, row+3,col,col );
    col++;

    Label[0][1] = new QLabel( this, "01" );
    Label[0][1]->setText( i18n( "1-2_letter_abbr_won","W" ) );
    Label[0][1]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][1]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][1], row, col );
    col++;
    Label[0][2] = new QLabel( this, "02" );
    Label[0][2]->setText( i18n( "1-2_letter_abbr_drawn","D" ) );
    Label[0][2]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][2]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][2], row, col );
    col++;
    Label[0][3] = new QLabel( this, "03" );
    Label[0][3]->setText( i18n("1-2_letter_abbr_lost","L" ) );
    Label[0][3]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][3]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][3], row, col );
    col++;
    QSpacerItem *Spacer1=new QSpacerItem(16,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer1, row, row+3,col,col );
    col++;
    Label[0][4] = new QLabel( this, "04" );
    Label[0][4]->setText( i18n("1-2_letter_abbr_number","No" ) );
    Label[0][4]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][4]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][4], row, col );
    col++;
    Label[0][5] = new QLabel( this, "05" );
    Label[0][5]->setText( i18n( "1-2_letter_abbr_breaks/aborted","Bk" ) );
    Label[0][5]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][5]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][5], row, col );
    col++;

    row++;

    QSpacerItem *Spacer2=new QSpacerItem(0,3,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer2, row, row,0,col-1 );

    col=0;
    row++;

    Label[1][0] = new QLabel( this, "10" );
    setPlayer("-------------------",0);
    Label[1][0]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][0]->setAlignment(Qt::AlignLeft);
    LayoutB->addWidget( Label[1][0], row, col );
    col++;
    col++;
    Label[1][1] = new QLabel( this, "11" );
    setWin(0,0);
    Label[1][1]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][1]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][1], row, col );
    col++;
    Label[1][2] = new QLabel( this, "12" );
    setRemis(0,0);
    Label[1][2]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][2]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][2], row, col );
    col++;
    Label[1][3] = new QLabel( this, "13" );
    setLost(0,0);
    Label[1][3]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][3]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][3], row, col );
    col++;
    col++;
    Label[1][4] = new QLabel( this, "14" );
    setSum(0,0);
    Label[1][4]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][4]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][4], row, col );
    col++;
    Label[1][5] = new QLabel( this, "15" );
    setBrk(0,0);
    Label[1][5]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][5]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][5], row, col );
    col++;

    col=0;
    row++;

    Label[2][0] = new QLabel( this, "20" );
    setPlayer("-----",1);
    Label[2][0]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][0]->setAlignment(Qt::AlignLeft);
    LayoutB->addWidget( Label[2][0], row, col );
    col++;
    col++;
    Label[2][1] = new QLabel( this, "21" );
    setWin(0,1);
    Label[2][1]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][1]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][1], row, col );
    col++;
    Label[2][2] = new QLabel( this, "22" );
    setRemis(0,1);
    Label[2][2]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][2]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][2], row, col );
    col++;
    Label[2][3] = new QLabel( this, "23" );
    setLost(0,1);
    Label[2][3]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][3]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][3], row, col );
    col++;
    col++;
    Label[2][4] = new QLabel( this, "24" );
    setSum(0,1);
    Label[2][4]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][4]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][4], row, col );
    col++;
    Label[2][5] = new QLabel( this, "25" );
    setBrk(0,1);
    Label[2][5]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][5]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][5], row, col );
    col++;

    adjustSize();
}

void StatusWidget::paintEvent( QPaintEvent * p)
{
    QPainter paint( this );
    paint.setClipRect(p->rect());
    Paint( &paint, p->rect() );			
}

void StatusWidget::Paint(QPainter *p,QRect )
{
  QPalette pal;
  pal.setColor(QColorGroup::Light, COL_STATUSLIGHT);
  pal.setColor(QColorGroup::Mid, COL_STATUSFIELD);
  pal.setColor(QColorGroup::Dark, COL_STATUSDARK);
  setPalette(pal); 
  drawFrame(p);
  
  QRect lrect;
  p->setPen(COL_STATUSFIELD_L);
  p->setBrush(COL_STATUSFIELD_L);
  lrect=LayoutB->cellGeometry(2,0)|LayoutB->cellGeometry(2,7);
  p->drawRect(lrect);

  p->setPen(COL_STATUSFIELD_D);
  p->setBrush(COL_STATUSFIELD_D);
  lrect=LayoutB->cellGeometry(3,0)|LayoutB->cellGeometry(3,7);
  p->drawRect(lrect);
  
  p->setPen(COL_STATUSBORDER);
}

void StatusWidget::setPlayer(QString s,int no)
{
  if (no==0)  Label[1][0]->setText(s);
  else Label[2][0]->setText(s);
  adjustSize();
}

void StatusWidget::setWin(int i,int no)
{
  if (no==0)  Label[1][1]->setText(QString("%1").arg(i));
  else Label[2][1]->setText( QString("%1").arg(i) );
  adjustSize();
}

void StatusWidget::setRemis(int i,int no)
{
  if (no==0)  Label[1][2]->setText(QString("%1").arg(i));
  else Label[2][2]->setText( QString("%1").arg(i) );
  adjustSize();
}

void StatusWidget::setLost(int i,int no)
{
  if (no==0)  Label[1][3]->setText(QString("%1").arg(i));
  else Label[2][3]->setText( QString("%1").arg(i) );
  adjustSize();
}

void StatusWidget::setSum(int i,int no)
{
  if (no==0)  Label[1][4]->setText(QString("%1").arg(i));
  else Label[2][4]->setText( QString("%1").arg(i) );
  adjustSize();
}

void StatusWidget::setBrk(int i,int no)
{
  if (no==0)  Label[1][5]->setText(QString("%1").arg(i));
  else Label[2][5]->setText( QString("%1").arg(i) );
  adjustSize();
}

#include "statuswidget.moc"
