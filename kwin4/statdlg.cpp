/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Thu Mar 30 2000
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

/*
    Statdlg.cpp

    $Id$
    
    Name dialog for player Statistics
    
    (c) Martin Heni, martin@heni-online.de
	      June 1999
    
    License: GPL

*/

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qdialog.h>
#include "statdlg.h"

//#include "statdlg.moc"

#define Y0  0
#define Y1  20
#define Y2  50
#define Y3  70
#define Y4  145
#define Y5  190

#define X0  110
#define X0b 110
#define X0c 130
#define X1  10
#define X2  70
#define X3  120
#define X4  170
#define X5  220
#define X6  280



// Show the all time statistics dialog
StatDlg::StatDlg( QWidget *parent, const char *name,const char * /*sufi*/ )
    : QDialog( parent, name,TRUE )
{

  setMinimumSize(370,240);
  setMaximumSize(370,240);                                                        
  resize( 370, 240 );
  setCaption(i18n("Statistics..."));

}

void StatDlg::Init()
{
  QString s;
  QGroupBox* grp;
  QLabel *Label;

  grp = new QGroupBox(i18n("All time game statistics"), this);
  grp->resize(330,120);
  grp->move(20,10);


  // ---------- Names ------------------
  s=i18n("Name");
  Label=new QLabel(grp,"Name_1");
  Label->setGeometry(X1,Y1,60,30);
  Label->setText(s);

  s=name1;
  Label=new QLabel(grp,"Name_2");
  Label->setGeometry(X1,Y2,60,30);
  Label->setText(s);

  s=name2;
  Label=new QLabel(grp,"Name_3");
  Label->setGeometry(X1,Y3,60,30);
  Label->setText(s);

  // ---------- Wins ------------------
  s=i18n("Won");
  Label=new QLabel(grp,"Won_1");
  Label->setGeometry(X2,Y1,60,30);
  Label->setText(s);

  s.sprintf("%3d",win1);
  Label=new QLabel(grp,"Won_2");
  Label->setGeometry(X2,Y2,30,30);
  Label->setText(s);

  s.sprintf("%3d",win2);
  Label=new QLabel(grp,"Won_3");
  Label->setGeometry(X2,Y3,30,30);
  Label->setText(s);

  // ---------- Drawn ------------------
  s=i18n("Drawn");
  Label=new QLabel(grp,"Drawn_1");
  Label->setGeometry(X3,Y1,60,30);
  Label->setText(s);

  s.sprintf("%3d",remis1);
  Label=new QLabel(grp,"Drawn_2");
  Label->setGeometry(X3,Y2,30,30);
  Label->setText(s);

  s.sprintf("%3d",remis2);
  Label=new QLabel(grp,"Drawn_3");
  Label->setGeometry(X3,Y3,30,30);
  Label->setText(s);

  // ---------- Lost ------------------
  s=i18n("Lost");
  Label=new QLabel(grp,"Loss_1");
  Label->setGeometry(X4,Y1,60,30);
  Label->setText(s);

  s.sprintf("%3d",loose1);
  Label=new QLabel(grp,"Loss_2");
  Label->setGeometry(X4,Y2,30,30);
  Label->setText(s);

  s.sprintf("%3d",loose2);
  Label=new QLabel(grp,"Loss_3");
  Label->setGeometry(X4,Y3,30,30);
  Label->setText(s);

  // ---------- Breaked ------------------
  s=i18n("Aborted");
  Label=new QLabel(grp,"Abort_1");
  Label->setGeometry(X5,Y1,60,30);
  Label->setText(s);

  s.sprintf("%3d",brk1);
  Label=new QLabel(grp,"Abort_2");
  Label->setGeometry(X5,Y2,30,30);
  Label->setText(s);

  s.sprintf("%3d",brk2);
  Label=new QLabel(grp,"Abort_3");
  Label->setGeometry(X5,Y3,30,30);
  Label->setText(s);

  // ---------- Sum ------------------
  s=i18n("Sum");
  Label=new QLabel(grp,"Sum_1");
  Label->setGeometry(X6,Y1,30,30);
  Label->setText(s);

  s.sprintf("%3d",sum1);
  Label=new QLabel(grp,"Sum_2");
  Label->setGeometry(X6,Y2,30,30);
  Label->setText(s);

  s.sprintf("%3d",sum2);
  Label=new QLabel(grp,"Sum_3");
  Label->setGeometry(X6,Y3,30,30);
  Label->setText(s);



  QPushButton *PushButton;
  PushButton = new QPushButton( this, "PushButton_1" );
  PushButton->setGeometry( X0b, Y4, 105, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(clearStat()) );
  PushButton->setText( i18n("Clear all statistics" ));
  PushButton->setAutoRepeat( FALSE );
  PushButton->setAutoResize( FALSE );

  PushButton = new QPushButton( this, "PushButton_2" );
  PushButton->setGeometry( X0c, Y5, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(accept()) );
  PushButton->setText( i18n("OK" ));
  PushButton->setAutoRepeat( FALSE );
  PushButton->setAutoResize( FALSE );
  PushButton->setDefault( TRUE );
}

// The button clear statistics got pressed
void StatDlg::clearStat()
{
  int ret = KMessageBox::questionYesNo(this,
            i18n("Do you really want to delete the all time \n"
                 "statistics?\n"),
            i18n("Statistics warning"),
//            KMessageBox::QUESTION,
            i18n("Yes"),
            i18n("No"));

   if(ret == KMessageBox::Yes)
   {
     done(-100);
   }
}


void StatDlg::SetNames(QString n1,QString n2)
{
  name1=n1;
  name2=n2;
}
void StatDlg::SetStat1(int x1,int x2,int x3,int x4,int x5)
{
  win1=x1;
  remis1=x2;
  loose1=x3;
  brk1=x4;
  sum1=x5;
}
void StatDlg::SetStat2(int x1,int x2,int x3,int x4,int x5)
{
  win2=x1;
  remis2=x2;
  loose2=x3;
  brk2=x4;
  sum2=x5;
}
#include "statdlg.moc"
