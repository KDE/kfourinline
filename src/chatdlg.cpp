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
#include <QPushButton>
#include <QGroupBox>

// include files for KDE
#include <klocale.h>
#include <kdebug.h>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kchatdialog.h>
#include <libkdegamesprivate/kgame/kgamechat.h>
#include <libkdegamesprivate/kgame/dialogs/kgamedialog.h>

// application specific includes
#include "kwin4player.h"


// Constructor for the chat widget. This widget
// is derived from the libkdegames chat widget
ChatDlg::ChatDlg(KGame *game,QWidget *parent)
       : KDialog(parent),mChat(0), mChatDlg(0)
{
  setCaption(i18n("Chat Dlg"));
  setButtons(Ok);
  setDefaultButton(Ok);
  showButtonSeparator(true);
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
  
  setMainWidget(frame);
  
  adjustSize();
  
  mChatDlg                 = new KChatDialog(mChat,frame,true);
  connect(mButton,SIGNAL(clicked()),mChatDlg,SLOT(show()));
}

// Set the player in who does the chat. This should be the local player.
void ChatDlg::setPlayer(KWin4Player* p)
{
  if (!mChat)
  {
    kError() << "ChatDlg::setPlayer::Chat not defined can't set player";
    return ;
  }
  if (!p)
  {
    kError() << "ChatDlg::setPlayer::Player not defined can't set player";
    return ;
  }
  mChat->setFromPlayer(p);
}

#include "chatdlg.moc"
