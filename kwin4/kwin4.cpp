/***************************************************************************
                          kwin4  -  Boardgame for KDE
                             -------------------
    begin                : Sun Mar 26 12:50:12 CEST 2000
    copyright            : (C) |1995-2000 by Martin Heni
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


// include files for QT
#include <qstring.h>
#include <qlayout.h>
#include <qhgroupbox.h>
#include <qvbox.h>
#include <qradiobutton.h>


#include <stdio.h>
//#include <unistd.h>

// include files for KDE
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kkeydialog.h>

#include <kchatdialog.h>
#include <kgamechat.h>
#include <dialogs/kgamedialog.h>
#include <dialogs/kgamedialogconfig.h>
#include <dialogs/kgameconnectdialog.h>
#include <dialogs/kgameerrordialog.h>
#include <dialogs/kgamedebugdialog.h>



// application specific includes
#include "kwin4.h"
#include "kwin4view.h"
#include "kwin4doc.h"

#include "namedlg.h"
#include "statdlg.h"
#include "geom.h"
#include "msgdlg.h"
#include "kwin4aboutdlg.h"
#include <qpushbutton.h>
#include <kstatusbar.h>

#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG               1003
#define ID_STATUS_MOVER              1002
#define ID_STATUS_TIME               1001


/**
 * Constructor for the chat widget. This widget
 * is derived from the libkdegames chat widget
 **/
ChatDlg::ChatDlg(KGame *game,QWidget *parent)
    : KDialogBase(Plain,i18n("Chat Dlg"),Ok,Ok,parent,0,false,true)
{
 setMinimumSize(QSize(200,200));
 mChat=0;
 mChatDlg=0;

 #ifdef OLDWIDGET
 QVBoxLayout* mTopLayout=new QVBoxLayout(plainPage(), spacingHint());;
 QHBoxLayout* h = new QHBoxLayout(mTopLayout);
 QHGroupBox* b = new QHGroupBox(i18n("Chat"), plainPage());
 mChat = new KGameChat(game, 10000, b);
 h->addWidget(b, 1);
 h->addSpacing(10);
 #endif

 QGridLayout* mGridLayout=new QGridLayout(plainPage());

 QHBoxLayout* h = new QHBoxLayout(plainPage());
 QHGroupBox* b = new QHGroupBox(i18n("Chat"), plainPage());
 mChat = new KGameChat(game, 10000, b);
 h->addWidget(b, 1);
 h->addSpacing(10);
 mGridLayout->addLayout(h,0,0);

 QPushButton *mButton=new QPushButton(i18n("Configure..."),plainPage());
 mGridLayout->addWidget(mButton,1,1);

 adjustSize();

 mChatDlg=new KChatDialog(mChat,plainPage(),true);
 connect(mButton,SIGNAL(clicked()),mChatDlg,SLOT(show()));
}

/**
 * Set the player in who does the chat. This should be
 * the local player. 
 */
void ChatDlg::setPlayer(Kwin4Player *p)
{
  kdDebug() << "CHATDLG: !!!!!!!!!! player " << p->userId() << endl;
  if (!mChat)
  {
    kdError() << "ChatDlg::setPlayer::Chat not defined .. cannot set player" << endl;
    return ;
  }
  if (!p)
  {
    kdError() << "ChatDlg::setPlayer::Player not defined .. cannot set player" << endl;
    return ;
  }
  mChat->setFromPlayer(p);
}


/**
 * Construct the main application window 
 **/
Kwin4App::Kwin4App() : KMainWindow(0)
{
  mMyChatDlg=0;
  mChat=0;
  view=0;
  doc=0;
  config=kapp->config();
  mAppTitle=i18n("Four wins");

  kdDebug() <<" VERSION " << VERSION << endl;

   // localise data file
   QString file="kwin4/grafix/default/grafix.rc";
   mGrafix=kapp->dirs()->findResourceDir("data",file);
   if (mGrafix.isNull()) mGrafix="grafix/default/";
   else mGrafix+="kwin4/grafix/default/";
   if (global_debug>3) kdDebug() << "Localised grafix dir " << mGrafix << endl;

   // Allow overriding of the grafix directory
   // This is a cheap and dirty way for theming
   config->setGroup("Themes");
   mGrafix = config->readPathEntry("grafixdir", mGrafix);
   kdDebug() << "Localised mGrafix " << mGrafix << endl;


  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initGUI();
  initStatusBar();
  setHelpMenuEnabled(false);
  createGUI(QString::null, false);
  if (toolBar()) toolBar()->hide();

  initDocument();
  initView();
  initPlayers();
  readOptions();


  setMinimumSize(580,400);      // TODO
  setMaximumSize(800,600);
  resize( 640, 480 );

  // better be last in init
  checkMenus();
}

