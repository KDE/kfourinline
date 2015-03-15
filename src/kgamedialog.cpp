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

#include "kgamedialog.h"

//Added by qt3to4:
#include <QList>
#include <QPushButton>

#include <KLocalizedString>
#include <kvbox.h>

#include "kfourinline_debug.h"

#include "kgamedialogconfig.h"

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgame.h>



class KGameDialogPrivate
{
public:
	KGameDialogPrivate()
	{
		mNetworkPage = 0;

		mNetworkConfig = 0;

		mOwner = 0;
		mGame = 0;
	}

	KVBox* mNetworkPage;
	KGameDialogNetworkConfig* mNetworkConfig;

// a list of all config widgets added to this dialog
	QList<KGameDialogConfig*> mConfigWidgets;

// just pointers:
	KPlayer* mOwner;
	KGame* mGame;
};

KGameDialog::KGameDialog(KGame* g, KPlayer* owner, const QString& title,
		QWidget* parent)
    : KPageDialog(parent),
      d( new KGameDialogPrivate )
{
    setWindowTitle(title);
    //QT5 setButtons(Ok|Default|Apply|Cancel);
    //QT5 setDefaultButton(Ok);
    setFaceType(KPageDialog::Tabbed);
    setModal(true);
 init(g, owner);
 addNetworkConfig(new KGameDialogNetworkConfig(0));
 connect(button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &KGameDialog::slotOk);
 connect(button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &KGameDialog::slotDefault);
 connect(button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &KGameDialog::slotApply);
}

void KGameDialog::init(KGame* g, KPlayer* owner)
{
//AB: do we need a "Cancel" Button? currently removed

// qCDebug(KFOURINLINE_LOG) << ": this=" << this;

 setOwner(owner);
 setKGame(g);
 if (g) {
	setAdmin(g->isAdmin());
 } else {
	setAdmin(false);
 }
}

KGameDialog::~KGameDialog()
{
// qCDebug(KFOURINLINE_LOG) << "DESTRUCT KGameDialog" << this;
 qDeleteAll(d->mConfigWidgets);
 delete d;
}

void KGameDialog::addNetworkConfig(KGameDialogNetworkConfig* netConf)
{
 if (!netConf) {
	return;
 }
 d->mNetworkConfig = netConf;
 d->mNetworkPage = addConfigPage(netConf, i18n("&Network"));
}

KVBox *KGameDialog::configPage()
{
 return d->mNetworkPage;
}

KVBox* KGameDialog::addConfigPage(KGameDialogConfig* widget, const QString& title)
{
 if (!widget) {
	qCCritical(KFOURINLINE_LOG) << "Cannot add NULL config widget";
	return 0;
 }
 KVBox* page = new KVBox();
 addPage(page,title);
 addConfigWidget(widget, page);
 return page;
}

void KGameDialog::addConfigWidget(KGameDialogConfig* widget, QWidget* parent)
{
 if (!widget) {
	qCCritical(KFOURINLINE_LOG) << "Cannot add NULL config widget";
	return;
 }
 if (!parent) {
	qCCritical(KFOURINLINE_LOG) << "Cannot reparent to NULL widget";
	return;
 }
// qCDebug(KFOURINLINE_LOG) << "reparenting widget";
 widget->setParent(parent);
 widget->move(QPoint(0,0));
 d->mConfigWidgets.append(widget);
 connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(slotRemoveConfigWidget(QObject*)));
 if (!d->mGame) {
	qCWarning(KFOURINLINE_LOG) << "No game has been set!";
 } else {
	widget->setKGame(d->mGame);
	widget->setAdmin(d->mGame->isAdmin());
 }
 if (!d->mOwner) {
	qCWarning(KFOURINLINE_LOG) << "No player has been set!";
 } else {
	widget->setOwner(d->mOwner);
 }
 widget->show();
}

KGameDialogNetworkConfig* KGameDialog::networkConfig() const
{ return d->mNetworkConfig; }

void KGameDialog::slotApply()
{
 submitToKGame();
}

void KGameDialog::slotDefault()
{
 if (!d->mGame) {
	return;
 }

//TODO *only*  call setKGame/setOwner for the *current* page!!
 setKGame(d->mGame);
 setOwner(d->mOwner);
}

void KGameDialog::slotOk()
{
 slotApply();
 QDialog::accept();
}

void KGameDialog::setOwner(KPlayer* owner)
{
//AB: note: NULL player is ok!
 d->mOwner = owner;
 for (int i = 0; i < d->mConfigWidgets.count(); i++) {
	if (d->mConfigWidgets.at(i)) {
		d->mConfigWidgets.at(i)->setOwner(d->mOwner);
	} else {
		qCCritical(KFOURINLINE_LOG) << "NULL widget??";
	}
 }
}

void KGameDialog::setKGame(KGame* g)
{
 if (d->mGame) {
	disconnect(d->mGame, 0, this, 0);
 }
 d->mGame = g;
 for (int i = 0; i < d->mConfigWidgets.count(); i++) {
	d->mConfigWidgets.at(i)->setKGame(d->mGame);
 }
 if (d->mGame) {
	setAdmin(d->mGame->isAdmin());
	connect(d->mGame, &KGame::destroyed, this, &KGameDialog::slotUnsetKGame);
	connect(d->mGame, &KGame::signalAdminStatusChanged, this, &KGameDialog::setAdmin);
 }
}

void KGameDialog::setAdmin(bool admin)
{
 for (int i = 0; i < d->mConfigWidgets.count(); i++) {
	d->mConfigWidgets.at(i)->setAdmin(admin);
 }
}

void KGameDialog::slotUnsetKGame() // called when KGame is destroyed
{ setKGame(0); }

void KGameDialog::submitToKGame()
{
 if (!d->mGame) {
	qCCritical(KFOURINLINE_LOG) << ": no game has been set";
	return;
 }
 if (!d->mOwner) {
	qCCritical(KFOURINLINE_LOG) << ": no player has been set";
	return;
 }

 for (int i = 0; i < d->mConfigWidgets.count(); i++) {
// qCDebug(KFOURINLINE_LOG) << "submit to kgame" << i;
	d->mConfigWidgets.at(i)->submitToKGame(d->mGame, d->mOwner);
// qCDebug(KFOURINLINE_LOG) << "done: submit to kgame" << i;
 }
}

void KGameDialog::slotRemoveConfigWidget(QObject* configWidget)
{
 d->mConfigWidgets.removeAll((KGameDialogConfig*)configWidget);
}
