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
#include <qvbuttongroup.h>
#include <qlcdnumber.h>

// include files for KDE
#include <kapplication.h>
#include <kstdgameaction.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kaction.h>
#include <qpushbutton.h>
#include <kstatusbar.h>
#include <kconfigdialog.h>

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
#include "prefs.h"
#include "settings.h"
#include "statistics.h"

#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG                1003
#define ID_STATUS_MOVER              1002

/**
 * Constructor for the chat widget. This widget
 * is derived from the libkdegames chat widget
 */
ChatDlg::ChatDlg(KGame *game,QWidget *parent)
    : KDialogBase(Plain,i18n("Chat Dlg"),Ok,Ok,parent,0,false,true),mChat(0), mChatDlg(0)
{
 setMinimumSize(QSize(200,200));

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
  if (!mChat)
  {
    kdError() << "ChatDlg::setPlayer::Chat not defined can't set player" << endl;
    return ;
  }
  if (!p)
  {
    kdError() << "ChatDlg::setPlayer::Player not defined can't set player" << endl;
    return ;
  }
  mChat->setFromPlayer(p);
}

/**
 * Construct the main application window 
 */
Kwin4App::Kwin4App(QWidget *parent, const char *name) : KMainWindow(parent,name), view(0), doc(0), mChat(0), mMyChatDlg(0)
{
  initGUI();
  initStatusBar();
  initDocument();

  view = new Kwin4View(doc,this);
  doc->setView(view);
  setCentralWidget(view);
  doc->initPlayers();

  setMinimumSize(640,400);      // TODO
  setMaximumSize(800,600);
  
  setupGUI();

  doc->ReadConfig(kapp->config());

  checkMenus();
}

/**
 * This method is called from various places
 * and signals to check, uncheck and enable
 * or disable all menu items.
 * The menu parameter can limit this operation
 * to one or more of the main menues (File,View,...)
 */
void Kwin4App::checkMenus(CheckFlags menu)
{
  bool localgame=(!doc->isNetwork());
  bool isRunning = (doc->gameStatus()==KGame::Run);
  if (!menu || (menu&CheckFileMenu))
  {
    changeAction("hint", !(!isRunning && localgame));
    changeAction("new_game", !isRunning);
    changeAction("save", isRunning);
    changeAction("end_game", isRunning);
  }

  if (!menu || (menu&CheckEditMenu))
  {
    if (!isRunning || !localgame)
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
    if (!isRunning || !localgame)
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
}

/** 
 * Function to create the actions for the menu. This
 * works together with the xml guirc file
 */
void Kwin4App::initGUI()
{
  KStdGameAction::gameNew(this, SLOT(newGame()), actionCollection(), "new_game");
  ACTION("new_game")->setStatusText(i18n("Start a new game"));

  KStdGameAction::load(this, SLOT(slotOpenGame()), actionCollection(), "open");
  ACTION("open")->setStatusText(i18n("Open a saved game..."));

  KStdGameAction::save(this, SLOT(slotSaveGame()), actionCollection(), "save");
  ACTION("save")->setStatusText(i18n("Save a game..."));

  KStdGameAction::end(this, SLOT(endGame()), actionCollection(), "end_game");
  ACTION("end_game")->setStatusText(i18n("Ending the current game..."));
  ACTION("end_game")->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));

  new KAction(i18n("&Network Configuration..."),0, this, SLOT(slotInitNetwork()),
                       actionCollection(), "network_conf");

  new KAction(i18n("Network Chat..."),0, this, SLOT(slotChat()),
                       actionCollection(), "network_chat");

  if (global_debug>0)                     
    new KAction(i18n("Debug KGame"), 0, this, SLOT(slotDebugKGame()),
                        actionCollection(), "file_debug");

  new KAction(i18n("&Show Statistics"),"flag", 0, this,
           SLOT(showStatistics()), actionCollection(), "statistics");
  ACTION("statistics")->setStatusText(i18n("Show statistics."));

  KStdGameAction::hint(doc, SLOT(calcHint()), actionCollection(), "hint");
  ACTION("hint")->setStatusText(i18n("Shows a hint on how to move."));

  KStdGameAction::quit(this, SLOT(close()), actionCollection(), "game_exit");
  ACTION("game_exit")->setStatusText(i18n("Quits the program."));

  KStdGameAction::undo(this, SLOT(slotUndo()), actionCollection(), "edit_undo");
  ACTION("edit_undo")->setStatusText(i18n("Undo last move."));

  KStdGameAction::redo(this, SLOT(slotRedo()), actionCollection(), "edit_redo");
  ACTION("edit_redo")->setStatusText(i18n("Redo last move."));
  
  actionCollection()->setHighlightingEnabled(true);
  connect(actionCollection(), SIGNAL(actionStatusText(const QString &)), SLOT(slotStatusMsg(const QString &)));
  connect(actionCollection(), SIGNAL(clearStatusText()), SLOT(slotClearStatusText()));

  KStdAction::preferences(this, SLOT(showSettings()), actionCollection());
}

