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
    Networkdlg.cpp

    $Id$
    
    Confiugre game over IP
    
    (c) Martin Heni, martin@heni-online.de
	      June 1999
    
    License: GPL

*/

#include "networkdlg.h"
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <klocale.h>




// Create the dialog 
NetworkDlg::NetworkDlg( QWidget *parent, const char *name,const char *sufi )
    : QDialog( parent, name,TRUE )
{
  QGroupBox* grp;
  KApplication *app=KApplication::kApplication();
  config=app->sessionConfig();
  int port;
  //int sport;
  QString s;
  QLabel *Label;
  QPushButton *PushButton;

  setCaption(i18n("Configure network options..."));
  setMinimumSize(330,260);
  setMaximumSize(330,260);
  resize( 330, 260 );

  grp = new QGroupBox(i18n("Remote settings"), this);
  grp->resize(290,175);
  grp->move(20,10);

  IPEdit = new QLineEdit( grp, "Edit_2" );
  IPEdit->setGeometry( 95, 25, 170, 30 );
  IPEdit->setText(QCString( "localhost" ) );

  Label=new QLabel(grp,"&host");
  Label->setGeometry(10,25,85,30);
  Label->setText(i18n("Remote host" ));


  PortEdit = new QLineEdit( grp, "Edit_3" );
  PortEdit->setGeometry( 95, 60, 50, 30 );
  PortEdit->setText(QCString( "7442" ));
  PortEdit->setMaxLength(5);

  Label=new QLabel(grp,"&Port");
  Label->setGeometry(10,60,85,30);
  Label->setText(i18n("Port" ));

  Label=new QLabel(grp,"Label_3");
  Label->setGeometry(10,95,270,75);
  Label->setText(i18n("You can leave the remote host input "
  "empty.\nThen your computer acts as server only and\ndoes not "
  "try to connect to a remote host."));




  PushButton = new QPushButton( this, "PushButton_1" );
  PushButton->setGeometry( 130, 210, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(accept()) );
  PushButton->setText( i18n("OK" ));
  PushButton->setAutoRepeat( FALSE );
  PushButton->setAutoResize( FALSE );
  PushButton->setDefault( TRUE );

  /*
  PushButton = new QPushButton( this, "PushButton_2" );
  PushButton->setGeometry( 165, 210, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(reject()) );
  PushButton->setText( i18n("Cancel" ));
  PushButton->setAutoRepeat( FALSE );
  PushButton->setAutoResize( FALSE );
  */

 // setBackgroundColor(DLGBACK);
}

void NetworkDlg::accept()
{
  QDialog::accept();
}

void NetworkDlg::SetHost(QString s)
{
  IPEdit->setText( s );
}
void NetworkDlg::SetPort(unsigned short port)
{
  QString s;
  s.setNum(port);
  PortEdit->setText(s);
}
unsigned short NetworkDlg::QueryPort()
{
  QString s;
  unsigned short port;
  s=PortEdit->text();
  port=(unsigned short)s.toInt();
  return port;
}
QString NetworkDlg::QueryHost()
{
  return IPEdit->text();
}

#include "networkdlg.moc"
