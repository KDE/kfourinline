/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kchatdialog.h"

// own
#include "kfourinline_debug.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kchatbase.h>
// KF
#include <KLocalizedString>
// Qt
#include <QDialogButtonBox>
#include <QFontDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class KChatDialogPrivate
{
 public:
	KChatDialogPrivate()
	{
		mTextPage = nullptr;

		mNamePreview = nullptr;
		mTextPreview = nullptr;
		mSystemNamePreview = nullptr;
		mSystemTextPreview = nullptr;

		mChat = nullptr;
	}

	QFrame* mTextPage;

	QLabel* mNamePreview;
	QLabel* mTextPreview;
	QLabel* mSystemNamePreview;
	QLabel* mSystemTextPreview;

	QLineEdit* mMaxMessages;

	KChatBase* mChat;
};

KChatDialog::KChatDialog(KChatBase* chat, QWidget* parent, bool modal) 
    : QDialog(parent),
      d( new KChatDialogPrivate )
{
 setModal(modal);
 init();
 plugChatWidget(chat);
}

KChatDialog::KChatDialog(QWidget* parent, bool modal) 
    : QDialog(parent),
      d( new KChatDialogPrivate )
{
 setModal(modal);
 init();
}

KChatDialog::~KChatDialog()
{
 delete d;
}

void KChatDialog::init()
{
 d->mTextPage = new QFrame( this );
 QGridLayout* layout = new QGridLayout(d->mTextPage);

 setWindowTitle(i18nc("@title:window", "Configure Chat"));
 buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Apply);
 QWidget *mainWidget = new QWidget(this);
 setLayout(layout);
 layout->addWidget(mainWidget);
 QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
 okButton->setDefault(true);
 okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
 connect(buttonBox, &QDialogButtonBox::accepted, this, &KChatDialog::accept);
 connect(buttonBox, &QDialogButtonBox::rejected, this, &KChatDialog::reject);

// General fonts
 QPushButton* nameFont = new QPushButton(i18n("Name Font..."), d->mTextPage);
 connect(nameFont, &QPushButton::pressed, this, &KChatDialog::slotGetNameFont);
 layout->addWidget(nameFont, 0, 0);
 QPushButton* textFont = new QPushButton(i18n("Text Font..."), d->mTextPage);
 connect(textFont, &QPushButton::pressed, this, &KChatDialog::slotGetTextFont);
 layout->addWidget(textFont, 0, 1);

 QFrame* messagePreview = new QFrame(d->mTextPage);
 messagePreview->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
 QHBoxLayout* messageLayout = new QHBoxLayout(messagePreview);
 layout->addWidget(messagePreview, 1, 0, 1, 2);

 d->mNamePreview = new QLabel(i18n("Player: "), messagePreview);
 messageLayout->addWidget(d->mNamePreview, 0);
 d->mTextPreview = new QLabel(i18n("This is a player message"), messagePreview);
 messageLayout->addWidget(d->mTextPreview, 1);

 layout->addItem(new QSpacerItem(0, 10), 2, 0);
 
// System Message fonts
 QLabel* systemMessages = new QLabel(i18n("System Messages - Messages directly sent from the game"), d->mTextPage);
 layout->addWidget(systemMessages, 3, 0, 1, 2);
 QPushButton* systemNameFont = new QPushButton(i18n("Name Font..."), d->mTextPage);
 connect(systemNameFont, &QPushButton::pressed, this, &KChatDialog::slotGetSystemNameFont);
 layout->addWidget(systemNameFont, 4, 0);
 QPushButton* systemTextFont = new QPushButton(i18n("Text Font..."), d->mTextPage);
 connect(systemTextFont, &QPushButton::pressed, this, &KChatDialog::slotGetSystemTextFont);
 layout->addWidget(systemTextFont, 4, 1);

 QFrame* systemMessagePreview = new QFrame(d->mTextPage);
 systemMessagePreview->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
 QHBoxLayout* systemMessageLayout = new QHBoxLayout(systemMessagePreview);
 layout->addWidget(systemMessagePreview, 5, 0, 1, 2);
 
 d->mSystemNamePreview = new QLabel(i18n("--- Game: "), systemMessagePreview);
 systemMessageLayout->addWidget(d->mSystemNamePreview, 0);
 d->mSystemTextPreview = new QLabel(i18n("This is a system message"), systemMessagePreview);
 systemMessageLayout->addWidget(d->mSystemTextPreview, 1);

