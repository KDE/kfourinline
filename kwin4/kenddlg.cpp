/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2000 by Martin Heni
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

#include "kenddlg.h"
#include "kwin4.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <klocale.h>


/* 
 *  Constructs a kEndDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
kEndDlg::kEndDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name ) setName( "kEndDlg" );
    //resize( 486, 238 ); 
    resize( 486, 180 ); 
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    Frame3 = new QFrame( this, "Frame3" );
    Frame3->setFrameShadow( QFrame::Sunken );
    Frame3->setFrameShape( QFrame::StyledPanel );
    vbox_2 = new QVBoxLayout( Frame3 ); 
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    vbox_3 = new QVBoxLayout; 
    vbox_3->setSpacing( 6 );
    vbox_3->setMargin( 0 );

    PixmapLabel1 = new QLabel( Frame3, "PixmapLabel1" );
    PixmapLabel1->setScaledContents( TRUE );
    vbox_3->addWidget( PixmapLabel1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_3->addItem( spacer );

    PushButton1 = new QPushButton( Frame3, "PushButton1" );
    PushButton1->setText( i18n( "Good Bye"  ) );
    vbox_3->addWidget( PushButton1 );
    hbox->addLayout( vbox_3 );

    Frame5 = new QFrame( Frame3, "Frame5" );
    Frame5->setFrameShadow( QFrame::Sunken );
    Frame5->setFrameShape( QFrame::Box );
    hbox_2 = new QHBoxLayout( Frame5 ); 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 11 );

    TextLabel1 = new QLabel( Frame5, "TextLabel1" );
    TextLabel1->setText( i18n( "Thanks for playing !\n\nCome back soon\n\n\nSure you really wanna quit?"  ) );
    TextLabel1->setAlignment( int( QLabel::AlignCenter ) );
    hbox_2->addWidget( TextLabel1 );
    hbox->addWidget( Frame5 );

    vbox_4 = new QVBoxLayout; 
    vbox_4->setSpacing( 6 );
    vbox_4->setMargin( 0 );

    PixmapLabel2 = new QLabel( Frame3, "PixmapLabel2" );
    PixmapLabel2->setScaledContents( TRUE );
    vbox_4->addWidget( PixmapLabel2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_4->addItem( spacer_2 );

    PushButton1_2 = new QPushButton( Frame3, "PushButton1_2" );
    PushButton1_2->setText( i18n( "Stay Here"  ) );
    vbox_4->addWidget( PushButton1_2 );
    hbox->addLayout( vbox_4 );
    vbox_2->addLayout( hbox );
    vbox->addWidget( Frame3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer_3 );

  connect( PushButton1, SIGNAL(clicked()), SLOT(accept()) );
  connect( PushButton1_2, SIGNAL(clicked()), SLOT(reject()) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
kEndDlg::~kEndDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void kEndDlg::SetPixmap(QPixmap &pixmap)
{
    PixmapLabel1->setPixmap( pixmap );
    PixmapLabel2->setPixmap( pixmap );
}
#include "kenddlg.moc"
