/***************************************************************************
                          Namedlg  -  Change player names
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
    Namedlg.cpp

    $Id$
    
    Name dialog for player names
    
    (c) Martin Heni, martin@heni-online.de
	      June 1999
    
    License: GPL

*/

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <klocale.h>
#include "namedlg.h"


#define NAME_MAX_LEN 9



// Create the dialog for changing the player names
NameDlg::NameDlg( QWidget *parent, const char *name,const char *sufi )
    : QDialog( parent, name,TRUE )
{
  KApplication *app=KApplication::kApplication();
  config=app->sessionConfig();
 
  setCaption(i18n("Configure names..."));
  setMinimumSize(200,160);
  setMaximumSize(200,160);                                                        
  resize( 200, 160 );

  QGroupBox* grp;
  grp = new QGroupBox(i18n("Player names"), this);
  grp->resize(180,100);
  grp->move(10,10);

  QLabel *Label;
  Label=new QLabel(grp,"Name_1");
  Label->setGeometry(10,20,60,30);
  Label->setText(i18n("Yellow" ));  

  NameEdit1 = new QLineEdit( grp, "LineEdit_1" );
  NameEdit1->setGeometry( 70, 20, 100, 30 );
  NameEdit1->setText( QCString("") );
  NameEdit1->setMaxLength(NAME_MAX_LEN);


  Label=new QLabel(grp,"Name_2");
  Label->setGeometry(10,60,60,30);
  Label->setText(i18n("Red" ));  

  NameEdit2 = new QLineEdit( grp, "LineEdit_2" );
  NameEdit2->setGeometry( 70, 60, 100, 30 );
  NameEdit2->setText( QCString("" ));
  NameEdit2->setMaxLength(NAME_MAX_LEN);


  QPushButton *PushButton;
  PushButton = new QPushButton( this, "PushButton_1" );
  PushButton->setGeometry( 20, 120, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(accept()) );
  PushButton->setText( i18n("Ok" ));
  PushButton->setAutoRepeat( FALSE );
  PushButton->setAutoResize( FALSE );

  PushButton = new QPushButton( this, "PushButton_2" );
  PushButton->setGeometry( 105, 120, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(reject()) );
  PushButton->setText( i18n("Cancel" ));
  PushButton->setAutoRepeat( FALSE );
  PushButton->setAutoResize( FALSE );

  // setBackgroundColor(DLGBACK);
}

// In and output the name strings
void NameDlg::SetNames(QString n1, QString n2)
{
  NameEdit1->setText( n1 );
  NameEdit2->setText( n2 );
}
void NameDlg::GetNames(QString &n1, QString &n2)
{
  n1=NameEdit1->text(  );
  n1.truncate(NAME_MAX_LEN);
  n2=NameEdit2->text(  );
  n2.truncate(NAME_MAX_LEN);
}





#include "namedlg.moc"
