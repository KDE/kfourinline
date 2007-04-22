/***************************************************************************
                          kwin4  -  Boardgame for KDE
                             -------------------
    begin                : Sun Mar 26 12:50:12 CEST 2000
    copyright            : (C) |1995-2007 by Martin Heni
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

// Qt includes
#include <QRadioButton>
#include <QLayout>
#include <QTimer>

// KDE includes
#include <kvbox.h>
#include <kstandardgameaction.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kconfigdialog.h>
#include <kactioncollection.h>
#include <kbuttongroup.h>
#include <kicon.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kselectaction.h>

// KGame includes
#include <kgamechat.h>
#include <dialogs/kgamedialog.h>
#include <dialogs/kgamedialogconfig.h>
#include <dialogs/kgameconnectdialog.h>
#include <dialogs/kgameerrordialog.h>
#include <dialogs/kgamedebugdialog.h>

// application specific includes
#include "kwin4.h"
#include "chatdlg.h"
#include "kwin4doc.h"
#include "kwin4view.h"
#include "prefs.h"
#include "ui_settings.h"
#include "ui_statistics.h"

// Abbreviations
#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG                1003
#define ID_STATUS_MOVER              1002


// Configuration file
#include "config-src.h"

// Construct the main application window
KWin4App::KWin4App(QWidget *parent)
        : KXmlGuiWindow(parent), mView(0), mDoc(0), mMyChatDlg(0)
{
  #ifdef SRC_DIR
  kDebug() << "Found SRC_DIR =" << SRC_DIR << endl;
  KGlobal::dirs()->addResourceDir("data",QString(SRC_DIR)+QString("/grafix/"));
  QString theme = KStandardDirs::locate("data", "default.rc");
  kDebug() << "theme =" << theme << endl;
  #endif

  mThemeDirName = KGlobal::dirs()->findResourceDir("data","default.rc");
  kDebug() << "THEME DIR IS " << mThemeDirName << endl;

  // Setup application
  initGUI();
  initStatusBar();
  initDocument();

  // Scene
  mScene  = new QGraphicsScene(this);

  // Theme
  mTheme  = new ThemeManager("default.rc", this);

  // View
  mView   = new KWin4View(QSize(800,600),25,mScene,mTheme,this);
  mDoc->setView(mView);

  // Players
  mDoc->initPlayers();

  // Adjust GUI
  setCentralWidget(mView);
  setupGUI();

  // Read global config
  mDoc->readConfig(KGlobal::config().data());

  // Check menues
  checkMenus();

  // Skip intro?
  if (global_skip_intro)
  {
    menuNewGame();
  }
  // Start game automatically in demo mode
  else if (global_demo_mode)
  {
    QTimer::singleShot(11500, this,SLOT(menuNewGame()));
  }
}


// Destruct application
KWin4App::~KWin4App()
{
  kDebug() << "~KWin4App()" << endl;
  if (mDoc) delete mDoc;
  if (mView) delete mView;
  if (mScene) delete mScene;
  if (mTheme) delete mTheme;
  if (mMyChatDlg) delete mMyChatDlg;

}

// This method is called from various places
// and signals to check, uncheck and enable
// or disable all menu items.
// The menu parameter can limit this operation
// to one or more of the main menus (File,View,...)
void KWin4App::checkMenus(CheckFlags menu)
{
  bool localgame=(!mDoc->isNetwork());
  bool isRunning = (mDoc->gameStatus()==KGame::Run);

  // Check file menu
  if (!menu || (menu&CheckFileMenu))
  {
    changeAction("hint", !(!isRunning && localgame));
    changeAction("new_game", !isRunning);
    changeAction("save", isRunning);
    changeAction("end_game", isRunning);
  }

  // Edit menu
  if (!menu || (menu&CheckEditMenu))
  {
    if (!isRunning || !localgame)
    {
      disableAction("edit_undo");
    }
    else if (mDoc->getHistoryCnt()==0)
    {
      disableAction("edit_undo");
    }
    else if (mDoc->getCurrentMove()<1 )
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
    else if (mDoc->getHistoryCnt()==mDoc->getMaxMove())
    {
      disableAction("edit_redo");
    }
    else
    {
      enableAction("edit_redo");
    }
  }

  // Disable some menus in demo mode
  if (global_demo_mode)
  {
    disableAction(KStandardAction::name(KStandardAction::Preferences));
    disableAction("edit_undo");
    disableAction("edit_redo");
    disableAction("new_game");
    disableAction("end_game");
    disableAction("save");
    disableAction("open");
    disableAction("network_conf");
    disableAction("network_chat");
    disableAction("statistics");
    disableAction("hint");
  }
}

// Create the actions for the menu. This works together with the xml guirc file
void KWin4App::initGUI()
{
  QAction* action;

  action = KStandardGameAction::gameNew(this, SLOT(menuNewGame()), this);
  actionCollection()->addAction("new_game", action);
  ACTION("new_game")->setToolTip(i18n("Start a new game"));

  action = KStandardGameAction::load(this, SLOT(menuOpenGame()), this);
  actionCollection()->addAction("open", action);
  ACTION("open")->setToolTip(i18n("Open a saved game..."));

  action = KStandardGameAction::save(this, SLOT(menuSaveGame()), this);
  actionCollection()->addAction("save", action);
  ACTION("save")->setToolTip(i18n("Save a game..."));

  action = KStandardGameAction::end(this, SLOT(endGame()), this);
  actionCollection()->addAction("end_game", action);
  ACTION("end_game")->setToolTip(i18n("Ending the current game..."));
  ACTION("end_game")->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));

  action = actionCollection()->addAction("network_conf");
  action->setText(i18n("&Network Configuration..."));
  connect(action, SIGNAL(triggered(bool) ), SLOT(configureNetwork()));

  action = actionCollection()->addAction("network_chat");
  action->setText(i18n("Network Chat..."));
  connect(action, SIGNAL(triggered(bool) ), SLOT(configureChat()));

  if (global_debug>0)
  {
    action = actionCollection()->addAction("file_debug");
    action->setText(i18n("Debug KGame"));
    connect(action, SIGNAL(triggered(bool) ), SLOT(debugKGame()));
  }

  action = actionCollection()->addAction("statistics");
  action->setIcon(KIcon("flag"));
  action->setText(i18n("&Show Statistics"));
  connect(action, SIGNAL(triggered(bool)), SLOT(showStatistics()));
  ACTION("statistics")->setToolTip(i18n("Show statistics."));

  action = KStandardGameAction::hint(this, SLOT(askForHint()), this);
  actionCollection()->addAction("hint", action);
  ACTION("hint")->setToolTip(i18n("Shows a hint on how to move."));

  action = KStandardGameAction::quit(this, SLOT(close()), this);
  actionCollection()->addAction("game_exit", action);
  ACTION("game_exit")->setToolTip(i18n("Quits the program."));

  action = KStandardGameAction::undo(this, SLOT(undoMove()), this);
  actionCollection()->addAction("edit_undo", action);
  ACTION("edit_undo")->setToolTip(i18n("Undo last move."));

  action = KStandardGameAction::redo(this, SLOT(redoMove()), this);
  actionCollection()->addAction("edit_redo", action);
  ACTION("edit_redo")->setToolTip(i18n("Redo last move."));

  actionCollection()->addAction(KStandardAction::Preferences, this, SLOT(configureSettings()));

  // TODO: The actions need to work with translated theme names. How?
  QDir dir(mThemeDirName);
  QStringList filters;
  filters.append("*.rc");
  QStringList rcFiles = dir.entryList(filters);
  kDebug() << "Theme dir = " << mThemeDirName << endl;
  kDebug() << "Available theme files="<<rcFiles<<endl;

  action = actionCollection()->addAction("theme", new KSelectAction(i18n("Theme"), this));
  ((KSelectAction*)action)->setItems(rcFiles);
  connect( action, SIGNAL(triggered(const QString&)), SLOT(changeTheme(const QString&)) );
}


// Change the theme of the game
void KWin4App::changeTheme(const QString& name)
{
  QString theme = name;
  theme.replace("&","");
  mTheme->updateTheme(theme);
  // TODO: Write Preferences
}


// Create the status bar with the message part, the player part.
void KWin4App::initStatusBar()
{
  statusBar()->insertPermanentItem(i18n("This leaves space for the mover"),ID_STATUS_MOVER,0);
  statusBar()->insertItem(i18n("Ready"), ID_STATUS_MSG);

  displayStatusbarMover("");
  displayStatusMessage(i18n("Welcome to Four Wins"));
}


// Set up the document, i.e. the KGame object
// and connect all signals emitted by it
void KWin4App::initDocument()
{
  mDoc = new KWin4Doc(this);

  // KGame signals
  connect(mDoc,SIGNAL(signalGameOver(int, KPlayer*,KGame*)),
         this,SLOT(slotGameOver(int, KPlayer*,KGame *)));
  connect(mDoc,SIGNAL(signalNextPlayer(int)),
         this,SLOT(moveDone(int)));
  connect(mDoc,SIGNAL(signalClientLeftGame(int, int,KGame*)),
         this,SLOT(networkBroken(int, int, KGame*)));
  connect(mDoc,SIGNAL(signalGameRun()),
         this,SLOT(gameRun()));
}


// Enable or disable an action
void KWin4App::changeAction(const char* action, bool enable)
{
  if (!action)
  {
    return;
  }

  QAction* act=actionCollection()->action(action);
  if (act)
  {
    act->setEnabled(enable);
  }
}


// Store the current game
void KWin4App::saveProperties(KConfigGroup& /*cfg*/)
{
  QString filename = KStandardDirs::locateLocal("appdata", "current_game");
  mDoc->save(filename);
}

