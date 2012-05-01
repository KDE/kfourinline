/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 1995-2007 Martin Heni <kde@heni-online.de>

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
#include "kwin4.h"

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
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamechat.h>
#include <libkdegamesprivate/kgame/dialogs/kgamedialog.h>
#include <libkdegamesprivate/kgame/dialogs/kgamedialogconfig.h>
#include <libkdegamesprivate/kgame/dialogs/kgameconnectdialog.h>
#include <libkdegamesprivate/kgame/dialogs/kgameerrordialog.h>
#include <libkdegamesprivate/kgame/dialogs/kgamedebugdialog.h>

// application specific includes
#include "chatdlg.h"
#include "kwin4doc.h"
#include "kwin4view.h"
#include "reflectiongraphicsscene.h"
#include "prefs.h"
#include "ui_settings.h"
#include "ui_statistics.h"

// Abbreviations
#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG                1003
#define ID_STATUS_MOVER              1002


#define UPDATE_TIME  25   /* [ms] */

// Configuration file
#include <config-src.h>

// Construct the main application window
KWin4App::KWin4App(QWidget *parent)
        : KXmlGuiWindow(parent), mView(0), mDoc(0), mMyChatDlg(0)
{
  // default names for players
  (void)I18N_NOOP2("default name of first player", "Player 1");
  (void)I18N_NOOP2("default name of second player", "Player 2");

  mDoc       = 0;
  mView      = 0;
  mScene     = 0;
  mTheme     = 0;
  mMyChatDlg = 0;

  // Add resource type to grafix
  KGlobal::dirs()->addResourceType("kwin4theme", "appdata", "grafix/");

  #ifndef NDEBUG
  #ifdef SRC_DIR
  kDebug() << "Found SRC_DIR =" << SRC_DIR;
  KGlobal::dirs()->addResourceDir("kwin4theme",QString(SRC_DIR)+QString("/grafix/"));
  #endif
  #endif

  // Read theme files
  QStringList themeList =  KGlobal::dirs()->findAllResources("kwin4theme", "*.desktop", KStandardDirs::NoDuplicates);
  if (themeList.isEmpty())
  {
    KMessageBox::error(this, i18n("Installation error: No theme list found."));
    QTimer::singleShot(0, this,SLOT(close()));
    return;
  }

  // Read theme files
  for (int i = 0; i < themeList.size(); i++)
  {
    KConfig themeInfo( themeList.at(i), KConfig::SimpleConfig);
    KConfigGroup themeGroup(&themeInfo, "Theme");
    QString name   = themeGroup.readEntry("Name", QString());
    QString file   = themeGroup.readEntry("File", QString());
    bool isDefault = themeGroup.readEntry("Default", false);
    if (mThemeDefault.isNull()) mThemeDefault = name;
    if (isDefault) mThemeDefault = name;
    mThemeFiles[name] = file;
    kDebug() <<  "Found theme("<<i<<"): " <<themeList.at(i) <<" Name(i18n)="<<name<<" File="<<file << " default="<<isDefault;   
  }
  mThemeIndexNo = themeIdxFromName(mThemeDefault);


  // Setup application
  mDoc = new KWin4Doc(this);
  kDebug() << "Init doc";
  QString aiEngine = mDoc->findProcessName();
  kDebug() << "Init AI" << aiEngine;
  if (aiEngine.isEmpty())
  {
    KMessageBox::error(this, i18n("Installation error: No AI engine found. Continue without AI."));
  }

 
  // Read properties (before GUI and thememanager but after new document)
  kDebug() << "read prop";
  readProperties();


  // Scene
  mScene  = new ReflectionGraphicsScene(UPDATE_TIME, this);


  // Theme
  QString themeFile = themefileFromIdx(mThemeIndexNo);
  kDebug() << "Loading theme" << themeFile << " #"<<mThemeIndexNo;
  mTheme  = new ThemeManager(themeFile, this);
  if (mTheme->checkTheme() != 0)
  {
    KMessageBox::error(this, i18n("Installation error: Theme file error."));
    QTimer::singleShot(0, this,SLOT(close()));
    return;
  }

  // View
  mView   = new KWin4View(UPDATE_TIME, QSize(800,600),mScene,mTheme,this);
  mDoc->setView(mView);
  connect(mView, SIGNAL(signalQuickStart(COLOUR,KGameIO::IOMode,KGameIO::IOMode,int)), 
          this, SLOT(quickStart(COLOUR,KGameIO::IOMode,KGameIO::IOMode,int)));

           


  // Players (after view)
  kDebug() << "Init pl";
  mDoc->initPlayers();

  // Init GUI
  initGUI();
  initStatusBar();

  // Adjust GUI
  setCentralWidget(mView);
  setupGUI();

  // Connect signals
  connectDocument();
  // Read global config for document (after initPlayers)
  mDoc->readConfig(KGlobal::config().data());

  // Check menus
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
  kDebug() << "~KWin4App()";
  delete mDoc;
  delete mView;
  delete mScene;
  delete mTheme;
  delete mMyChatDlg;
  kDebug() << "~KWin4App()";
}


