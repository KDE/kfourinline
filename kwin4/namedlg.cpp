/****************************************************************************
** Form implementation generated from reading ui file 'namedlg.ui'
**
** Created: Thu Nov 23 11:10:17 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "namedlg.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>

#define NAME_MAX_LEN 12

/* 
 *  Constructs a NameDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
NameDlg::NameDlg( QWidget *parent, const char *name,bool /* modal */, WFlags /* fl */ )
    : KDialogBase( Plain, i18n("Configure names..."), Ok|Cancel, Ok,
                   parent, name, true,true )

{
  QWidget *page = plainPage();
    if ( !name ) setName( "NameDlg" );
    resize( 252, 186 ); 
//    setCaption( i18n( "Configure names..."  ) );
    vbox = new QVBoxLayout( page,spacingHint() ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    player_names = new QGroupBox( page, "player_names" );
    player_names->setTitle(i18n("Player names") );
    player_names->setColumnLayout(0, Qt::Vertical );
    player_names->layout()->setSpacing( 0 );
    player_names->layout()->setMargin( 0 );
    vbox_2 = new QVBoxLayout( player_names->layout() );
    vbox_2->setAlignment( Qt::AlignTop );
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

    vbox_3 = new QVBoxLayout; 
    vbox_3->setSpacing( 6 );
    vbox_3->setMargin( 0 );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );

    text_player1 = new QLabel( player_names, "text_player1" );
    text_player1->setText( i18n("Player 1" ) );
    hbox_2->addWidget( text_player1 );

    edit_player1 = new QLineEdit( player_names, "edit_player1" );
    edit_player1->setMaxLength( NAME_MAX_LEN );
    QWhatsThis::add(  edit_player1, i18n( "Enter a player's name" ) );

    hbox_2->addWidget( edit_player1 );
    vbox_3->addLayout( hbox_2 );

    hbox_3 = new QHBoxLayout; 
    hbox_3->setSpacing( 6 );
    hbox_3->setMargin( 0 );

    text_player2 = new QLabel( player_names, "text_player2" );
    text_player2->setText( i18n("Player 1" ) );
    hbox_3->addWidget( text_player2 );

    edit_player2 = new QLineEdit( player_names, "edit_player2" );
    edit_player2->setText( i18n( "Player 2"  ) );
    edit_player2->setMaxLength( NAME_MAX_LEN );
    QWhatsThis::add(  edit_player2, i18n( "Enter a player's name" ) );
    hbox_3->addWidget( edit_player2 );
    vbox_3->addLayout( hbox_3 );
    vbox_2->addLayout( vbox_3 );

    // left
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_3 );

    hbox->addWidget( player_names );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer );

    // top
    QSpacerItem* spacer_4 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer_4 );

    vbox->addLayout( hbox );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer_2 );

}

/*  
 *  Destroys the object and frees any allocated resources
 */
NameDlg::~NameDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

// In and output the name strings
void NameDlg::SetNames(QString n1, QString n2)
{
  edit_player1->setText( n1 );
  edit_player2->setText( n2 );
}
void NameDlg::GetNames(QString &n1, QString &n2)
{
  n1=edit_player1->text(  );
  n1.truncate(NAME_MAX_LEN);
  n2=edit_player2->text(  );
  n2.truncate(NAME_MAX_LEN);
}

#include "namedlg.moc"