// Load current game back
void KWin4App::readProperties(const KConfigGroup& /*cfg*/)
{
  QString filename = KStandardDirs::locateLocal("appdata", "current_game");
  if(QFile::exists(filename))
  {
    mDoc->load(filename);
  }
}


// Load a game menu
void KWin4App::menuOpenGame()
{
  QString dir(":<kwin4>");
  QString filter("*");
  QString file("/tmp/kwin.save");
  if (global_debug < 1)
    file=KFileDialog::getOpenFileName(dir,filter,this);
  mDoc->load(file,true);
  checkMenus();
}

// Save game menu
void KWin4App::menuSaveGame()
{
  QString dir(":<kwin4>");
  QString filter("*");
  QString file("/tmp/kwin.save");
  if (global_debug < 1)
    file=KFileDialog::getSaveFileName(dir,filter,this);
  mDoc->save(file);
}


// Start a new game menu
void KWin4App::menuNewGame()
{
  // End the intro if it is running
  mDoc->setGameStatus(KWin4Doc::End);
  // Init the board and Clear the old game out
  mDoc->setGameStatus(KWin4Doc::Init);
  // Run it
  mDoc->setGameStatus(KWin4Doc::Run);
  // Display game status
  displayStatusMessage(i18n("Game running..."));
}