// Called by KMainWindow when the last window of the application is
bool KWin4App::queryExit()
{
  saveProperties();
  return true;
}


// Retrieve a theme file name from the menu index number
QString KWin4App::themefileFromIdx(int idx)
{
  QStringList list(mThemeFiles.keys());
  list.sort();
  QString themeFile = mThemeFiles[list.at(idx)];
  return themeFile;
}


// Retrieve a theme idx from a theme name 
int KWin4App::themeIdxFromName(QString name)
{
  QStringList list(mThemeFiles.keys());
  list.sort();
  for (int i=0; i < list.size(); ++i)
  {
    if (list[i] == name) return i;
  }
  kError() << "Theme index lookup failed for " << name;
  return 0;
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
    changeAction("move_hint", !(!isRunning && localgame));
    changeAction("game_new", !isRunning);
    changeAction("game_save", isRunning);
    changeAction("game_end", isRunning);
  }

  // Edit menu
  if (!menu || (menu&CheckEditMenu))
  {
    if (!isRunning || !localgame)
    {
      disableAction("move_undo");
    }
    else if (mDoc->getHistoryCnt()==0)
    {
      disableAction("move_undo");
    }
    else if (mDoc->getCurrentMove()<1 )
    {
      disableAction("move_undo");
    }
    else
    {
      enableAction("move_undo");
    }

    // Show redo
    if (!isRunning || !localgame)
    {
      disableAction("move_redo");
    }
    else if (mDoc->getHistoryCnt()==mDoc->getMaxMove())
    {
      disableAction("move_redo");
    }
    else
    {
      enableAction("move_redo");
    }
  }

  // Disable some menus in demo mode
  if (global_demo_mode)
  {
    disableAction(KStandardAction::name(KStandardAction::Preferences));
    disableAction("move_undo");
    disableAction("move_redo");
    disableAction("game_new");
    disableAction("game_end");
    disableAction("game_save");
    disableAction("game_open");
    disableAction("network_conf");
    disableAction("network_chat");
    disableAction("statistics");
    disableAction("move_hint");
  }
}

// Create the actions for the menu. This works together with the xml guirc file
void KWin4App::initGUI()
{
  QAction* action;

  // Game
  KStandardGameAction::gameNew(this, SLOT(menuNewGame()), actionCollection());
  KStandardGameAction::load(this, SLOT(menuOpenGame()), actionCollection());
  KStandardGameAction::save(this, SLOT(menuSaveGame()), actionCollection());
  action = KStandardGameAction::end(this, SLOT(endGame()), actionCollection());
  action->setWhatsThis(i18n("Ends a currently played game. No winner will be declared."));
  KStandardGameAction::hint(this, SLOT(askForHint()), actionCollection());
  KStandardGameAction::quit(this, SLOT(close()), actionCollection());
  
  action = actionCollection()->addAction( QLatin1String( "network_conf" ));
  action->setText(i18n("&Network Configuration..."));
  connect(action, SIGNAL(triggered(bool)), SLOT(configureNetwork()));

  action = actionCollection()->addAction( QLatin1String( "network_chat" ));
  action->setText(i18n("Network Chat..."));
  connect(action, SIGNAL(triggered(bool)), SLOT(configureChat()));

  action = actionCollection()->addAction( QLatin1String( "statistics" ));
  action->setIcon(KIcon( QLatin1String( "view-statistics" )));
  action->setText(i18n("&Show Statistics"));
  connect(action, SIGNAL(triggered(bool)), SLOT(showStatistics()));
  action->setToolTip(i18n("Show statistics."));

  // Move
  KStandardGameAction::undo(this, SLOT(undoMove()), actionCollection());
  KStandardGameAction::redo(this, SLOT(redoMove()), actionCollection());

  actionCollection()->addAction(KStandardAction::Preferences, this, SLOT(configureSettings()));

  // Add all theme files to the menu
  QStringList themes(mThemeFiles.keys());
  themes.sort();

  action = actionCollection()->addAction( QLatin1String( "theme" ) , new KSelectAction(i18n("Theme" ), this));
  action->setIcon(KIcon( QLatin1String( "games-config-theme" )));
  ((KSelectAction*)action)->setItems(themes);
  connect( action, SIGNAL(triggered(int)), SLOT(changeTheme(int)) );
  kDebug() << "Setting current theme item to" << mThemeIndexNo;
  ((KSelectAction*)action)->setCurrentItem(mThemeIndexNo);

  // Debug
  if (global_debug>0)
  {
    action = actionCollection()->addAction( QLatin1String( "file_debug" ));
    action->setText(i18n("Debug KGame"));
    connect(action, SIGNAL(triggered(bool)), SLOT(debugKGame()));
  }
}


