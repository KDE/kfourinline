/***************************************************************************
                          Msgdlg  -  Send message to remote 
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
    Msgdlg.cpp

    $Id$
    
    Msg dialog for player names
    
    (c) Martin Heni, martin@heni-online.de
	      June 1999
    
    License: GPL

*/

#include <qgroupbox.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>
#include "msgdlg.h"



// Create the dialog for changing the player names
MsgDlg::MsgDlg( QWidget *parent, const char *name,const char * /*sufi*/ )
    : QDialog( parent, name,TRUE )
{
  setCaption(i18n("Send Message to Remote Player"));
  setMinimumSize(400,160);
  setMaximumSize(600,360);                                                        
  resize( 400, 160 );

  QGroupBox* grp;
  grp = new QGroupBox(i18n("Enter Message"), this);
  grp->resize(380,100);
  grp->move(10,10);

  MultiLine = new QMultiLineEdit( grp, "MLineEdit" );
  MultiLine->setGeometry( 10, 20, 360, 70 );

  QPushButton *PushButton;
  PushButton = new QPushButton( i18n("Send" ), this, "PushButton_1" );
  PushButton->setGeometry( 20, 120, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(accept()) );
  PushButton->setAutoRepeat( FALSE );

  PushButton = new KPushButton( KStdGuiItem::cancel(), this, "PushButton_2" );
  PushButton->setGeometry( 305, 120, 65, 30 );
  connect( PushButton, SIGNAL(clicked()), SLOT(reject()) );
  PushButton->setAutoRepeat( FALSE );
}

QString MsgDlg::GetMsg()
{
  return MultiLine->text();
}





#include "msgdlg.moc"