// Slot: Noticed that a new game started...update menus
void KWin4App::gameRun()
{
  updateStatusNames();
  checkMenus(All);
}


// Abort a running game
void KWin4App::endGame()
{
  mDoc->setGameStatus(KWin4Doc::Abort);
}


// Menu to ask for a game hint
void KWin4App::askForHint()
{
  if (mDoc) mDoc->calculateHint();
}


// Show statistics dialog
void KWin4App::showStatistics()
{
  QDialog dlg(this);
  Ui::Statistics ui;
  ui.setupUi(&dlg);

  ui.p1_name->setText(mDoc->getName(Yellow));
  ui.p1_won->display(mDoc->getStatistic(Yellow, TWin));
  ui.p1_drawn->display(mDoc->getStatistic(Yellow, TRemis));
  ui.p1_lost->display(mDoc->getStatistic(Yellow, TLost));
  ui.p1_aborted->display(mDoc->getStatistic(Yellow, TBrk));
  ui.p1_sum->display(mDoc->getStatistic(Yellow, TSum));

  ui.p2_name->setText(mDoc->getName(Red));
  ui.p2_won->display(mDoc->getStatistic(Red, TWin));
  ui.p2_drawn->display(mDoc->getStatistic(Red, TRemis));
  ui.p2_lost->display(mDoc->getStatistic(Red, TLost));
  ui.p2_aborted->display(mDoc->getStatistic(Red, TBrk));
  ui.p2_sum->display(mDoc->getStatistic(Red, TSum));

  if(dlg.exec() == QDialog::Rejected)
  {
    mDoc->resetStatistic();
  }
}


