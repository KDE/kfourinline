/****************************************************************************
** Form implementation generated from reading ui file 'aboutDlg.ui'
**
** Created: Thu Nov 23 11:42:20 2000
**      by:  The User Interface Compiler (uic)
**
****************************************************************************/
#include "kwin4aboutdlg.h"

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

#define VERSION 0.91


/* 
 *  Constructs a aboutDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
aboutDlg::aboutDlg( QWidget* parent,  const char* name, bool /* modal */, WFlags /*fl*/ )
    : KDialogBase( Plain, i18n("About Four Wins"), Ok, Ok,
                   parent, name, true,true )
{
  QWidget *page = plainPage();
    if ( !name ) setName( "aboutDlg" );
    resize( 315, 318 ); 
    vbox = new QVBoxLayout( page ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    Frame5 = new QFrame( page, "Frame5" );
    Frame5->setFrameShadow( QFrame::Sunken );
    Frame5->setFrameShape( QFrame::StyledPanel );
    Frame5->setBackgroundColor(Qt::white);
    Frame5->setLineWidth( 3 );
    Frame5->setMidLineWidth( 2 );
    vbox_2 = new QVBoxLayout( Frame5 ); 
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    Label1= new QLabel(Frame5,"Pix1");
    Label1->setMinimumSize( QSize( 114, 64 ) );
    Label1->setMaximumSize( QSize( 114, 64 ) );
    hbox->addWidget( Label1 );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer );

    TextLabel3 = new QLabel( Frame5, "TextLabel3" );
    TextLabel3->setText( i18n( "Four Wins\n\nVersion %1"  ).arg(VERSION) );
    TextLabel3->setAlignment( int( QLabel::AlignCenter ) );
    TextLabel3->setBackgroundColor(Qt::white);
    hbox->addWidget( TextLabel3 );
    vbox_2->addLayout( hbox );

    TextLabel4 = new QLabel( Frame5, "TextLabel4" );
    TextLabel4->setText( i18n( "(c) 2000 Martin Heni\nEmail: martin@heni-online.de\n\nGamefeatures:\n  - Multiplayer network game\n  - Up to two Computerplayer\n\n\n Many thanks to Laura for betatesting!"  ) );
    TextLabel4->setBackgroundColor(Qt::white);
    vbox_2->addWidget( TextLabel4 );
    vbox->addWidget( Frame5 );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_2 );

}

/*  
 *  Destroys the object and frees any allocated resources
 */
aboutDlg::~aboutDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void aboutDlg::SetPixmap(QPixmap &pixmap)
{
  if (Label1) Label1->setPixmap(pixmap);
}

#include "kwin4aboutdlg.moc"