// Change the theme of the game
void KWin4App::changeTheme(int idx)
{
  mThemeIndexNo = idx;
  QString themeFile = themefileFromIdx(idx);
  kDebug() << "Select theme" << themeFile;
  mTheme->updateTheme(themeFile);
}


// Create the status bar with the message part, the player part.
void KWin4App::initStatusBar()
{
  statusBar()->insertItem(i18n("Ready"), ID_STATUS_MSG,1);
  statusBar()->insertPermanentItem(i18n("This leaves space for the mover"),ID_STATUS_MOVER,0);
  statusBar()->setItemAlignment(ID_STATUS_MOVER, Qt::AlignLeft | Qt::AlignVCenter);
  statusBar()->setItemAlignment(ID_STATUS_MSG, Qt::AlignLeft | Qt::AlignVCenter);


  displayStatusbarMover("");
  displayStatusMessage(i18n("Welcome to Four Wins"));
}


// Set up the document, i.e. the KGame object
// and connect all signals emitted by it
void KWin4App::connectDocument()
{
  // KGame signals
  connect(mDoc,SIGNAL(signalGameOver(int,KPlayer*,KGame*)),
         this,SLOT(slotGameOver(int,KPlayer*,KGame*)));
  connect(mDoc,SIGNAL(signalNextPlayer(int)),
         this,SLOT(moveDone(int)));
  connect(mDoc,SIGNAL(signalClientLeftGame(int,int,KGame*)),
         this,SLOT(networkBroken(int,int,KGame*)));
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


// Save instance-specific properties. The function is
void KWin4App::saveProperties(KConfigGroup& grp)
{
  kDebug() << "SAVE PROPERTIES for GROUP" << grp.name();

  // Save current game?
  QString name = QString("current_game")+grp.name();
  QString filename = KStandardDirs::locateLocal("appdata", name);
  bool isRunning = (mDoc->gameStatus()==KGame::Run);
  if (isRunning)
  {
    kDebug() << "Saving" << filename;
    mDoc->save(filename);
    grp.writeEntry("CurrentGame", filename);
  }
  else
  {
    QFile file(filename);
    kDebug() << "Deleting" << file.fileName();
    file.remove();
    grp.deleteEntry("CurrentGame");
  }
}


// Read instance-specific properties.
void KWin4App::readProperties(const KConfigGroup& grp)
{
  kDebug() << "READ PROPERTIES for GROUP" << grp.name();

  QString filename = grp.readEntry("CurrentGame", QString());
  kDebug() << "Filename is" << filename;
  
  if(!filename.isNull() && QFile::exists(filename))
  {
    kDebug() << "Loading" << filename;
    // TODO: CRASHES mDoc->load(filename);
    kDebug() << "Loading" << filename << "done";
  }

}


// Store the current game
void KWin4App::saveProperties()
{
  KConfig *config = KGlobal::config().data();

  // Program data
  KConfigGroup cfg = config->group("ProgramData");
  cfg.writeEntry("ThemeIndexNo", mThemeIndexNo);

  mDoc->writeConfig(config);

  config->sync();
  kDebug() << "SAVED PROPERTIES";
}

// Load current game back
void KWin4App::readProperties()
{
  KConfig *config = KGlobal::config().data();

  // Program data
  KConfigGroup cfg = config->group("ProgramData");
  mThemeIndexNo = cfg.readEntry("ThemeIndexNo", themeIdxFromName(mThemeDefault));
  if (mThemeIndexNo >= mThemeFiles.size()) mThemeIndexNo = 0;
  kDebug() << "Index = " << mThemeIndexNo << " def index=" << themeIdxFromName(mThemeDefault);


  kDebug() << "LOADED PROPERTIES";
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


// Received quick start command from view
void KWin4App::quickStart(COLOUR startPlayer, KGameIO::IOMode input0, KGameIO::IOMode input1, int level)
{
  if (startPlayer == Yellow) 
  {
    Prefs::setStartcolourred(false);
    Prefs::setStartcolouryellow(true);
  }
  else if (startPlayer == Red) 
  {
    Prefs::setStartcolourred(true);
    Prefs::setStartcolouryellow(false);
  }
  if (level >= 0) 
  {
    Prefs::setLevel(level);
  }
  if (input0 == KGameIO::MouseIO)
  {
    Prefs::setInput0mouse(true);
    Prefs::setInput0key(false);
    Prefs::setInput0ai(false);
  }
  if (input0 == KGameIO::ProcessIO)
  {
    Prefs::setInput0mouse(false);
    Prefs::setInput0key(false);
    Prefs::setInput0ai(true);
  }
  if (input1 == KGameIO::MouseIO)
  {
    Prefs::setInput1mouse(true);
    Prefs::setInput1key(false);
    Prefs::setInput1ai(false);
  }
  if (input1 == KGameIO::ProcessIO)
  {
    Prefs::setInput1mouse(false);
    Prefs::setInput1key(false);
    Prefs::setInput1ai(true);
  }

  // Reload settings
  mDoc->loadSettings();

  // Start game (direct call will crash as intro object will be deleted)
  QTimer::singleShot(0, this,SLOT(menuNewGame()));
}


// Start a new game menu
void KWin4App::menuNewGame()
{
  kDebug() << "MENU NEW GAME";
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
  kDebug(12010) <<  "KWin4App::networkBroken("<<oldstatus<<")";

  // Set all input devices back to default
  if (mDoc->playedBy(Yellow)==0)
    mDoc->setPlayedBy(Yellow,KGameIO::MouseIO);
  if (mDoc->playedBy(Red)==0)
    mDoc->setPlayedBy(Red,KGameIO::MouseIO);

  kDebug(12010) << "CurrrentPlayer=" << mDoc->getCurrentPlayer();
  kDebug(12010) << "   " <<  mDoc->getPlayer(mDoc->getCurrentPlayer());

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
  kDebug() << "KWin4App::slotGameOver";
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
    QString  m=i18n(" Game ended. Please restart next round.");
    displayStatusMessage(m);
  }
  else
  {
    kError() << "Gameover with status" << status << ". This is unexpected and a serious problem";
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
  KGameDialog dlg(mDoc, 0, i18n("Network Configuration"), this, KGameDialog::NetworkConfig, 20000, true);
  dlg.networkConfig()->setDefaultNetworkInfo(host, port);
  dlg.networkConfig()->setDiscoveryInfo("_kfourinline._tcp",Prefs::gamename());

  KVBox *box=dlg.configPage(KGameDialog::NetworkConfig);
  QLayout *l=box->layout();

  mColorGroup=new KButtonGroup(box);
  QVBoxLayout *grouplay=new QVBoxLayout(mColorGroup);
  connect(mColorGroup, SIGNAL(clicked(int)), this, SLOT(remoteChanged(int)));
  connect(dlg.networkConfig(), SIGNAL(signalServerTypeChanged(int)), this, SLOT(serverTypeChanged(int)));

  QRadioButton *b1 = new QRadioButton(i18n("Black should be played by remote player"), mColorGroup);
  QRadioButton *b2 = new QRadioButton(i18n("Red should be played by remote player"), mColorGroup);
  grouplay->addWidget(b1);
  grouplay->addWidget(b2);
  l->addWidget(mColorGroup);
  mColorGroup->setSelected(0);
  remoteChanged(0);

  dlg.adjustSize();
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
    connect(mDoc,SIGNAL(signalChatChanged(KWin4Player*)),
            mMyChatDlg,SLOT(setPlayer(KWin4Player*)));
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
    // The dialog need to refresh the buttons as they are not conectable via a signal-slot 
    // in KConfigDialog
    ui.kcfg_startcolourred->setChecked(Prefs::startcolourred());
    ui.kcfg_startcolouryellow->setChecked(Prefs::startcolouryellow());
    ui.kcfg_level->setValue(Prefs::level());
    ui.kcfg_input0mouse->setChecked(Prefs::input0mouse());
    ui.kcfg_input0key->setChecked(Prefs::input0key());
    ui.kcfg_input0ai->setChecked(Prefs::input0ai());
    ui.kcfg_input1mouse->setChecked(Prefs::input1mouse());
    ui.kcfg_input1key->setChecked(Prefs::input1key());
    ui.kcfg_input1ai->setChecked(Prefs::input1ai());

    return;
  }

  KConfigDialog* dialog = new KConfigDialog(this, "settings", Prefs::self());
  dialog->setFaceType(KPageDialog::Plain);
  dialog->setButtons(KDialog::Default|KDialog::Ok|KDialog::Apply|KDialog::Cancel|KDialog::Help);
  dialog->setDefaultButton(KDialog::Ok);
  dialog->setModal(true);
  dialog->setHelp(QString(),"kfourinline");
  QWidget* frame = new QWidget(dialog);
  ui.setupUi(frame);
  dialog->addPage(frame, i18n("General"), "games-config-options");
  connect(dialog, SIGNAL(settingsChanged(QString)), mDoc, SLOT(loadSettings()));
  dialog->show();
}

#include "kwin4.moc"
