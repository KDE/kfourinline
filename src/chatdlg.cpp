/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Header includes
#include "chatdlg.h"

// include files for QT
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

// include files for KDE
#include <KConfigGroup>
#include <KLocalizedString>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamechat.h>

// application specific includes
#include "kfourinline_debug.h"
#include "kchatdialog.h"
#include "kwin4player.h"


// Constructor for the chat widget. This widget
// is derived from the libkdegames chat widget
ChatDlg::ChatDlg(KGame *game,QWidget *parent)
       : QDialog(parent),mChat(0), mChatDlg(0)
{
  setWindowTitle(i18n("Chat Dlg"));
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setDefault(true);
  okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  okButton->setDefault(true);
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
  mGridLayout->addWidget(mButton,1,1);
  
  mainLayout->addWidget(frame);
  mainLayout->addWidget(buttonBox); 
  adjustSize();
  
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