/**
 * Set the status message to Ready
 */
void Kwin4App::slotClearStatusText()
{
  slotStatusMsg(i18n("Ready"));
}

/** 
 * Create the status bar with the message part, the
 * player part
 */
void Kwin4App::initStatusBar()
{
  statusBar()->insertItem(i18n("This leaves space for the mover"),ID_STATUS_MOVER,0,true);
  statusBar()->insertItem(i18n("Ready"), ID_STATUS_MSG);

  slotStatusMover(i18n("(c) Martin Heni   "));
  slotStatusMsg(i18n("Welcome to KWin4"));
}

/**
 * Set up the document, i.e. the KGame object
 * and connect all signals emitted by it
 */
void Kwin4App::initDocument()
{
  doc = new Kwin4Doc(this);
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

void Kwin4App::changeAction(const char *action, bool enable){
  if (!action)
    return;

  KAction *act=actionCollection()->action(action);
  if (act)
    act->setEnabled(enable);
}

/**
 * Store the current game
 */
void Kwin4App::saveProperties(KConfig *cfg)
{
  QString tempfile = kapp->tempSaveName(QDir::currentDirPath()+"kwin4");
  cfg->writePathEntry("filename", tempfile );
  doc->save(tempfile);
}

/**
 * Load game back
 */
void Kwin4App::readProperties(KConfig* cfg)
{
  QString filename = cfg->readPathEntry("filename");
  if(!filename.isEmpty())
    doc->load(filename);
}

/**
 * Load a game
 */
void Kwin4App::slotOpenGame()
{
  QString dir(":<kwin4>");
  QString filter("*");
  QString file("/tmp/kwin.save");
  if (global_debug < 10)
    file=KFileDialog::getOpenFileName(dir,filter,this);
  doc->load(file,true);
  checkMenus();
}

/**
 * Save game
 */
void Kwin4App::slotSaveGame()
{
  QString dir(":<kwin4>");
  QString filter("*");
  QString file("/tmp/kwin.save");
  if (global_debug < 10)
    file=KFileDialog::getSaveFileName(dir,filter,this);
  doc->save(file);
}

/**
 * Start a new game
 */
void Kwin4App::newGame()
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
  //checkMenus(CheckFileMenu|CheckEditMenu);
  checkMenus(All);
}

/**
 * Abort a running game
 */
void Kwin4App::endGame()
{
  doc->setGameStatus(Kwin4Doc::Abort);
}

/** 
 * Show statistics dialog
 */
void Kwin4App::showStatistics()
{
  Statistics *dlg=new Statistics(this,"Game statistics");
  
  dlg->p1_name->setText(doc->QueryName(Gelb));
  dlg->p1_won->display(doc->QueryStat(Gelb, TWin));
  dlg->p1_drawn->display(doc->QueryStat(Gelb, TRemis));
  dlg->p1_lost->display(doc->QueryStat(Gelb, TLost));
  dlg->p1_aborted->display(doc->QueryStat(Gelb, TBrk));
  dlg->p1_sum->display(doc->QueryStat(Gelb, TSum));
  
  dlg->p2_name->setText(doc->QueryName(Rot));
  dlg->p2_won->display(doc->QueryStat(Rot, TWin));
  dlg->p2_drawn->display(doc->QueryStat(Rot, TRemis));
  dlg->p2_lost->display(doc->QueryStat(Rot, TLost));
  dlg->p2_aborted->display(doc->QueryStat(Rot, TBrk));
  dlg->p2_sum->display(doc->QueryStat(Rot, TSum));
  
  if(dlg->exec() == QDialog::Rejected)
    doc->ResetStat();
}

/**
 * Undo menu call
 */
void Kwin4App::slotUndo()
{
  doc->UndoMove();
  // Undo twice if computer is moving then
  if (doc->playedBy(doc->QueryCurrentPlayer())==KGameIO::ProcessIO)
    doc->UndoMove();

  // Prepare menus
  slotStatusNames();
  checkMenus(CheckEditMenu);
}

/**
 * Redo menu call
 */
void Kwin4App::slotRedo()
{
  doc->RedoMove();
  if (doc->playedBy(doc->QueryCurrentPlayer())==KGameIO::ProcessIO)
    doc->RedoMove();
  slotStatusNames();
  checkMenus(CheckEditMenu);
}

/**
 * Set the given text into the statusbar
 * change status message permanently
 */