/**
 * Standard application destructor 
 **/
Kwin4App::~Kwin4App()
{
   delete statusTimer;
}

/**
 * This method is called from various places
 * and signals to check, uncheck and enable
 * or disable all menu items.
 * The menu parameter can limit this operation
 * to one or more of the main menues (File,View,...)
 **/
void Kwin4App::checkMenus(int menu)
{
  bool localgame=(!doc->isNetwork());
  
  if (!menu || (menu&CheckFileMenu))
  {
    // Show Hint ?
    if (!doc->IsRunning() && localgame)
    {
      disableAction("hint");
    }
    else
    {
      enableAction("hint");
    }

    // Show new game?
    if (doc->IsRunning() )
    {
      disableAction("new_game");
    }
    else
    {
      enableAction("new_game");
    }

    // Show message
    if ( doc->isNetwork())
    {
      enableAction("send_message");
    }
    else
    {
      disableAction("send_message");
    }

    // Show game end?
    if (doc->IsRunning())
    {
      enableAction("end_game");
    }
    else
    {
      disableAction("end_game");
    }
  }

  if (!menu || (menu&CheckEditMenu))
  {
    // Show undo?
    if (!doc->IsRunning() || !localgame)
    {
      disableAction("edit_undo");
    }
    else if (doc->QueryHistoryCnt()==0)
    {
      disableAction("edit_undo");
    }
    else if (doc->QueryCurrentMove()<1 )
    {
      disableAction("edit_undo");
    }
    else
    {
      enableAction("edit_undo");
    }

    // Show redo
    if (!doc->IsRunning() || !localgame)
    {
      disableAction("edit_redo");
    }
    else if (doc->QueryHistoryCnt()==doc->QueryMaxMove())
    {
      disableAction("edit_redo");
    }
    else
    {
      enableAction("edit_redo");
    }

  }

  if (!menu || (menu&CheckViewMenu))
  {
    if (statusBar()->isHidden()) ((KToggleAction*)ACTION("options_show_statusbar"))->setChecked(false);
    else ((KToggleAction*)ACTION("options_show_statusbar"))->setChecked(true);
  }

  if (!menu || (menu&CheckOptionsMenu))
  {
    if (doc->QueryPlayerColour(0)==Gelb)
    {
      ((KSelectAction *)ACTION("startplayer"))->setCurrentItem(0);
    }
    else
    {
      ((KSelectAction *)ACTION("startplayer"))->setCurrentItem(1);
    }
    if (doc->IsRunning())
    {
      disableAction("startplayer");
    }
    else
    {
      enableAction("startplayer");
    }


    if (doc->playedBy(Gelb)==KGameIO::MouseIO)
        ((KSelectAction *)ACTION("player1"))->setCurrentItem(0);
    else if (doc->playedBy(Gelb)==KGameIO::ProcessIO)
        ((KSelectAction *)ACTION("player1"))->setCurrentItem(1);
    else
        ((KSelectAction *)ACTION("player1"))->setCurrentItem(2);

    if (doc->IsRunning() && doc->QueryCurrentPlayer()==Gelb)
    {
      disableAction("player1");
    }
    else
    {
      enableAction("player1");
    }

    if (doc->playedBy(Rot)==KGameIO::MouseIO)
        ((KSelectAction *)ACTION("player2"))->setCurrentItem(0);
    else if (doc->playedBy(Rot)==KGameIO::ProcessIO)
        ((KSelectAction *)ACTION("player2"))->setCurrentItem(1);
    else
        ((KSelectAction *)ACTION("player2"))->setCurrentItem(2);

    if (doc->IsRunning() && doc->QueryCurrentPlayer()==Rot)
    {
      disableAction("player2");
    }
    else
    {
      enableAction("player2");
    }

    ((KSelectAction *)ACTION("choose_level"))->setCurrentItem(doc->QueryLevel()-1);

    /*
    // Network
    if (doc->QueryServer()) ((KToggleAction*)ACTION("network_server"))->setChecked(true);
    else ((KToggleAction*)ACTION("network_server"))->setChecked(false);

    if (doc->QueryBlink()) ((KToggleAction*)ACTION("animations"))->setChecked(true);
    else ((KToggleAction*)ACTION("animations"))->setChecked(false);
    */

  }

}

