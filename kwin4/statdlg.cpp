/***************************************************************************
                          statdlg .- kwin4
                             -------------------
    begin                : Thu Mar 30 2000
    copyright            : (C) |1995-2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/
#include "statdlg.h"

#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kseparator.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

/* 
 *  Constructs a StatDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
StatDlg::StatDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name ) setName( "StatDlg" );
    resize( 596, 512 ); 
    setCaption(i18n("Statistics"));

    Frame3 = new QFrame( this, "Frame3" );
    Frame3->setGeometry( QRect( 30, 10, 511, 461 ) ); 
    Frame3->setFrameShadow( QFrame::Sunken );
    Frame3->setFrameShape( QFrame::StyledPanel );



    QWidget* Layout17 = new QWidget( Frame3, "Layout17" );
    Layout17->setGeometry( QRect( 15, 12, 452, 406 ) ); 
    vbox = new QVBoxLayout( Layout17 ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 0 );

    GroupBox1 = new QGroupBox( Layout17, "GroupBox1" );
    GroupBox1->setTitle( i18n( "All Time Game Statistics"  ) );

    QWidget* Layout10 = new QWidget( GroupBox1, "Layout10" );
    Layout10->setGeometry( QRect( 10, 30, 399, 113 ) ); 
    hbox = new QHBoxLayout( Layout10 ); 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    vbox_2 = new QVBoxLayout; 
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 0 );

    TextLabel1 = new QLabel( Layout10, "TextLabel1" );
    TextLabel1->setText( i18n( "Name"  ) );
    vbox_2->addWidget( TextLabel1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_2->addItem( spacer );

    TextPlayer1 = new QLabel( Layout10, "TextPlayer1" );
    TextPlayer1->setText( i18n( "Player 1"  ) );
    vbox_2->addWidget( TextPlayer1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_2->addItem( spacer_2 );

    TextPlayer2 = new QLabel( Layout10, "TextPlayer2" );
    TextPlayer2->setText( i18n( "Player 2"  ) );
    vbox_2->addWidget( TextPlayer2 );
    hbox->addLayout( vbox_2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_3 );

    vbox_3 = new QVBoxLayout; 
    vbox_3->setSpacing( 6 );
    vbox_3->setMargin( 0 );

    TextLabel2 = new QLabel( Layout10, "TextLabel2" );
    TextLabel2->setText( i18n( "Won"  ) );
    vbox_3->addWidget( TextLabel2 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_3->addItem( spacer_4 );

    TextWon1 = new QLabel( Layout10, "TextWon1" );
    TextWon1->setText( "0"   );
    vbox_3->addWidget( TextWon1 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_3->addItem( spacer_5 );

    TextWon2 = new QLabel( Layout10, "TextWon2" );
    TextWon2->setText(  "0"   );
    vbox_3->addWidget( TextWon2 );
    hbox->addLayout( vbox_3 );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_6 );

    vbox_4 = new QVBoxLayout; 
    vbox_4->setSpacing( 6 );
    vbox_4->setMargin( 0 );

    TextLabel3 = new QLabel( Layout10, "TextLabel3" );
    TextLabel3->setText( i18n( "Drawn"  ) );
    vbox_4->addWidget( TextLabel3 );
    QSpacerItem* spacer_7 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_4->addItem( spacer_7 );

    TextDrawn1 = new QLabel( Layout10, "TextDrawn1" );
    TextDrawn1->setText(  "0"   );
    vbox_4->addWidget( TextDrawn1 );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_4->addItem( spacer_8 );

    TextDrawn2 = new QLabel( Layout10, "TextDrawn2" );
    TextDrawn2->setText(  "0"   );
    vbox_4->addWidget( TextDrawn2 );
    hbox->addLayout( vbox_4 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_9 );

    vbox_5 = new QVBoxLayout; 
    vbox_5->setSpacing( 6 );
    vbox_5->setMargin( 0 );

    TextLabel4 = new QLabel( Layout10, "TextLabel4" );
    TextLabel4->setText( i18n( "Lost"  ) );
    vbox_5->addWidget( TextLabel4 );
    QSpacerItem* spacer_10 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_5->addItem( spacer_10 );

    TextLost1 = new QLabel( Layout10, "TextLost1" );
    TextLost1->setText( "0"   );
    vbox_5->addWidget( TextLost1 );
    QSpacerItem* spacer_11 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_5->addItem( spacer_11 );

    TextLost2 = new QLabel( Layout10, "TextLost2" );
    TextLost2->setText(  "0"   );
    vbox_5->addWidget( TextLost2 );
    hbox->addLayout( vbox_5 );
    QSpacerItem* spacer_12 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_12 );

    vbox_6 = new QVBoxLayout; 
    vbox_6->setSpacing( 6 );
    vbox_6->setMargin( 0 );

    TextLabel5 = new QLabel( Layout10, "TextLabel5" );
    TextLabel5->setText( i18n( "Aborted"  ) );
    vbox_6->addWidget( TextLabel5 );
    QSpacerItem* spacer_13 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_6->addItem( spacer_13 );

    TextAborted1 = new QLabel( Layout10, "TextAborted1" );
    TextAborted1->setText(  "0"   );
    vbox_6->addWidget( TextAborted1 );
    QSpacerItem* spacer_14 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_6->addItem( spacer_14 );

    TextAborted2 = new QLabel( Layout10, "TextAborted2" );
    TextAborted2->setText(  "0"   );
    vbox_6->addWidget( TextAborted2 );
    hbox->addLayout( vbox_6 );
    QSpacerItem* spacer_15 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_15 );

    vbox_7 = new QVBoxLayout; 
    vbox_7->setSpacing( 6 );
    vbox_7->setMargin( 0 );

    TextLabel6 = new QLabel( Layout10, "TextLabel6" );
    TextLabel6->setText( i18n( "Sum"  ) );
    vbox_7->addWidget( TextLabel6 );
    QSpacerItem* spacer_16 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_7->addItem( spacer_16 );

    TextSum1 = new QLabel( Layout10, "TextSum1" );
    TextSum1->setText(  "0"   );
    vbox_7->addWidget( TextSum1 );
    QSpacerItem* spacer_17 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox_7->addItem( spacer_17 );

    TextSum2 = new QLabel( Layout10, "TextSum2" );
    TextSum2->setText(  "0"   );
    vbox_7->addWidget( TextSum2 );
    hbox->addLayout( vbox_7 );
    QSpacerItem* spacer_18 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox->addItem( spacer_18 );
    vbox->addWidget( GroupBox1 );
    QSpacerItem* spacer_19 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer_19 );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );
    QSpacerItem* spacer_20 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_20 );

    PixmapLabel2 = new QLabel( Layout17, "PixmapLabel2" );
    PixmapLabel2->setScaledContents( TRUE );
    hbox_2->addWidget( PixmapLabel2 );
    QSpacerItem* spacer_21 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_21 );

    PushButton1 = new QPushButton( Layout17, "PushButton1" );
    PushButton1->setText( i18n( "Clear All Statistics"  ) );
    hbox_2->addWidget( PushButton1 );
    QSpacerItem* spacer_22 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_22 );

    PixmapLabel1 = new QLabel( Layout17, "PixmapLabel1" );
    PixmapLabel1->setScaledContents( TRUE );
    hbox_2->addWidget( PixmapLabel1 );
    QSpacerItem* spacer_23 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_2->addItem( spacer_23 );
    vbox->addLayout( hbox_2 );
    QSpacerItem* spacer_24 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer_24 );

    Line1 = new KSeparator( KSeparator::HLine, Layout17);
    vbox->addWidget( Line1 );
    QSpacerItem* spacer_25 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer_25 );

    hbox_3 = new QHBoxLayout; 
    hbox_3->setSpacing( 6 );
    hbox_3->setMargin( 0 );
    QSpacerItem* spacer_26 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_3->addItem( spacer_26 );

    PushButton2 = new KPushButton( KStdGuiItem::ok(), Layout17, "PushButton2" );
    hbox_3->addWidget( PushButton2 );
    QSpacerItem* spacer_27 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    hbox_3->addItem( spacer_27 );
    vbox->addLayout( hbox_3 );

    // signals and slots connections
    connect( PushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( PushButton1, SIGNAL(clicked()), SLOT(clearStat()) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
StatDlg::~StatDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

// The button clear statistics got pressed
void StatDlg::clearStat()
{
  int ret = KMessageBox::questionYesNo(this,
            i18n("Do you really want to delete the all time "
                 "statistics?\n"),
            i18n("Statistics Warning"));

   if(ret == KMessageBox::Yes)
   {
     done(-100);
   }
}

void StatDlg::SetPixmap(QPixmap &pixmap)
{
   PixmapLabel1->setPixmap( pixmap );
   PixmapLabel2->setPixmap( pixmap );
}

void StatDlg::SetNames(QString n1,QString n2)
{
  TextPlayer1->setText( n1 );
  TextPlayer2->setText( n2 );
}
void StatDlg::SetStat1(int x1,int x2,int x3,int x4,int x5)
{
  QString s;
    TextWon1->setText( s.setNum(x1) );
    TextDrawn1->setText( s.setNum(x2)  );
    TextLost1->setText( s.setNum(x3)   );
    TextAborted1->setText(  s.setNum(x4)   );
    TextSum1->setText(  s.setNum(x5)   );
}
void StatDlg::SetStat2(int x1,int x2,int x3,int x4,int x5)
{
  QString s;
    TextWon2->setText( s.setNum(x1)   );
    TextDrawn2->setText(  s.setNum(x2)   );
    TextLost2->setText( s.setNum(x3)   );
    TextAborted2->setText(  s.setNum(x4)   );
    TextSum2->setText(  s.setNum(x5)   );
}
#include "statdlg.moc"