// Undo menu call
void KWin4App::undoMove()
{
  mDoc->undoMove();
  // Undo twice if computer is moving to keep player as input
  if (mDoc->playedBy(mDoc->getCurrentPlayer())==KGameIO::ProcessIO)
  {
    mDoc->undoMove();
  }

  // Refresh menus
  updateStatusNames();
  checkMenus(CheckEditMenu);
}

// Redo menu call
void KWin4App::redoMove()
{
  mDoc->redoMove();
  // Redo twice if computer is moving to keep player as input
  if (mDoc->playedBy(mDoc->getCurrentPlayer())==KGameIO::ProcessIO)
  {
    mDoc->redoMove();
  }
  updateStatusNames();
  checkMenus(CheckEditMenu);
}


// Set the given text into the statusbar change status message permanently
void KWin4App::displayStatusMessage(const QString &text)
{
  statusBar()->clearMessage();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}


// Set the string in the statusbar window for
// the player currently moving change status mover permanently
void KWin4App::displayStatusbarMover(const QString& text)
{
  statusBar()->clearMessage();
  statusBar()->changeItem(text, ID_STATUS_MOVER);
}


// Ends the current game.
// Called by the gameover signal
void KWin4App::EndGame(TABLE mode)
{
  mDoc->endGame(mode);
  mDoc->switchStartPlayer();
  updateStatusNames();
  checkMenus();

  // Automatically restart game in demo mode
  if (global_demo_mode)
  {
    QTimer::singleShot(10000, this,SLOT(menuNewGame()));
  }
}


// Set the names in the mover field
void KWin4App::updateStatusNames()
{
  QString msg;
  if (!(mDoc->gameStatus()==KGame::Run))
    msg=i18n("No game  ");
  else if (mDoc->getCurrentPlayer()==Yellow)
    msg=i18n(" %1 - Yellow ", mDoc->getName(Yellow));
  else if (mDoc->getCurrentPlayer())
    msg=i18n(" %1 - Red ", mDoc->getName(Red));
  else
    msg=i18n("Nobody  ");
  displayStatusbarMover(msg);
}

// Notification that the network connection is lost.
void KWin4App::networkBroken(int /*id*/, int oldstatus ,KGame * /*game */)
{
  kDebug(12010) <<  "KWin4App::networkBroken("<<oldstatus<<")" << endl;

  // Set all input devices back to default
  if (mDoc->playedBy(Yellow)==0)
    mDoc->setPlayedBy(Yellow,KGameIO::MouseIO);
  if (mDoc->playedBy(Red)==0)
    mDoc->setPlayedBy(Red,KGameIO::MouseIO);

  kDebug(12010) << "CurrrentPlayer=" << mDoc->getCurrentPlayer() << endl;
  kDebug(12010) << "   " <<  mDoc->getPlayer(mDoc->getCurrentPlayer()) << endl;

  // Activate input device
  mDoc->getPlayer(mDoc->getCurrentPlayer())->setTurn(true,true);

  // Issue message
  KMessageBox::information(this,i18n("The network game ended!\n"));

  // Restore status
  mDoc->setGameStatus(oldstatus);
}


// A move is done. Update status display.
void KWin4App::moveDone(int /*playerNumber*/)
{
  checkMenus(CheckEditMenu);
  updateStatusNames();
  displayStatusMessage(i18n("Game running..."));
}

// The game is over or aborted. Set status and display it.
void KWin4App::slotGameOver(int status, KPlayer* p, KGame* /*me*/)
{
  kDebug() << "KWin4App::slotGameOver" << endl;
  if (status==-1) // remis
  {
    EndGame(TRemis);
    displayStatusMessage(i18n("The game is drawn. Please restart next round."));
  }
  else if (status==1) // One of the players won
  {
    if (p->userId()==Yellow)
      EndGame(TWin);
    else
      EndGame(TLost);
    QString msg=i18n("%1 won the game. Please restart next round.", mDoc->getName(((COLOUR)p->userId())));
    displayStatusMessage(msg);
  }
  else if (status==2) // Abort
  {
    EndGame(TBrk);
    QString  m=i18n(" Game aborted. Please restart next round.");
    displayStatusMessage(m);
  }
  else
  {
    kError() << "Gameover with status " << status << ". This is unexpected and a serious problem" << endl;
  }
  checkMenus(CheckEditMenu);
}