// message count
 QLabel* maxMessages = new QLabel(i18n("Maximum number of messages (-1 = unlimited):"), d->mTextPage);
 layout->addWidget(maxMessages, 6, 0);
 d->mMaxMessages = new QLineEdit(d->mTextPage);
 d->mMaxMessages->setText(QString::number(-1));
 layout->addWidget(d->mMaxMessages, 6, 1);

 layout->addWidget(buttonBox, 7, 0, 1, 2);
 connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &KChatDialog::slotApply);
 connect(okButton, &QPushButton::clicked, this, &KChatDialog::slotOk);
}

void KChatDialog::slotGetNameFont()
{
 QFont font = nameFont();
 bool ok;
 font = QFontDialog::getFont(&ok, font, this);
 if (ok)
    setNameFont(font);
}

void KChatDialog::slotGetTextFont()
{
 QFont font = textFont();
 bool ok;
 font = QFontDialog::getFont(&ok, font, this);
 if (ok)
   setTextFont(font);
}

void KChatDialog::slotGetSystemNameFont()
{
 QFont font = systemNameFont();
 bool ok;
 font = QFontDialog::getFont(&ok, font, this);
 if (ok)
   setSystemNameFont(font);
}

void KChatDialog::slotGetSystemTextFont()
{
 QFont font = systemTextFont();
 bool ok;
 font = QFontDialog::getFont(&ok, font, this);
 if (ok)
   setSystemTextFont(font);
}

QFont KChatDialog::nameFont() const
{
 return d->mNamePreview->font();
}

QFont KChatDialog::textFont() const
{
 return d->mTextPreview->font();
}

QFont KChatDialog::systemNameFont() const
{
 return d->mSystemNamePreview->font();
}

QFont KChatDialog::systemTextFont() const
{
 return d->mSystemTextPreview->font();
}

void KChatDialog::plugChatWidget(KChatBase* widget, bool applyFonts)
{
 d->mChat = widget;
 if (applyFonts && d->mChat) {
	setNameFont(d->mChat->nameFont());
	setTextFont(d->mChat->messageFont());
	setSystemNameFont(d->mChat->systemNameFont());
	setSystemTextFont(d->mChat->systemMessageFont());
	setMaxMessages(d->mChat->maxItems());
 }
}

void KChatDialog::configureChatWidget(KChatBase* widget)
{
 if (!widget) {
	return;
 }
 widget->setNameFont(nameFont());
 widget->setMessageFont(textFont());

 widget->setSystemNameFont(systemNameFont());
 widget->setSystemMessageFont(systemTextFont());
 
 widget->setMaxItems(maxMessages());
 widget->saveConfig();
 qCDebug(KFOURINLINE_LOG) << "Saved configuration";
}

void KChatDialog::slotOk()
{
 slotApply();
 QDialog::accept();
}

void KChatDialog::slotApply()
{
 configureChatWidget(d->mChat);
}

void KChatDialog::setNameFont(const QFont &f)
{
 d->mNamePreview->setFont(f);
}

void KChatDialog::setTextFont(const QFont &f)
{
 d->mTextPreview->setFont(f);
}

void KChatDialog::setSystemNameFont(const QFont &f)
{
 d->mSystemNamePreview->setFont(f);
}

void KChatDialog::setSystemTextFont(const QFont &f)
{
 d->mSystemTextPreview->setFont(f);
}

void KChatDialog::setMaxMessages(int max)
{
 d->mMaxMessages->setText(QString::number(max));
}

int KChatDialog::maxMessages() const
{
 bool ok;
 int max = d->mMaxMessages->text().toInt(&ok);
 if (!ok) {
	return -1; // unlimited is default
 }
 return max;
}


