/*
    This file is part of the KDE games library
    Copyright (C) 2001 Andreas Beckermann (b_mann@gmx.de)
    Copyright (C) 2001 Martin Heni (kde at heni-online.de)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kgamedialogconfig.h"

#include "kgameconnectdialog.h"

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgame.h>
#include <libkdegamesprivate/kgame/kgamechat.h>
#include <libkdegamesprivate/kgame/kplayer.h>
#include <libkdegamesprivate/kgame/kgameproperty.h>


#include <QListWidget>
#include <KLocalizedString>
#include <knuminput.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <klineedit.h>

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <KDebug>



class KGameDialogConfigPrivate
{
public:
	KGameDialogConfigPrivate()
	{
		mOwner = 0;
		mGame = 0;

		mAdmin = false;
	}

	bool mAdmin;
	KGame* mGame;
	KPlayer* mOwner;
};

KGameDialogConfig::KGameDialogConfig(QWidget* parent)
    : QWidget( parent ),
      d( new KGameDialogConfigPrivate )
{
}

KGameDialogConfig::~KGameDialogConfig()
{
 kDebug(11001) ;
 delete d;
}

void KGameDialogConfig::setKGame(KGame* g)
{
 d->mGame = g;
}

void KGameDialogConfig::setOwner(KPlayer* p)
{
 d->mOwner = p;
}

void KGameDialogConfig::setAdmin(bool a)
{
 d->mAdmin = a;
}

KGame* KGameDialogConfig::game() const
{ return d->mGame; }
bool KGameDialogConfig::admin() const
{ return d->mAdmin; }
KPlayer* KGameDialogConfig::owner() const
{ return d->mOwner; }

/////////////////////////// KGameDialogNetworkConfig /////////////////////////
class KGameDialogNetworkConfigPrivate
{
public:
	KGameDialogNetworkConfigPrivate()
	{
		mInitConnection = 0;
		mNetworkLabel = 0;
		mDisconnectButton = 0;
		mConnect = 0;
		mDefaultServer=true;

	}

	// QPushButton* mInitConnection;
	QGroupBox* mInitConnection;
	QLabel* mNetworkLabel;
	QPushButton *mDisconnectButton;

	bool mDefaultServer;
	QString mDefaultHost;
	unsigned short int mDefaultPort;
	KGameConnectWidget *mConnect;
};


KGameDialogNetworkConfig::KGameDialogNetworkConfig(QWidget* parent)
		: KGameDialogConfig(parent)
{
// kDebug(11001) << ": this=" << this;
 d = new KGameDialogNetworkConfigPrivate();

 QVBoxLayout* topLayout = new QVBoxLayout(this);
 topLayout->setMargin( KDialog::marginHint() );
 topLayout->setSpacing( KDialog::spacingHint() );

 QHBoxLayout *hb = new QHBoxLayout;
 hb->setSpacing( KDialog::spacingHint() );
 topLayout->addLayout(hb);

 d->mNetworkLabel = new QLabel(this);
 hb->addWidget(d->mNetworkLabel);

 d->mDisconnectButton=new QPushButton(i18n("Disconnect"),this);
 connect(d->mDisconnectButton, &QPushButton::clicked, this, &KGameDialogNetworkConfig::slotExitConnection);
 hb->addWidget(d->mDisconnectButton);

 d->mInitConnection = new QGroupBox(i18n("Network Configuration"), this);
 QHBoxLayout* gboxLay = new QHBoxLayout(d->mInitConnection);
 topLayout->addWidget(d->mInitConnection);

 d->mConnect = new KGameConnectWidget(d->mInitConnection);
 gboxLay->addWidget(d->mConnect);
 connect(d->mConnect, &KGameConnectWidget::signalNetworkSetup, this, &KGameDialogNetworkConfig::slotInitConnection);
 connect(d->mConnect, &KGameConnectWidget::signalServerTypeChanged, this, &KGameDialogNetworkConfig::signalServerTypeChanged);

 // Needs to be AFTER the creation of the dialogs
 setConnected(false);
 setDefaultNetworkInfo(QLatin1String( "localhost" ), 7654,true);
}

KGameDialogNetworkConfig::~KGameDialogNetworkConfig()
{
 kDebug(11001) ;
 delete d;
}

void KGameDialogNetworkConfig::slotExitConnection()
{
 kDebug(11001) << " !!!!!!!!!!!!!!!!!!!!!!!";
  if (game()) game()->disconnect();
  setConnected(false,false);
}

void KGameDialogNetworkConfig::slotInitConnection()
{
 kDebug(11001) ;
 bool connected = false;
 bool master = true;
 unsigned short int port = d->mConnect->port();
 QString host = d->mConnect->host();

 if (host.isNull()) {
	master = true;
	if (game()) {
		game()->setDiscoveryInfo(d->mConnect->type(),d->mConnect->gameName());
		connected = game()->offerConnections(port);
	}
 } else {
	master = false;
	if (game()) {
		connected = game()->connectToServer(host, port);
	}
  // We need to learn about failed connections
  if (game()) {
     connect(game(), SIGNAL(signalConnectionBroken()),
      this, SLOT(slotConnectionBroken()));
  }
 }
 setConnected(connected, master);
}

void KGameDialogNetworkConfig::slotConnectionBroken()
{
  kDebug(11001) ;
  setConnected(false,false);
  KMessageBox::error(this, i18n("Cannot connect to the network"));
}

void KGameDialogNetworkConfig::setConnected(bool connected, bool master)
{
 if (!connected) {
	d->mNetworkLabel->setText(i18n("Network status: No Network"));
	d->mInitConnection->setEnabled(true);
  d->mDisconnectButton->setEnabled(false);
	return;
 }
 if (master) {
	d->mNetworkLabel->setText(i18n("Network status: You are MASTER"));
 } else {
	d->mNetworkLabel->setText(i18n("Network status: You are connected"));
 }
 d->mInitConnection->setEnabled(false);
 d->mDisconnectButton->setEnabled(true);
}

void KGameDialogNetworkConfig::submitToKGame(KGame* , KPlayer* )
{
}

void KGameDialogNetworkConfig::setKGame(KGame* g)
{
 KGameDialogConfig::setKGame(g);
 if (!game()) {
	setConnected(false);
	return;
 }
 setConnected(game()->isNetwork(), game()->isMaster());
}

void KGameDialogNetworkConfig::setDefaultNetworkInfo(const QString& host, unsigned short int port,bool server)
{
 d->mDefaultPort = port;
 d->mDefaultHost = host;
 d->mDefaultServer = server;

 d->mConnect->setHost(host);
 d->mConnect->setPort(port);
 if (server) {
	d->mConnect->setDefault(0);
 } else {
	d->mConnect->setDefault(1);
 }
}

void KGameDialogNetworkConfig::setDiscoveryInfo(const QString& type, const QString& name)
{
 d->mConnect->setType(type);
 d->mConnect->setName(name);
}