// Show the network configuration dialog
void KWin4App::configureNetwork()
{
  if (mDoc->gameStatus()==KWin4Doc::Intro)
  {
    mDoc->setGameStatus(KWin4Doc::Pause);
  }

  QString host = Prefs::host();
  int port=Prefs::port();

  // just for testing - should be non-modal
  KGameDialog dlg(mDoc, 0, i18n("Network Configuration"), this,
      KGameDialog::NetworkConfig, 20000, true);
  dlg.networkConfig()->setDefaultNetworkInfo(host, port);
  dlg.networkConfig()->setDiscoveryInfo("_kwin4._tcp",Prefs::gamename());

  KVBox *box=dlg.configPage(KGameDialog::NetworkConfig);
  QVBoxLayout *l=(QVBoxLayout *)(box->layout());

  mColorGroup=new KButtonGroup(box);
  connect(mColorGroup, SIGNAL(clicked(int)), this, SLOT(remoteChanged(int)));
  connect(dlg.networkConfig(), SIGNAL(signalServerTypeChanged(int)), this, SLOT(serverTypeChanged(int)));

  new QRadioButton(i18n("Yellow should be played by remote"), mColorGroup);
  new QRadioButton(i18n("Red should be played by remote"), mColorGroup);
  l->addWidget(mColorGroup);
  mColorGroup->setSelected(0);
  remoteChanged(0);

  dlg.exec();// note: we don't have to check for the result - maybe a bug
}

// Can't get rid of this function in KGame's current state.
// Can't pass a int signal to a bool slot, so this must be here
void KWin4App::serverTypeChanged(int t)
{
  mColorGroup->setDisabled(t);
}


// The remote player in the network dialog has changed. Adapt priorities.
void KWin4App::remoteChanged(int button)
{
  if (button==0)
  {
    mDoc->getPlayer(Yellow)->setNetworkPriority(0);
    mDoc->getPlayer(Red)->setNetworkPriority(10);
  }
  else
  {
    mDoc->getPlayer(Yellow)->setNetworkPriority(10);
    mDoc->getPlayer(Red)->setNetworkPriority(0);
  }
}


// Show the chat dialog.
void KWin4App::configureChat()
{
  if (!mMyChatDlg)
  {
    mMyChatDlg=new ChatDlg(mDoc,this);
    KWin4Player *p=mDoc->getPlayer(Yellow);
    if (!p->isVirtual())
      mMyChatDlg->setPlayer(mDoc->getPlayer(Yellow));
    else
      mMyChatDlg->setPlayer(mDoc->getPlayer(Red));
    connect(mDoc,SIGNAL(signalChatChanged(KWin4Player *)),
            mMyChatDlg,SLOT(setPlayer(KWin4Player *)));
  }

  if (mMyChatDlg->isHidden())
    mMyChatDlg->show();
  else
    mMyChatDlg->hide();
}


// Show the KGame debug window.
void KWin4App::debugKGame()
{
  KGameDebugDialog* debugWindow = new KGameDebugDialog(mDoc, this);
  debugWindow->show();
}


// Show Configure dialog.
void KWin4App::configureSettings()
{
  static Ui::Settings ui; // Dialog is internally static anyway
  if(KConfigDialog::showDialog("settings"))
  {
    // The dialog need to refresh the buttons
    ui.kcfg_startcolourred->setChecked(Prefs::startcolourred());
    ui.kcfg_startcolouryellow->setChecked(Prefs::startcolouryellow());
    return;
  }

  KConfigDialog* dialog = new KConfigDialog(this, "settings", Prefs::self(), KPageDialog::Plain,
                               KDialog::Default|KDialog::Ok|KDialog::Apply|KDialog::Cancel|KDialog::Help,
                               KDialog::Ok,
                               true);
  QWidget* frame = new QWidget(dialog);
  ui.setupUi(frame);
  dialog->addPage(frame, i18n("General"), "package_settings");
  connect(dialog, SIGNAL(settingsChanged(const QString &)), mDoc, SLOT(loadSettings()));
  dialog->show();
}

#include "kwin4.moc"