/** 
 * Function to create the actions for the menu. This
 * works together with the xml guirc file
 **/
void Kwin4App::initGUI()
{
  QStringList list;

  (void)KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection(), "new_game");
  ACTION("new_game")->setStatusText(i18n("Starting a new game..."));
  ACTION("new_game")->setWhatsThis(i18n("Starting a new game..."));

  (void)KStdAction::open(this, SLOT(slotOpenFile()), actionCollection(), "open");
  ACTION("open")->setStatusText(i18n("Open a saved game..."));
  ACTION("open")->setWhatsThis(i18n("Open a saved game..."));

  (void)KStdAction::saveAs(this, SLOT(slotSaveFile()), actionCollection(), "save");
  ACTION("save")->setStatusText(i18n("Save a game."));
  ACTION("save")->setWhatsThis(i18n("Save a game."));

  (void)new KAction(i18n("&Abort Game"),"stop", 0, this, SLOT(slotFileClose()),
                      actionCollection(), "end_game");
  ACTION("end_game")->setStatusText(i18n("Ending the current game..."));
  ACTION("end_game")->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));

  (void)new KAction(i18n("&Network Configuration..."),0, this, SLOT(slotInitNetwork()),
                       actionCollection(), "file_network");

  (void)new KAction(i18n("Chat Widget..."),0, this, SLOT(slotChat()),
                       actionCollection(), "file_chat");

  if (global_debug>0)                     
  {
    (void)new KAction(i18n("Debug KGame"), 0, this, SLOT(slotDebugKGame()),
                        actionCollection(), "file_debug");
  }

  (void)new KAction(i18n("&Show Statistics"),"flag", 0, this, SLOT(slotFileStatistics()),
                      actionCollection(), "statistics");
  ACTION("statistics")->setStatusText(i18n("Show all time statistics."));
  ACTION("statistics")->setWhatsThis(i18n("Shows the all time statistics which is kept in all sessions."));

  (void)new KAction(i18n("&Hint"),"help", CTRL+Key_H, this, SLOT(slotFileHint()),
                      actionCollection(), "hint");
  ACTION("hint")->setStatusText(i18n("Shows a hint on how to move."));
  ACTION("hint")->setWhatsThis(i18n("Shows a hint on how to move."));

  (void)KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection(), "game_exit");
  ACTION("game_exit")->setStatusText(i18n("Exiting..."));
  ACTION("game_exit")->setWhatsThis(i18n("Quits the program."));

  (void)KStdAction::undo(this, SLOT(slotEditUndo()), actionCollection(), "edit_undo");
  ACTION("edit_undo")->setStatusText(i18n("Undo last move."));
  ACTION("edit_undo")->setWhatsThis(i18n("Undo last move."));

  (void)KStdAction::redo(this, SLOT(slotEditRedo()), actionCollection(), "edit_redo");
  ACTION("edit_redo")->setStatusText(i18n("Redo last move."));
  ACTION("edit_redo")->setWhatsThis(i18n("Redo last move."));

  (void)KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection());

  (void)new KSelectAction(i18n("Starting Player"),0,this,SLOT(slotStartplayer()),
                      actionCollection(), "startplayer");
  ACTION("startplayer")->setStatusText(i18n("Changing starting player..."));
  ACTION("startplayer")->setWhatsThis(i18n("Chooses which player begins the next game."));
  list.clear();
  list.append(i18n("&Yellow"));
  list.append(i18n("&Red"));
  ((KSelectAction *)ACTION("startplayer"))->setItems(list);

  (void)new KSelectAction(i18n("Yellow Played By"),0,this,SLOT(slotPlayer1By()),
                      actionCollection(), "player1");
  ACTION("player1")->setStatusText(i18n("Change who plays the yellow player..."));
  ACTION("player1")->setWhatsThis(i18n("Change who plays the yellow player..."));
  list.clear();
  list.append(i18n("&Mouse"));
  list.append(i18n("&Computer"));
  list.append(i18n("&Keyboard"));
  ((KSelectAction *)ACTION("player1"))->setItems(list);
  (void)new KSelectAction(i18n("Red Played By"),0,this,SLOT(slotPlayer2By()),
                      actionCollection(), "player2");
  ACTION("player1")->setStatusText(i18n("Change who plays the red player..."));
  ACTION("player1")->setWhatsThis(i18n("Change who plays the red player..."));
  ((KSelectAction *)ACTION("player2"))->setItems(list);

  (void)new KSelectAction(i18n("&Level"),0,this,SLOT(slotLevel()),
                      actionCollection(), "choose_level");
  ACTION("choose_level")->setStatusText(i18n("Change level..."));
  ACTION("choose_level")->setWhatsThis(i18n("Change the strength of the computer player."));
  list.clear();
  list.append(i18n("&1"));
  list.append(i18n("&2"));
  list.append(i18n("&3"));
  list.append(i18n("&4"));
  list.append(i18n("&5"));
  list.append(i18n("&6"));
  list.append(i18n("&7"));
  list.append(i18n("&8"));
  list.append(i18n("&9"));
  ((KSelectAction *)ACTION("choose_level"))->setItems(list);


  (void)new KAction(i18n("Change &Names..."), 0, this, SLOT(slotOptionsNames()),
                      actionCollection(), "change_names");
  ACTION("change_names")->setStatusText(i18n("Configure player names..."));
  ACTION("change_names")->setWhatsThis(i18n("Configure player names..."));


  actionCollection()->setHighlightingEnabled(true);
  connect(actionCollection(), SIGNAL(actionStatusText(const QString &)), SLOT(slotStatusMsg(const QString &)));
  connect(actionCollection(), SIGNAL(clearStatusText()), SLOT(slotClearStatusMsg()));

  KStdAction::keyBindings(this, SLOT(slotKeyBindings()), actionCollection());

  KHelpMenu *helpMenu = new KHelpMenu(this, 0, true, actionCollection());
  connect( helpMenu, SIGNAL(showAboutApplication()), this, SLOT(slotHelpAbout()));

}


