/****************************************************************************
** Form implementation generated from reading ui file 'network.ui'
**
** Created: Tue Jan 23 17:27:55 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "networkdlg.h"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <klocale.h>

/* 
 *  Constructs a NetworkDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
NetworkDlg::NetworkDlg( QWidget* parent,  const char* name, bool /*modal*/, WFlags /*fl*/ )
    : KDialogBase( Plain, i18n("Configure Network Options..."), Ok|Cancel, Ok,
                   parent, name, true,true )
{
    if ( !name ) setName( "NetworkDlg" );
    QWidget *page = plainPage();
    resize( 283, 207 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, sizePolicy().hasHeightForWidth() ) );
    grid = new QGridLayout( page ); 
    grid->setSpacing( 6 );
    grid->setMargin( 11 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    grid->addItem( spacer_2, 2, 1 );

    GroupBoxRemote = new QGroupBox( page, "GroupBoxRemote" );
    GroupBoxRemote->setTitle( i18n( "Remote Settings"  ) );
    GroupBoxRemote->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, GroupBoxRemote->sizePolicy().hasHeightForWidth() ) );
    GroupBoxRemote->setColumnLayout(0, Qt::Vertical );
    GroupBoxRemote->layout()->setSpacing( 0 );
    GroupBoxRemote->layout()->setMargin( 0 );
    vbox = new QVBoxLayout( GroupBoxRemote->layout() );
    vbox->setAlignment( Qt::AlignTop );
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    ButtonGroup2 = new QButtonGroup( GroupBoxRemote, "ButtonGroup2" );
    ButtonGroup2->setFrameShape( QButtonGroup::NoFrame );
    ButtonGroup2->setColumnLayout(0, Qt::Vertical );
    ButtonGroup2->layout()->setSpacing( 0 );
    ButtonGroup2->layout()->setMargin( 0 );
    vbox_2 = new QVBoxLayout( ButtonGroup2->layout() );
    vbox_2->setAlignment( Qt::AlignTop );
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

#ifdef KWIN4_KDE2_2
/* uncommented because of gettext
    radioSlave = new QRadioButton( ButtonGroup2, "radioSlave" );
    radioSlave->setText( i18n( "Game client"  ) );
    radioSlave->setChecked( TRUE );
    vbox_2->addWidget( radioSlave );

    radioMaster = new QRadioButton( ButtonGroup2, "radioMaster" );
    radioMaster->setText( i18n( "Game master"  ) );
    vbox_2->addWidget( radioMaster );
*/
#else

    CheckBox1 = new QCheckBox( ButtonGroup2, "CheckBox1" );
    CheckBox1->setGeometry( QRect( 29, 34, 221, 21 ) ); 
    CheckBox1->setText( i18n( "Connect to a remote game"  ) );
    CheckBox1->setChecked( TRUE );
    vbox_2->addWidget( CheckBox1 );
#endif
    vbox->addWidget( ButtonGroup2 );


    grid_2 = new QGridLayout; 
    grid_2->setSpacing( 6 );
    grid_2->setMargin( 0 );

    remoteHost = new QLineEdit( GroupBoxRemote, "remoteHost" );
    remoteHost->setText( QString( "localhost"  ) );
    grid_2->addWidget( remoteHost, 0, 1 );

    TextLabelHost = new QLabel( GroupBoxRemote, "TextLabelHost" );
    TextLabelHost->setText( i18n( "Remote host"  ) );
    grid_2->addWidget( TextLabelHost, 0, 0 );

    TextLabelPort = new QLabel( GroupBoxRemote, "TextLabelPort" );
    TextLabelPort->setText( i18n( "Port"  ) );
    grid_2->addWidget( TextLabelPort, 1, 0 );

    port = new QLineEdit( GroupBoxRemote, "port" );
    grid_2->addWidget( port, 1, 1 );

    vbox->addLayout( grid_2 );

    grid->addWidget( GroupBoxRemote, 1, 1 );

    // signals and slots connections
#ifdef KWIN4_KDE2_2
    connect( radioSlave, SIGNAL( toggled(bool) ), remoteHost, SLOT( setEnabled(bool) ) );
    connect( radioMaster, SIGNAL( toggled(bool) ), remoteHost, SLOT( setDisabled(bool) ) );
#else
    connect( CheckBox1, SIGNAL( toggled(bool) ), remoteHost, SLOT( setEnabled(bool) ) );
#endif
}

/*  
 *  Destroys the object and frees any allocated resources
 */
NetworkDlg::~NetworkDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void NetworkDlg::SetHost(QString s)
{
  if (s.isEmpty()) s=QString("localhost");
  remoteHost->setText( s );
}
void NetworkDlg::SetPort(unsigned short mport)
{
  QString s;
  s.setNum(mport);
  port->setText(s);
}
unsigned short NetworkDlg::QueryPort()
{
  QString s;
  unsigned short mport;
  s=port->text();
  mport=(unsigned short)s.toInt();
  return mport;
}
QString NetworkDlg::QueryHost()
{
  QString s;
#ifdef KWIN4_KDE2_2
  if (!radioMaster->isChecked()) s=remoteHost->text();
#else
  if (CheckBox1->isChecked()) s=remoteHost->text();
#endif
  else s=QString("");
  return s;
}

#include "networkdlg.moc"