void Kwin4App::slotStatusMsg(const QString &text)
{
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

/**
 * Set the string in the statusbar window for
 * the player currently moving
 * change status mover permanently
 */
void Kwin4App::slotStatusMover(const QString &text)
{
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MOVER);
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

/**
 * Set the names in the mover field
 */
void Kwin4App::slotStatusNames(){
  QString msg;
  if (!(doc->gameStatus()==KGame::Run))
    msg=i18n("No game  ");
  else if (doc->QueryCurrentPlayer()==Gelb)
    msg=QString(" ")+doc->QueryName(Gelb)+ i18n(" - Yellow ");
  else if (doc->QueryCurrentPlayer())
    msg=QString(" ")+doc->QueryName(Rot)+ i18n(" - Red ");
  else
    msg=i18n("Nobody  ");
  slotStatusMover(msg);
}

/**
 * The network connection is lost
 */
void Kwin4App::slotNetworkBroken(int /*id*/, int oldstatus ,KGame * /*game */)
{
  kdDebug(12010) <<  "Kwin4App::slotNetworkBroken" << endl;
  if (doc->playedBy(Gelb)==0)
    doc->setPlayedBy(Gelb,KGameIO::MouseIO);
  if (doc->playedBy(Rot)==0)
    doc->setPlayedBy(Rot,KGameIO::MouseIO);

  kdDebug(12010) << "CurrrentPlayer=" << doc->QueryCurrentPlayer() << endl;
  kdDebug(12010) << "   " <<  doc->getPlayer(doc->QueryCurrentPlayer()) << endl;
  doc->getPlayer(doc->QueryCurrentPlayer())->setTurn(true,true);

  KMessageBox::information(this,i18n("The network game ended!\n"));
  doc->setGameStatus(oldstatus);
}

/**
 * A move is done. update status
 */
void Kwin4App::slotMoveDone(int /* x */ ,int /* y */ )
{
  checkMenus(CheckEditMenu);
  slotStatusNames();
  slotStatusMsg(i18n("Game running..."));
}

/**
 * The game is over or aborted
 */ 
void Kwin4App::slotGameOver(int status, KPlayer * p, KGame * /*me*/)
{
  if (status==-1) // remis
  {
    EndGame(TRemis);
    slotStatusMsg(i18n("The game is drawn. Please restart next round."));
  }
  else if (status==1)
  {
    if (p->userId()==Gelb)
      EndGame(TWin);
    else
      EndGame(TLost);
    QString msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(((FARBE)p->userId())));
    slotStatusMsg(msg);
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
  checkMenus(CheckEditMenu);
}

void Kwin4App::slotInitNetwork()
{
  if (doc->gameStatus()==Kwin4Doc::Intro) doc->setGameStatus(Kwin4Doc::Pause);

  QString host = Prefs::host();
  int port=Prefs::port();

  // just for testing - should be non-modal
  KGameDialog dlg(doc, 0, i18n("Network Configuration"), this,
      KGameDialog::NetworkConfig, 20000, true);
  dlg.networkConfig()->setDefaultNetworkInfo(host, port);
  dlg.networkConfig()->setDiscoveryInfo("_kwin4._tcp",Prefs::gamename());

  QVBox *box=dlg.configPage(KGameDialog::NetworkConfig);
  QVBoxLayout *l=(QVBoxLayout *)(box->layout());

  mColorGroup=new QVButtonGroup(box);
  connect(mColorGroup, SIGNAL(clicked(int)), this, SLOT(slotRemoteChanged(int)));
  connect(dlg.networkConfig(), SIGNAL(signalServerTypeChanged(int)), this, SLOT(slotServerTypeChanged(int)));

  new QRadioButton(i18n("Yellow should be played by remote"), mColorGroup);
  new QRadioButton(i18n("Red should be played by remote"), mColorGroup);
  l->addWidget(mColorGroup);
  mColorGroup->setButton(0);
  slotRemoteChanged(0);

  dlg.exec();// note: we don't have to check for the result - maybe a bug
}

/**
 * Arg can't get rid of this function in KGame's current state.
 * Can't pass a int signal to a bool slot, so this must be here
 */
void Kwin4App::slotServerTypeChanged(int t)
{
  mColorGroup->setDisabled(t);
}

/**
 * The remove player changed
 */
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
  if (!mMyChatDlg)
  {
    mMyChatDlg=new ChatDlg(doc,this);
    Kwin4Player *p=doc->getPlayer(Gelb);
    if (!p->isVirtual())
      mMyChatDlg->setPlayer(doc->getPlayer(Gelb));
    else
      mMyChatDlg->setPlayer(doc->getPlayer(Rot));
    connect(doc,SIGNAL(signalChatChanged(Kwin4Player *)),
            mMyChatDlg,SLOT(setPlayer(Kwin4Player *)));
  }
  
  if (mMyChatDlg->isHidden())
    mMyChatDlg->show();
  else
    mMyChatDlg->hide();
}

/**
 * Show the KGame debug window
 */ 
void Kwin4App::slotDebugKGame()
{
  KGameDebugDialog* debugWindow = new KGameDebugDialog(doc, this);
  debugWindow->show();
}

/**
 * Show Configure dialog.
 */
void Kwin4App::showSettings(){
  if(KConfigDialog::showDialog("settings"))
    return;

  KConfigDialog *dialog = new KConfigDialog(this, "settings", Prefs::self(), KDialogBase::Swallow);
  Settings *general = new Settings(0, "General");
  dialog->addPage(general, i18n("General"), "package_settings");
  connect(dialog, SIGNAL(settingsChanged()), doc, SLOT(loadSettings()));
  dialog->show();
}

#include "kwin4.moc"