/**
 * Set the status message to Ready
 **/
void Kwin4App::slotClearStatusMsg()
{
  slotStatusMsg(i18n("Ready"));
}


/** 
 * Create the status bar with the message part, the
 * player part and the clock
 **/
void Kwin4App::initStatusBar()
{
 // statusBar()->setInsertOrder(KStatusBar::RightToLeft);
  statusBar()->insertItem(i18n("This leaves space for the mover"),ID_STATUS_MOVER,0,true);
  statusBar()->insertItem(i18n("23:45"),ID_STATUS_TIME,0,true);
  statusBar()->insertItem(i18n("Ready"), ID_STATUS_MSG);

  slotStatusTime();
  slotStatusMover(i18n("(c) Martin Heni   "));
  slotStatusMsg(i18n("Welcome to %1").arg(appTitle()));

  statusTimer=new QTimer(this);
  connect(statusTimer,SIGNAL(timeout()),this,SLOT(slotStatusTimer()));
  statusTimer->start(10000,FALSE);
}

/**
 * Set up the document, i.e. the KGame object
 * and connect all signals emitted by it
 **/
void Kwin4App::initDocument()
{
  doc = new Kwin4Doc(this);
  doc->newDocument(config,mGrafix);
   // Game Over signal
  connect(doc,SIGNAL(signalGameOver(int, KPlayer *,KGame *)),
         this,SLOT(slotGameOver(int, KPlayer *,KGame *)));
  connect(doc,SIGNAL(signalMoveDone(int, int)),
         this,SLOT(slotMoveDone(int, int)));
  connect(doc,SIGNAL(signalClientLeftGame(int, int,KGame *)),
         this,SLOT(slotNetworkBroken(int, int, KGame *)));
  connect(doc,SIGNAL(signalNextPlayer()),
         this,SLOT(slotStatusNames()));

  connect(doc,SIGNAL(signalGameRun()),
         this,SLOT(slotNewGame()));
}


/**
 * Set up the players
 **/
void Kwin4App::initPlayers()
{
  doc->initPlayers();
}

/**
 * Set up the view (QCanvas)
 **/
void Kwin4App::initView()
{
  view = new Kwin4View(mGrafix,this);
  doc->setView(view);
  setCentralWidget(view);
  setCaption(appTitle());
}


/** 
 * Abbreviation to enable an KAction
 **/
void Kwin4App::enableAction(const char *s)
{
  if (s)
  {
    KAction *act=actionCollection()->action(s);
    if (act) act->setEnabled(true);
  }
}

/** 
 * Abbreviation to disable an KAction
 **/
void Kwin4App::disableAction(const char *s)
{
  if (s)
  {
    KAction *act=actionCollection()->action(s);
    if (act) act->setEnabled(false);
  }
}

/**
 * Returns points to the KGame object
 **/
Kwin4Doc *Kwin4App::getDocument() const
{
  return doc;
}


/** 
 * Save the window, geometry and stuff
 **/
