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
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for KDE 
#include <kmainwindow.h>
#include <kdialogbase.h>

#define KWIN4_VERSION "v1.00"

// forward declaration of the Kwin4 classes
class KGame;
class KPlayer;
class KGameChat;
class KChatDialog;
class Kwin4Doc;
class Kwin4View;
class Geom;
class Kwin4Player;
class QVButtonGroup;

extern int global_debug;

typedef enum  {Niemand=-1,Gelb=0,Rot=1,Tip=3,Rand=4,GelbWin=8,RotWin=9} FARBE;
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
  * The base class for Kwin4 application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. An instance of Kwin4View creates your center view, which is connected
  * to the window's Doc object.
  * Kwin4App reimplements the methods that KMainWindow provides for main window handling and supports
  * full session management as well as keyboard accelerator configuration by using KAccel.
  * @see KMainWindow
  * @see KApplication
  * @see KConfig
  * @see KAccel
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
  * @version KDevelop version 0.4 code generation
  */
class Kwin4App : public KMainWindow
{
  Q_OBJECT

  friend class Kwin4View;

  public:
    /** construtor of Kwin4App, calls all init functions to create the application.
     * @see initMenuBar initToolBar
     */
    Kwin4App();
    
    // Enabled/Disabled menu/toolbar items
    void changeAction(const char *,bool);
    void enableAction(const char *action) {changeAction(action, true); } ;
    void disableAction(const char *action) {changeAction(action, false); } ;
    
  /**
   * Ends the current game
   * called only by the KGame framework via GameStatus
   */
  void EndGame(TABLE mode);

  protected:
  /**
  * Flags whhich menus should be checked and set
  **/
  enum CheckFlags {All=0,CheckFileMenu=1,CheckEditMenu=2};
  /** Checks all menus..usually done on init programm */
  void checkMenus(int menu=0);
  /** initGUI creates the menubar and inserts the menupopups as well as creating the helpMenu.
    * @see KApplication#getHelpMenu
    */
  void initGUI();

  /** sets up the statusbar for the main window by initialzing a statuslabel.
    */
  void initStatusBar();
  /** initializes the document object of the main window that is connected to the view in initView().
    * @see initView();
    */
  void initDocument();
  /** creates the centerwidget of the KMainWindow instance and sets it as the view
    */
  void initView();
  /** saves the window properties for each open window during session end to the session config file, including saving the currently
    * opened file by a temporary filename provided by KApplication.
    * @see KMainWindow#saveProperties
    */
  virtual void saveProperties(KConfig *_cfg);
  /** reads the session config file and restores the application's state including the last opened files and documents by reading the
    * temporary files saved by saveProperties()
    * @see KMainWindow#readProperties
    */
  virtual void readProperties(KConfig *_cfg);

  public slots:
    /**
    * By the network dialog. Switch server/client
    **/
    void slotServerTypeChanged(int t);
    /**
    * The remove player changed
    **/
    void slotRemoteChanged(int who);
    /**
    * The game is ober or aborted
    **/
    void slotGameOver(int status, KPlayer * p, KGame * me);
    /**
    * A move is done. update status
    **/
    void slotMoveDone(int x, int y);
    /**
    * The network connection is lost
    **/
    void slotNetworkBroken(int id, int oldstatus ,KGame *game);
    /** Being noticed that a new game started */
    void slotNewGame();
    /** Set the names in the mover field */
    void slotStatusNames();

    void slotInitNetwork();
    void slotChat();
    void slotDebugKGame();

    /** clears the document in the actual view to reuse it as the new document */
    void newGame();
    void slotOpenFile();
    void slotSaveFile();
    /** asks for saving if the file is modified, then closes the actual file and window*/
    void endGame();
    /** show statistics */
    void showStatistics();

    void slotUndo();
    void slotRedo();

    void slotStatusMover(const QString &text);
    void slotStatusMsg(const QString &text);

  private:
    KGameChat *mChat;
    ChatDlg *mMyChatDlg;
    // Grafix
    QString mGrafix;

    /**
     * view is the main widget which represents your working area. The View
     * class should handle all events of the view widget.  It is kept empty so
     * you can create your view according to your application's needs by
     * changing the view class.
     */
    Kwin4View *view;
    /** 
     * doc represents your actual document and is created only once. It keeps
     * information such as filename and does the serialization of your files.
     */
    Kwin4Doc *doc;
    QVButtonGroup *mColorGroup;

protected slots: // Protected slots
  /**
   *  Writes ready into the statusbar
   */
  void slotClearStatusText();

  void slotKeyBindings();
  void showSettings();
};
 
#endif // KWIN4_H

