/***************************************************************************
                                kwin4 program
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

#include "scorewidget.h"

#include "prefs.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qsizepolicy.h>
#include <kdebug.h>
#include <klocale.h>

#define COL_STATUSBORDER black
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)

ScoreWidget::ScoreWidget( QWidget* parent,  const char* name, WFlags fl )
    : QFrame( parent, name, fl )
{
  setFrameStyle( QFrame::Box | QFrame::Raised );
  setLineWidth( 2 );
  setMidLineWidth( 4 );
	
  setBackgroundColor( COL_STATUSFIELD );

   resize( 255, 187 ); 
   int row=0;

    setCaption( i18n( "Form1" ) );
    //LayoutB = new QGridLayout( this,4,3,15,5 ); 
    LayoutB = new QGridLayout( this);
    LayoutB->setSpacing( 3 );
    LayoutB->setMargin( 15 );

    TextLabel7 = new QLabel( this, "TextLabel7" );
    setPlayer("-----",0);
    TextLabel7->setBackgroundColor( COL_STATUSFIELD );
    TextLabel7->setAlignment(Qt::AlignHCenter);
    LayoutB->addMultiCellWidget( TextLabel7, row, row,0,2 );
    row++;
    
    TextLabel8 = new QLabel( this, "TextLabel8" );
    TextLabel8->setText( i18n( "vs" ) );
    TextLabel8->setBackgroundColor( COL_STATUSFIELD );
    TextLabel8->setAlignment(Qt::AlignHCenter);
    LayoutB->addMultiCellWidget( TextLabel8, row, row,0,2 );
    row++;
    
    TextLabel9 = new QLabel( this, "TextLabel9" );
    setPlayer("-----",1);
  //  TextLabel9->setFrameShape(QFrame::Box );
  //  TextLabel9->setLineWidth(5);
    TextLabel9->setBackgroundColor( COL_STATUSFIELD );
    TextLabel9->setAlignment(Qt::AlignHCenter);
    LayoutB->addMultiCellWidget( TextLabel9, row, row,0,2 );
    row++;

    QSpacerItem *Spacer2=new QSpacerItem(0,16,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer2, row, row,0,2 );
    row++;

    QSpacerItem *Spacer1=new QSpacerItem(25,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer1, row, row+2,1,1 );

    TextLabel1 = new QLabel( this, "Level" );
    TextLabel1->setText( i18n( "Level" ) );
    TextLabel1->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel1, row, 0 );

    TextLabel4 = new QLabel( this, "L" );
    setLevel(Prefs::level());
    TextLabel4->setAlignment(Qt::AlignRight);
    TextLabel4->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel4, row, 2 );

    row++;


    TextLabel2 = new QLabel( this, "Move" );
    TextLabel2->setText( i18n("number of MOVE in game", "Move" ) );
    TextLabel2->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel2, row, 0 );

    TextLabel5 = new QLabel( this, "M" );
    setMove(0);
    TextLabel5->setAlignment(Qt::AlignRight);
    TextLabel5->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel5, row, 2 );

    row++;


    TextLabel3 = new QLabel( this, "Chance" );
    TextLabel3->setText( i18n( "Chance" ) );
    TextLabel3->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel3, row, 0 );

    TextLabel6 = new QLabel( this, "C" );
    setChance(0);
    TextLabel6->setAlignment(Qt::AlignRight);
    TextLabel6->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel6, row, 2 );

    row++;

    QSpacerItem *Spacer3=new QSpacerItem(0,8,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer3, row, row,0,2 );
    row++;



    adjustSize();
}

void ScoreWidget::paintEvent( QPaintEvent * p)
{
    QPainter paint( this );
    paint.setClipRect(p->rect());
    Paint( &paint, p->rect() );			
}

void ScoreWidget::Paint(QPainter *p,QRect /*cliprect*/)
{
  QPalette pal;
  pal.setColor(QColorGroup::Light, COL_STATUSLIGHT);
  pal.setColor(QColorGroup::Mid, COL_STATUSFIELD);
  pal.setColor(QColorGroup::Dark, COL_STATUSDARK);
  setPalette(pal); 
  drawFrame(p);
}

void ScoreWidget::setMove(int i)
{
  TextLabel5->setText( QString("%1").arg(i));
}

void ScoreWidget::setLevel(int i)
{
  TextLabel4->setText( QString("%1").arg(i));
}

void ScoreWidget::setChance(int i)
{
  if (i==0)
    TextLabel6->setText("    ----");
  else if (i>=999) 
    TextLabel6->setText(i18n("Winner"));
  else if (i<=-999) 
    TextLabel6->setText(i18n("Loser"));
  else
    TextLabel6->setText(QString("%1").arg(i));
}

void ScoreWidget::setPlayer(QString s,int no)
{
  if (no==0) TextLabel7->setText(s);
  else TextLabel9->setText(s);
}

void ScoreWidget::setTurn(int i)
{
  if (i==0)
  {
    TextLabel7->setPaletteForegroundColor ( yellow);
    TextLabel9->setPaletteForegroundColor (black);
  }
  else
  {
    TextLabel9->setPaletteForegroundColor ( red);
    TextLabel7->setPaletteForegroundColor (black);
  }
  TextLabel7->update();
  TextLabel9->update();
}

#include "scorewidget.moc"
