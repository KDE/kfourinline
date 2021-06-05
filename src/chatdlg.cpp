/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "chatdlg.h"

// own
#include "kfourinline_debug.h"
#include "kchatdialog.h"
#include "kwin4player.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamechat.h>
// KF
#include <KLocalizedString>
// Qt
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>


// Constructor for the chat widget. This widget
// is derived from the libkdegames chat widget
ChatDlg::ChatDlg(KGame *game,QWidget *parent)
       : QDialog(parent),mChat(), mChatDlg()
{
  setWindowTitle(i18nc("@title:window", "Chat Dlg"));
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  okButton->setAutoDefault(false);
  setModal(false);
  setMinimumSize(QSize(200,200));
  
  QFrame* frame            = new QFrame(this);

  QGridLayout* mGridLayout = new QGridLayout(frame);
  QGroupBox* b             = new QGroupBox(i18n("Chat"), frame);
  QVBoxLayout* gboxLay     = new QVBoxLayout(b);
  mChat                    = new KGameChat(game, 10000, b);
  gboxLay->addWidget(mChat);
  mGridLayout->addWidget(b,0,0);
  
  QPushButton *mButton     = new QPushButton(i18n("Configure..."),frame);
  mButton->setAutoDefault(false);
  mGridLayout->addWidget(mButton,1,1);
  
  mainLayout->addWidget(frame);
  mainLayout->addWidget(buttonBox); 
  adjustSize();
  mChat->setFocus();
  
  mChatDlg                 = new KChatDialog(mChat,frame,true);
  connect(mButton, &QPushButton::clicked, mChatDlg, &KChatDialog::show);
}

// Set the player in who does the chat. This should be the local player.
void ChatDlg::setPlayer(KWin4Player* p)
{
  if (!mChat)
  {
    qCCritical(KFOURINLINE_LOG) << "ChatDlg::setPlayer::Chat not defined can't set player";
    return ;
  }
  if (!p)
  {
    qCCritical(KFOURINLINE_LOG) << "ChatDlg::setPlayer::Player not defined can't set player";
    return ;
  }
  mChat->setFromPlayer(p);
}