void Kwin4App::saveOptions()
{
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
#ifdef USE_TOOLBAR
  config->writeEntry("Show Toolbar", toolBar()->isVisible());
  config->writeEntry("ToolBarPos", (int) toolBar()->barPos());
#endif
  config->writeEntry("Show Statusbar",statusBar()->isVisible());
//  config->writePathEntry("Recent Files", recentFiles);

  doc->WriteConfig(config);

}

/**
 * Load the window
 **/
void Kwin4App::readOptions()
{
  config->setGroup("General Options");

  // bar status settings
#ifdef USE_TOOLBAR
  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
  viewMenu->setItemChecked(ID_VIEW_TOOLBAR, bViewToolbar);
  if(bViewToolbar)
     toolBar()->show();
  else
     toolBar()->hide();
#endif

#ifdef NOGUI
  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  viewMenu->setItemChecked(ID_VIEW_STATUSBAR, bViewStatusbar);
  if(bViewStatusbar)
    statusBar()->show();
  else
    statusBar()->hide();
#endif

  #ifdef USE_TOOLBAR
  // bar position settings
  KToolBar::BarPosition toolBarPos;
  toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
  toolBar()->setBarPos(toolBarPos);
#endif
  QSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }
  doc->ReadConfig(config);
}

/**
 * Forward the save to the document/Kgame to store
 * network and menu options
 **/
void Kwin4App::saveProperties(KConfig *_cfg)
{
  if(doc->getTitle()!=i18n("Untitled") )
  {
    // saving to tempfile not necessary
  }
  else
  {
    QString filename=doc->getAbsFilePath();
    _cfg->writePathEntry("filename", filename);

    QString tempname = kapp->tempSaveName(filename);
    doc->saveDocument(tempname);
  }
}

/**
 * Load game and menu options back
 **/
void Kwin4App::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readPathEntry("filename");
  bool modified = _cfg->readBoolEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);

    if(canRecover)
    {
      doc->openDocument(tempname);
      QFileInfo info(filename);
      doc->setAbsFilePath(info.absFilePath());
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(filename);
    }
  }

  QString caption=kapp->caption();
  setCaption(appTitle());
}

/** 
 * Always allow to close the game
 **/
bool Kwin4App::queryClose()
{
  return true;
}

/**
 * Alwas allow to exit
 **/
bool Kwin4App::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

/**
 * Load a game
 **/
void Kwin4App::slotOpenFile()
{
  QString dir,filter,file;

  dir=QString(":<kwin4>");
  filter=QString("*");
  if (global_debug>10) file="/tmp/kwin.save";
  else file=KFileDialog::getOpenFileName(dir,filter,this);
  doc->load(file,true);
  checkMenus();
}

/**
 * Save game
 **/
void Kwin4App::slotSaveFile()
{
  QString dir,filter,file;
  dir=QString(":<kwin4>");
  filter=QString("*");
  if (global_debug>10) file="/tmp/kwin.save";
  else file=KFileDialog::getSaveFileName(dir,filter,this);
  kdDebug() << "Saving to " << file << endl;
  doc->save(file);
}

/**
 * Start a new game
 **/
void Kwin4App::slotFileNew()
{
  // End the intro if it is running
  doc->setGameStatus(Kwin4Doc::End);
  // Init the board and Clear the old game out
  doc->setGameStatus(Kwin4Doc::Init);
  // Run it
  doc->setGameStatus(Kwin4Doc::Run);
}

/**
 * Slot: Noticed that a new game started...update menues
 */
void Kwin4App::slotNewGame()
{
  slotStatusNames();
  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);
}

/**
 * Abort a running game
 **/
void Kwin4App::slotFileClose()
{
  doc->setGameStatus(Kwin4Doc::Abort);
}

/**
 * Exit the program
 **/
void Kwin4App::slotFileQuit()
{
  saveOptions();
  view->close();
  close();
}

/** 
 * Show statistics dialog
 */
void Kwin4App::slotFileStatistics()
{
  int res;

  StatDlg *dlg=new StatDlg(this,"Game statistics");
  dlg->SetNames(doc->QueryName(Gelb),doc->QueryName(Rot));
  dlg->SetStat1(doc->QueryStat(Gelb,TWin),
                doc->QueryStat(Gelb,TRemis),
                doc->QueryStat(Gelb,TLost),
                doc->QueryStat(Gelb,TBrk),
                doc->QueryStat(Gelb,TSum) );
  dlg->SetStat2(doc->QueryStat(Rot,TWin),
                doc->QueryStat(Rot,TRemis),
                doc->QueryStat(Rot,TLost),
                doc->QueryStat(Rot,TBrk),
                doc->QueryStat(Rot,TSum) );

  QPixmap *pixmap=view->loadPixmap("win4about.png");
  if (pixmap) dlg->SetPixmap(*pixmap);

  res=dlg->exec();
  if (res==-100)
  {
    doc->ResetStat();
  }
}

