/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2001 by Martin Heni
    email                : martin@heni-online.de
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
 
#include <kmainwindow.h>
#include <kdialogbase.h>

#define KWIN4_VERSION "v1.10"

// forward declaration of the Kwin4 classes
class KGame;
class KPlayer;
class KGameChat;
class KChatDialog;
class Kwin4Doc;
class Kwin4View;
class Kwin4Player;
class QVButtonGroup;

extern int global_debug;

// The user or color?
typedef enum e_Farbe {Niemand=-1,Gelb=0,Rot=1,Tip=3,Rand=4,GelbWin=8,RotWin=9} FARBE;
// The type of player
typedef enum  {Men=0,Computer=1,Remote=2} PLAYER;
typedef enum  {TSum,TWin,TRemis,TLost,TBrk} TABLE;
typedef enum  {GIllMove=-2,GNotAllowed=-1,GNormal=0,GYellowWin=1,GRedWin=2,GRemis=3,GTip=4} MOVESTATUS;

/**
 * Subclass of the chat dialog provided by the KGame lib.
 * It supports a user defined chat and the setting of the
 * owner player
 **/
class ChatDlg : public KDialogBase
{
  Q_OBJECT
public:
  ChatDlg(KGame *game,QWidget* parent=0);

public slots:  
  void setPlayer(Kwin4Player *p);
  
private:  
  KGameChat *mChat;
  KChatDialog *mChatDlg;
};

/**
 * The base class for Kwin4 application window.
 */
class Kwin4App : public KMainWindow
{

Q_OBJECT

public:
  Kwin4App(QWidget *parent=0, const char *name=0);

protected:
  void EndGame(TABLE mode);

  // Flags which menus should be checked and set by below functions
  enum CheckFlags {All=0,CheckFileMenu=1,CheckEditMenu=2};
  
  void changeAction(const char *,bool);
  // Enabled/Disabled menu/toolbar items
  void enableAction(const char *action) {changeAction(action, true); } ;
  void disableAction(const char *action) {changeAction(action, false); } ;
  // Checks all menus..usually done on init programm
  void checkMenus(CheckFlags menu=All);
  
  void initGUI();
  void initStatusBar();
  void initDocument();
  
  virtual void saveProperties(KConfig *cfg);
  virtual void readProperties(KConfig *cfg);

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

  void slotUndo();
  void slotRedo();

  void slotStatusMover(const QString &text);
  void slotStatusMsg(const QString &text);

private:
  Kwin4View *view;
  Kwin4Doc *doc;

  QVButtonGroup *mColorGroup;
  KGameChat *mChat;
  ChatDlg *mMyChatDlg;  

protected slots:
  void slotClearStatusText();

  void slotKeyBindings();
  void showSettings();
};
 
#endif // KWIN4_H

