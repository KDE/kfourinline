/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2007 by Martin Heni
    email                : kde@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KWIN4_H
#define KWIN4_H

#include <QGraphicsScene>
 
#include <kmainwindow.h>
#include "kwin4global.h"
#include "thememanager.h"
#include <kdemacros.h>
class Kwin4Player;
class KWin4Doc;
class KWin4View;
class KButtonGroup;
class KPlayer;
class KGameChat;
class ChatDlg;
class KGame;

/**
 * The base class for Kwin4 application window.
 */
class KWin4App : public KMainWindow
{

Q_OBJECT

public:
  KWin4App(QWidget *parent=0);
  ~KWin4App();

protected:
  void EndGame(TABLE mode);

  // Flags which menus should be checked and set by below functions
  enum CheckFlags {All=0,CheckFileMenu=1,CheckEditMenu=2};
  
  void changeAction(const char *,bool);
  // Enabled/Disabled menu/toolbar items
  void enableAction(const char *action) {changeAction(action, true); } ;
  void disableAction(const char *action) {changeAction(action, false); } ;
  // Checks all menus..usually done on init program
  void checkMenus(CheckFlags menu=All);
  
  void initGUI();
  void initStatusBar();
  void initDocument();
  
  virtual void saveProperties(KConfigGroup &cfg);
  virtual void readProperties(const KConfigGroup &cfg);

public slots:
  void slotServerTypeChanged(int t);
  
  void slotRemoteChanged(int who);
  void slotGameOver(int status, KPlayer * p, KGame * me);
  void slotMoveDone(int x, int y);

  void slotNetworkBroken(int id, int oldstatus ,KGame *game);
  /** Being noticed that a new game started */
  void slotNewGame();
  void slotStatusNames();

  void slotInitNetwork();
  void slotChat();
  void slotDebugKGame();

  void newGame();
  void slotOpenGame();
  void slotSaveGame();
  void endGame();
  void showStatistics();
  void calcHint();
  void themeChanged(const QString& name);

  void slotUndo();
  void slotRedo();

  void slotStatusMover(const QString &text);
  void slotStatusMsg(const QString &text);

private:
  ThemeManager* mTheme;
  KWin4View *mView;
  KWin4Doc *mDoc;
  QGraphicsScene* mScene;
  QString mThemeDirName;

  KButtonGroup *mColorGroup;
  ChatDlg *mMyChatDlg;  

protected slots:
  void slotClearStatusText();

  void showSettings();
};
 
#endif // KWIN4_H

