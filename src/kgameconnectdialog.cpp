/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)
    SPDX-FileCopyrightText: 2001 Martin Heni (kde at heni-online.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kgameconnectdialog.h"

// own
#include "kfourinline_debug.h"
// KF
#include <kdnssd_version.h>
#if KDNSSD_VERSION >= QT_VERSION_CHECK(5, 84, 0)
#include <KDNSSD/ServiceBrowser>
#else
#include <DNSSD/ServiceBrowser>
#endif
#include <KLocalizedString>
// Qt
#include <QButtonGroup>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QSpinBox>
#include <QRadioButton>
#include <QVBoxLayout>

class KGameConnectWidgetPrivate
{
 public:
	KGameConnectWidgetPrivate()
	{
		mPort = nullptr;
		mHost = nullptr;
		mButtonGroup = nullptr;
		mBrowser = nullptr;
	}

	QSpinBox* mPort;
	QLineEdit* mHost;
	QButtonGroup* mButtonGroup;
	QComboBox *mClientName;
	QLabel *mClientNameLabel;
	KDNSSD::ServiceBrowser *mBrowser;
	QLabel *mServerNameLabel;
	QLineEdit *mServerName;
	QString mType;
};

KGameConnectWidget::KGameConnectWidget(QWidget* parent) : QWidget(parent)
{
 d = new KGameConnectWidgetPrivate;

 QVBoxLayout* vb = new QVBoxLayout(this);
 vb->setContentsMargins(0, 0, 0, 0);
 QGroupBox* box = new QGroupBox(this);
 QVBoxLayout* boxlay = new QVBoxLayout(box);
 d->mButtonGroup = new QButtonGroup(this);
 d->mButtonGroup->setExclusive(true);
 vb->addWidget(box);
 connect(d->mButtonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &KGameConnectWidget::slotTypeChanged);
 QRadioButton* buttonCreate = new QRadioButton(i18n("Create a network game"), box);
 boxlay->addWidget(buttonCreate);
 d->mButtonGroup->addButton(buttonCreate,0);
 QRadioButton* buttonJoin = new QRadioButton(i18n("Join a network game"), box);
 boxlay->addWidget(buttonJoin);
 d->mButtonGroup->addButton(buttonJoin,1);

 QWidget *g = new QWidget(this);
 vb->addWidget(g);
 d->mServerNameLabel = new QLabel(i18n("Game name:"), g);
 d->mServerName = new QLineEdit(g);
 d->mClientNameLabel = new QLabel(i18n("Network games:"), g);
 d->mClientName = new QComboBox(g);

 QGridLayout* layout = new QGridLayout(g);
 layout->setContentsMargins(0, 0, 0, 0);
 layout->addWidget(d->mServerNameLabel, 0, 0);
 layout->addWidget(d->mServerName, 0, 1);
 layout->addWidget(d->mClientNameLabel, 1, 0);
 layout->addWidget(d->mClientName, 1, 1);
 connect(d->mClientName, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &KGameConnectWidget::slotGameSelected);
 QLabel* label = new QLabel(i18n("Port to connect to:"), g);
 d->mPort = new QSpinBox(g);
 d->mPort->setMinimum(1024);
 d->mPort->setMaximum(65535);
 layout->addWidget(label, 2, 0);
 layout->addWidget(d->mPort, 2, 1);
 label = new QLabel(i18n("Host to connect to:"), g);
 d->mHost = new QLineEdit(g);
 layout->addWidget(label, 3, 0);
 layout->addWidget(d->mHost, 3, 1);

 QPushButton *button=new QPushButton(i18n("&Start Network"), this);
 connect(button, &QPushButton::clicked, this, &KGameConnectWidget::signalNetworkSetup);
 vb->addWidget(button);
 // Hide until type is set
 d->mClientName->hide();
 d->mClientNameLabel->hide();
 d->mServerName->hide();
 d->mServerNameLabel->hide();
}

void KGameConnectWidget::showDnssdControls() 
{
 if (!d->mBrowser) return;
 if (d->mHost->isEnabled()) {      // client	
  d->mClientName->show();
  d->mClientNameLabel->show();
  d->mServerName->hide();
  d->mServerNameLabel->hide();
  slotGameSelected(d->mClientName->currentIndex());
 } else {
  d->mClientName->hide();
  d->mClientNameLabel->hide();
  d->mServerName->show();
  d->mServerNameLabel->show();
 }
}

void KGameConnectWidget::setType(const QString& type) 
{
 d->mType = type;
 delete d->mBrowser;
 d->mBrowser = new KDNSSD::ServiceBrowser(type);
 connect(d->mBrowser, &KDNSSD::ServiceBrowser::finished, this, &KGameConnectWidget::slotGamesFound);
 d->mBrowser->startBrowse();
 showDnssdControls();
}

void KGameConnectWidget::slotGamesFound()
{
 bool autoselect=false;
 if (!d->mClientName->count()) autoselect=true;
 d->mClientName->clear();
 QStringList names;
 
 QListIterator<KDNSSD::RemoteService::Ptr> it(d->mBrowser->services());
 while (it.hasNext())
  names << it.next()->serviceName();
 d->mClientName->addItems(names);
 if (autoselect && d->mClientName->count()) slotGameSelected(0);
}

void KGameConnectWidget::setName(const QString& name) 
{
 d->mServerName->setText(name);
}

QString KGameConnectWidget::gameName() const
{
 return d->mServerName->text();
}

QString KGameConnectWidget::type() const
{
 return d->mType;
}

void KGameConnectWidget::slotGameSelected(int nr)
{
 if (nr>=(d->mBrowser->services().count()) || nr<0) return;
 if (!d->mHost->isEnabled()) return; // this is server mode, do not overwrite host and port controls
 KDNSSD::RemoteService::Ptr srv = d->mBrowser->services()[nr];
 if (!srv->isResolved() && !srv->resolve()) return;
 d->mHost->setText(srv->hostName());
 d->mPort->setValue(srv->port());
}
KGameConnectWidget::~KGameConnectWidget()
{
 delete d->mBrowser;
 delete d;
}

QString KGameConnectWidget::host() const
{ 
 if (d->mHost->isEnabled()) {
	return d->mHost->text();
 } else {
	return QString();
 }
}

unsigned short int KGameConnectWidget::port() const
{
 return d->mPort->value(); 
}

void KGameConnectWidget::setHost(const QString& host)
{ 
 d->mHost->setText(host); 
}

void KGameConnectWidget::setPort(unsigned short int port)
{
 d->mPort->setValue(port); 
}

void KGameConnectWidget::setDefault(int state)
{
 QAbstractButton *button = d->mButtonGroup->button(state);
 if (button == nullptr) {
  qCCritical(KFOURINLINE_LOG) << "KGameConnectWidget::setDefault" << state;
  return;
 } 
 button->setChecked(true);
 slotTypeChanged(button);
}

void KGameConnectWidget::slotTypeChanged(QAbstractButton *button)
{
 if (button) {
 const int t = d->mButtonGroup->id(button);
 if (t == 0) {
	d->mHost->setEnabled(false);
 } else if (t == 1) {
	d->mHost->setEnabled(true);
 }
 showDnssdControls();
 Q_EMIT signalServerTypeChanged(t);
 }
}