/**
 * Undo menu call
 **/
void Kwin4App::slotEditUndo()
{

  doc->UndoMove();
  // Undo twice if computer is moving then
  if (doc->playedBy(doc->QueryCurrentPlayer())==KGameIO::ProcessIO) doc->UndoMove();

  // Prepare menus
  slotStatusNames();
  checkMenus(CheckEditMenu|CheckOptionsMenu);

}

/**
 * Redo menu call
 **/
void Kwin4App::slotEditRedo()
{
  doc->RedoMove();
  if (doc->playedBy(doc->QueryCurrentPlayer())==KGameIO::ProcessIO) doc->RedoMove();
  slotStatusNames();
  checkMenus(CheckEditMenu|CheckOptionsMenu);

}

/**
 * Show or hide statusbar
 **/
void Kwin4App::slotViewStatusBar()
{
  if (statusBar()->isVisible())
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }
}


/**
 * Set the given text into the statusbar
 **/
void Kwin4App::slotStatusMsg(const QString &text)
{
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

/**
 * Set the string in the statusbar window for
 * the player currently moving
 **/
void Kwin4App::slotStatusMover(const QString &text)
{
  // change status mover permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MOVER);
}

/**
 * Set the clock in the statusbar
 **/
void Kwin4App::slotStatusTime()
{
  // change status time permanently
  QString s;
  QTime now;

  now=QTime::currentTime();
  s=now.toString();
  s.truncate(5);

  statusBar()->clear();
  statusBar()->changeItem(s, ID_STATUS_TIME);
}

/**
 * Display a help text in the statusbar for 2 seconds
 **/
void Kwin4App::slotStatusHelpMsg(const QString &text)
{
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 2000);
}

/**
 * The startplayer is changed in the menu
 **/
void Kwin4App::slotStartplayer()
{
  int i=((KSelectAction *)ACTION("startplayer"))->currentItem();
  if (i==0) slotStartcolourYellow();
  else slotStartcolourRed();
}

/** 
 * Change startcolour via menus
 */
void Kwin4App::slotStartcolourRed()
{
  if (doc->QueryPlayerColour(0)==Gelb)
  {
    doc->SwitchStartPlayer();
    checkMenus(CheckOptionsMenu);
  }
}

/** 
  * change startcolour
  */
void Kwin4App::slotStartcolourYellow()
{

  if (doc->QueryPlayerColour(0)==Rot)
  {
    doc->SwitchStartPlayer();
    checkMenus(CheckOptionsMenu);
  }
}

/**
 * Menu call that the player 1 should be played by
 * a different IO device
 **/
void Kwin4App::slotPlayer1By()
{
  switch(((KSelectAction *)ACTION("player1"))->currentItem())
  {
    case 0:
        slotYellowPlayer();
    break;
    case 1:
        slotYellowComputer();
    break;
    case 2:
        slotYellowKeyboard();
    break;
  }
}

/**
 * Menu call that the player 2 should be played by
 * a different IO device
 **/
void Kwin4App::slotPlayer2By()
{
  switch(((KSelectAction *)ACTION("player2"))->currentItem())
  {
    case 0:
        slotRedPlayer();
    break;
    case 1:
        slotRedComputer();
    break;
    case 2:
        slotRedKeyboard();
    break;
  }
}

/**
 * Change yellow to mouse
 */
void Kwin4App::slotYellowPlayer()
{
    doc->setPlayedBy(Gelb,KGameIO::MouseIO);
    checkMenus(CheckOptionsMenu);

}

/**
 * Change yellow to computer
 */
void Kwin4App::slotYellowComputer()
{
    doc->setPlayedBy(Gelb,KGameIO::ProcessIO);
    checkMenus(CheckOptionsMenu);
}

/**
 * Change yellow to keyboard
 */
void Kwin4App::slotYellowKeyboard()
{
  doc->setPlayedBy(Gelb,KGameIO::KeyIO);
  checkMenus(CheckOptionsMenu);

}

/**
 * Menu call to change the IO of the red player
 */
