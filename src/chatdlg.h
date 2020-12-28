/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHATDLG_H
#define CHATDLG_H

// Qt
#include <QDialog>

class KGame;
class KGameChat;
class KChatDialog;
class KWin4Player;

/**
 * Subclass of the chat dialog provided by the KGame lib.
 * It supports a user defined chat and the setting of the
 * owner player
 **/
class ChatDlg : public QDialog
{
  Q_OBJECT

  public:
    explicit ChatDlg(KGame *game,QWidget* parent=nullptr);

  public Q_SLOTS:  
    void setPlayer(KWin4Player *p);
  
  private:  
    KGameChat*   mChat;
    KChatDialog* mChatDlg;
};

#endif // CHATDLG_H

