/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)
    SPDX-FileCopyrightText: 2001 Martin Heni (kde at heni-online.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kgamedialog.h"

// own
#include "kfourinline_debug.h"
#include "kgamedialogconfig.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgame.h>
// KF
#include <KLocalizedString>
// Qt
#include <QList>
#include <QPushButton>



class KGameDialogPrivate
{
public:
	KGameDialogPrivate()
	{
		mNetworkPage = nullptr;

		mNetworkConfig = nullptr;

		mOwner = nullptr;
		mGame = nullptr;
	}

	QWidget* mNetworkPage;
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
 addNetworkConfig(new KGameDialogNetworkConfig());
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

QWidget* KGameDialog::configPage()
{
 return d->mNetworkPage;
}

QWidget* KGameDialog::addConfigPage(KGameDialogConfig* widget, const QString& title)
{
 if (!widget) {
	qCCritical(KFOURINLINE_LOG) << "Cannot add NULL config widget";
	return nullptr;
 }
 QWidget* page = new QWidget();
 QVBoxLayout *pageVBoxLayout = new QVBoxLayout(page);
 pageVBoxLayout->setContentsMargins(0, 0, 0, 0);
 pageVBoxLayout->addWidget(widget);
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
 d->mConfigWidgets.append(widget);
 connect(widget, &QObject::destroyed, this, &KGameDialog::slotRemoveConfigWidget);
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
	disconnect(d->mGame, nullptr, this, nullptr);
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
{ setKGame(nullptr); }

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