void Kwin4App::slotRedPlayer()
{
    doc->setPlayedBy(Rot,KGameIO::MouseIO);
    checkMenus(CheckOptionsMenu);
}

/**
 * Change red to computer
 */
void Kwin4App::slotRedComputer()
{
  doc->setPlayedBy(Rot,KGameIO::ProcessIO);
  checkMenus(CheckOptionsMenu);
}

/**
 * Change red to keyboard
 */
void Kwin4App::slotRedKeyboard()
{
  doc->setPlayedBy(Rot,KGameIO::KeyIO);
  checkMenus(CheckOptionsMenu);

}

/**
 * Change the level of the computer player
 */
void Kwin4App::slotLevel()
{
  int i=((KSelectAction *)ACTION("choose_level"))->currentItem();
  i++; // we start at 1

  doc->SetLevel(i);
  checkMenus(CheckOptionsMenu);

}

/**
 * Show the dialog to change the names of the player
 */
void Kwin4App::slotOptionsNames()
{
  NameDlg *dlg=new NameDlg(this,"Enter your name...");
  dlg->SetNames(doc->QueryName(Gelb),doc->QueryName(Rot));
  if (dlg->exec()==QDialog::Accepted)
  {
    QString n1,n2;
    dlg->GetNames(n1,n2);
    doc->SetName(Gelb,n1);
    doc->SetName(Rot,n2);
    slotStatusNames();
  }


}



/**
 * Ends the current game 
 * Called by the gameover signal
 */
void Kwin4App::EndGame(TABLE mode)
{
  doc->EndGame(mode);
  doc->SwitchStartPlayer();
  slotStatusNames();
  checkMenus();
}


/** Triggers the status timer */
void Kwin4App::slotStatusTimer(void){
  slotStatusTime();
}
/** Set the names in the mover field */
void Kwin4App::slotStatusNames(){

  QString msg;
  if (!doc->IsRunning())
  {
    msg=i18n("No game  ");
  }
  else if (doc->QueryCurrentPlayer()==Gelb)
  {
    msg=QString(" ")+doc->QueryName(Gelb)+ i18n(" - Yellow ");
  }
  else if (doc->QueryCurrentPlayer())
  {
    msg=QString(" ")+doc->QueryName(Rot)+ i18n(" - Red ");
  }
  else
  {
    msg=i18n("Nobody  ");
  }
  slotStatusMover(msg);

}

/**
 * Shows the about dialog
 */
void Kwin4App::slotHelpAbout()
{
  aboutDlg *dlg=new aboutDlg(this);
  QPixmap *pixmap=view->loadPixmap("win4about.png");
  if (pixmap) dlg->SetPixmap(*pixmap);
  dlg->exec();
}

void Kwin4App::SetGrafix(QString s)
{
  mGrafix=s;
}
void Kwin4App::slotNetworkBroken(int id, int oldstatus ,KGame * /*game */)
{
  kdDebug() <<  "Kwin4App::slotNetworkBroken" << endl;
  if (doc->playedBy(Gelb)==0)
  {
    slotYellowPlayer();
  }
  if (doc->playedBy(Rot)==0)
  {
    slotRedPlayer();
  }

  kdDebug() << "CurrrentPlayer=" << doc->QueryCurrentPlayer() << endl;
  kdDebug() << "   " <<  doc->getPlayer(doc->QueryCurrentPlayer()) << endl;
  doc->getPlayer(doc->QueryCurrentPlayer())->setTurn(true,true);


  KMessageBox::information(this,i18n("The network game ended!\n"));
  doc->setGameStatus(oldstatus);
}


void Kwin4App::slotMoveDone(int /* x */ ,int /* y */ )
{
  checkMenus(CheckEditMenu|CheckOptionsMenu);
  slotStatusNames();
  slotStatusMsg(i18n("Game running..."));
}

