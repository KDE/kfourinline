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

#include <QLabel>
#include <QLayout>
#include <qpainter.h>
#include <qsizepolicy.h>
//Added by qt3to4:
#include <Q3Frame>
#include <QGridLayout>
#include <QPaintEvent>
#include <kdebug.h>
#include <klocale.h>

#define COL_STATUSBORDER Qt::black
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)

ScoreWidget::ScoreWidget( QWidget* parent,  const char* name, Qt::WFlags fl )
    : Q3Frame( parent, name, fl )
{
  setFrameStyle( Q3Frame::Box | Q3Frame::Raised );
  setLineWidth( 2 );
  setMidLineWidth( 4 );

  QPalette palette;
  palette.setColor( backgroundRole(), COL_STATUSFIELD );
  setPalette( palette );

   resize( 255, 187 );
   int row=0;

    setWindowTitle( i18n( "Form1" ) );
    LayoutB = new QGridLayout(this);
    LayoutB->setMargin( 15 );
    LayoutB->setSpacing( 5 );

    TextLabel7 = new QLabel( this );
    setPlayer("-----",0);
    TextLabel7->setPalette( palette );
    TextLabel7->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( TextLabel7, row, 0, 1, 3 );
    row++;

    TextLabel8 = new QLabel( this );
    TextLabel8->setText( i18n( "vs" ) );
    TextLabel8->setPalette( palette );
    TextLabel8->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( TextLabel8, row, 0, 1, 3 );
    row++;

    TextLabel9 = new QLabel( this );
    setPlayer("-----",1);
  //  TextLabel9->setFrameShape(QFrame::Box );
  //  TextLabel9->setLineWidth(5);
    TextLabel9->setPalette( palette );
    TextLabel9->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( TextLabel8, row, 0, 1, 3 );
    row++;

    QSpacerItem *Spacer2=new QSpacerItem(0,16,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addItem( Spacer2, row, 0 , 1, 3 );
    row++;

    QSpacerItem *Spacer1=new QSpacerItem(25,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addItem( Spacer1, row, 1, 3, 1 );

    TextLabel1 = new QLabel( this );
    TextLabel1->setText( i18n( "Level" ) );
    TextLabel1->setPalette( palette );
    LayoutB->addWidget( TextLabel1, row, 0 );

    TextLabel4 = new QLabel( this );
    setLevel(Prefs::level());
    TextLabel4->setAlignment(Qt::AlignRight);
    TextLabel4->setPalette( palette );
    LayoutB->addWidget( TextLabel4, row, 2 );

    row++;


    TextLabel2 = new QLabel( this );
    TextLabel2->setText( i18nc("number of MOVE in game", "Move" ) );
    TextLabel2->setPalette( palette );
    LayoutB->addWidget( TextLabel2, row, 0 );

    TextLabel5 = new QLabel( this );
    setMove(0);
    TextLabel5->setAlignment(Qt::AlignRight);
    TextLabel5->setPalette( palette );
    LayoutB->addWidget( TextLabel5, row, 2 );

    row++;


    TextLabel3 = new QLabel( this );
    TextLabel3->setText( i18n( "Chance" ) );
    TextLabel3->setPalette( palette );
    LayoutB->addWidget( TextLabel3, row, 0 );

    TextLabel6 = new QLabel( this );
    setChance(0);
    TextLabel6->setAlignment(Qt::AlignRight);
    TextLabel6->setPalette( palette );
    LayoutB->addWidget( TextLabel6, row, 2 );

    row++;

    QSpacerItem *Spacer3=new QSpacerItem(0,8,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addItem( Spacer3, row, 0, 1, 3 );
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
  pal.setColor(QPalette::Light, COL_STATUSLIGHT);
  pal.setColor(QPalette::Mid, COL_STATUSFIELD);
  pal.setColor(QPalette::Dark, COL_STATUSDARK);
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
  QPalette palette;
  if (i==0)
  {
    palette.setColor( TextLabel7->foregroundRole(), Qt::yellow );
    TextLabel7->setPalette( palette );
    palette.setColor( TextLabel9->foregroundRole(), Qt::black );
    TextLabel9->setPalette( palette );
  }
  else
  {
    palette.setColor( TextLabel7->foregroundRole(), Qt::black );
    TextLabel7->setPalette( palette );
    palette.setColor( TextLabel9->foregroundRole(), Qt::red );
    TextLabel9->setPalette( palette );
  }

  TextLabel7->update();
  TextLabel9->update();
}

#include "scorewidget.moc"