void Kwin4App::slotGameOver(int status, KPlayer * p, KGame * /*me*/)
{
  QString msg;
  kdDebug() << "Kwin4App::GAME OVER  STATUS="<< status << endl;
  if (status==-1) // remis
  {
    EndGame(TRemis);
    checkMenus(CheckEditMenu);
    slotStatusMsg(i18n("The game is drawn. Please restart next round."));
    //KMessageBox::information(this,i18n("The game ended inadvertently!\n"),appTitle());
  }
  else if (status==1)
  {
    kdDebug() <<"in kwin4 userid after win="<<p->userId()<<endl;
    if (p->userId()==Gelb)
    {
      EndGame(TWin);
      checkMenus(CheckEditMenu);
      msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(Gelb));
      slotStatusMsg(msg);

      //msg=i18n("%1 (Yellow) has won the game!").arg(doc->QueryName(Gelb));
      //KMessageBox::information(this, msg, appTitle());
    }
    else
    {
      EndGame(TLost);
      checkMenus(CheckEditMenu);
      msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(Rot));
      slotStatusMsg(msg);

      //msg=i18n("%1 (Red) has won the game!").arg(doc->QueryName(Rot));
      //KMessageBox::information(this,msg, appTitle());
    }
  }
  else if (status==2) // Abort
  {
    EndGame(TBrk);
    QString  m=i18n(" Game aborted. Please restart next round.");
    slotStatusMsg(m);
  }
  else
  {
    kdError() << "Gameover with status " << status << ". This is unexpected and a serious problem" << endl;
  }
}


// File Menu slot
void Kwin4App::slotDisconnect()
{
  kdDebug() << "Kwin4App::slotDisconnect" << endl;
  doc->disconnect();
}
void Kwin4App::slotInitNetwork()
{
  kdDebug() << "Kwin4App::slotInitNetwork" << endl;
  if (doc->gameStatus()==Kwin4Doc::Intro) doc->setGameStatus(Kwin4Doc::Pause);

  QString host = "localhost";
  int port=4242;


 // just for testing - should be non-modal
  KGameDialog dlg(doc, 0, i18n("Configuration"), this,
      KGameDialog::NetworkConfig, 20000, true);
  dlg.networkConfig()->setDefaultNetworkInfo(host, port);

  QVBox *box=dlg.configPage(KGameDialog::NetworkConfig);
  QVBoxLayout *l=(QVBoxLayout *)(box->layout());

  mColorGroup=new QVButtonGroup(box);
  connect(mColorGroup, SIGNAL(clicked(int)), this, SLOT(slotRemoteChanged(int)));
  connect(dlg.networkConfig(), SIGNAL(signalServerTypeChanged(int)), this, SLOT(slotServerTypeChanged(int)));

  (void)new QRadioButton(i18n("Yellow should be played by remote"), mColorGroup);
  (void)new QRadioButton(i18n("Red should be played by remote"), mColorGroup);
  l->addWidget(mColorGroup);
  mColorGroup->setButton(0);
  slotRemoteChanged(0);


  dlg.exec();// note: we don't have to check for the result - maybe a bug

/*
  // start KGame configuration
  mDialog = new KGameDialog(mGame, i18n("Configure KGame"), this);
  connect(mDialog, SIGNAL(finished()), this, SLOT(slotDialogFinished()));
  mDialog->show();*/
}

void Kwin4App::slotServerTypeChanged(int t)
{
  kdDebug() << "slotServerTypeChanged="<<t<<endl;
  if (t==0)
  {
    mColorGroup->setEnabled(true);
  }
  else
  {
    mColorGroup->setEnabled(false);
  }
}
void Kwin4App::slotRemoteChanged(int button)
{
  if (button==0)
  {
    doc->getPlayer(Gelb)->setNetworkPriority(0);
    doc->getPlayer(Rot)->setNetworkPriority(10);
  }
  else
  {
    doc->getPlayer(Gelb)->setNetworkPriority(10);
    doc->getPlayer(Rot)->setNetworkPriority(0);
  }
}

void Kwin4App::slotChat()
{
  kdDebug() << "Kwin4App::Chat" << endl;

  if (!mMyChatDlg)
  {
    mMyChatDlg=new ChatDlg(doc,this);
    Kwin4Player *p=doc->getPlayer(Gelb);
    if (!p->isVirtual())
    {
      mMyChatDlg->setPlayer(doc->getPlayer(Gelb));
    }
    else
    {
      mMyChatDlg->setPlayer(doc->getPlayer(Rot));
    }
    connect(doc,SIGNAL(signalChatChanged(Kwin4Player *)),
            mMyChatDlg,SLOT(setPlayer(Kwin4Player *)));
  }
  if (mMyChatDlg->isHidden()) mMyChatDlg->show();
  else mMyChatDlg->hide();

}

void Kwin4App::slotFileHint()
{
  doc->calcHint();
}
void Kwin4App::slotDebugKGame()
{
 if(!doc) return;
 KGameDebugDialog* d = new KGameDebugDialog(doc, this);
 d->show();
}

void Kwin4App::slotKeyBindings()
{
 KKeyDialog::configure(actionCollection(), this);
}


#include "kwin4.moc"
