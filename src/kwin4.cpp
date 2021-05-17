/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 1995-2007 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kwin4.h"

// own
#include "chatdlg.h"
#include "kwin4doc.h"
#include "kwin4view.h"
#include "reflectiongraphicsscene.h"
#include "prefs.h"
#include "ui_settings.h"
#include "ui_statistics.h"
#include "kfourinline_debug.h"
#include "kgamedialog.h"
#include "kgamedialogconfig.h"
#include "kgamedebugdialog.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamechat.h>
// KF
#include <kwidgetsaddons_version.h>
#include <KActionCollection>
#include <KConfig>
#include <KConfigDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSelectAction>
#include <KStandardAction>
#include <KStandardGameAction>
// Qt
#include <QFileDialog>
#include <QGroupBox>
#include <QIcon>
#include <QLayout>
#include <QRadioButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>


// Abbreviations
#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG                1003
#define ID_STATUS_MOVER              1002


#define UPDATE_TIME  25   /* [ms] */

// Configuration file
#include "config-src.h"

// Construct the main application window
KWin4App::KWin4App(QWidget *parent)
        : KXmlGuiWindow(parent),
          mTheme(),
          mView(),
          mDoc(),
          mScene(),
          mColorGroup(),
          mMyChatDlg(),
          mStatusMsg(),
          mStatusMover()
{
  // Read theme files
  QStringList themeList;
  const QString dir = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("grafix"), QStandardPaths::LocateDirectory);
  const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.desktop"));
  for (const QString& file : fileNames)
    themeList.append(dir + QLatin1Char('/') + file);

  if (themeList.isEmpty())
  {
    KMessageBox::error(this, i18n("Installation error: No theme list found."));
    QTimer::singleShot(0, this,&QWidget::close);
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
    qCDebug(KFOURINLINE_LOG) <<  "Found theme("<<i<<"): " <<themeList.at(i) <<" Name(i18n)="<<name<<" File="<<file << " default="<<isDefault;
  }
  mThemeIndexNo = themeIdxFromName(mThemeDefault);


  // Setup application
  mDoc = new KWin4Doc(this);
  qCDebug(KFOURINLINE_LOG) << "Init doc";
  QString aiEngine = mDoc->findProcessName();
  qCDebug(KFOURINLINE_LOG) << "Init AI" << aiEngine;
  if (aiEngine.isEmpty())
  {
    KMessageBox::error(this, i18n("Installation error: No AI engine found. Continue without AI."));
  }

 
  // Read properties (before GUI and thememanager but after new document)
  qCDebug(KFOURINLINE_LOG) << "read prop";
  readProperties();


  // Scene
  mScene  = new ReflectionGraphicsScene(UPDATE_TIME, this);


  // Theme
  QString themeFile = themefileFromIdx(mThemeIndexNo);
  qCDebug(KFOURINLINE_LOG) << "Loading theme" << themeFile << " #"<<mThemeIndexNo;
  mTheme  = new ThemeManager(themeFile, this);
  if (mTheme->checkTheme() != 0)
  {
    KMessageBox::error(this, i18n("Installation error: Theme file error."));
    QTimer::singleShot(0, this,&QWidget::close);
    return;
  }

  // View
  mView   = new KWin4View(UPDATE_TIME, QSize(800,600),mScene,mTheme,this);
  mDoc->setView(mView);
  connect(mView, &KWin4View::signalQuickStart, this, &KWin4App::quickStart);

           


  // Players (after view)
  qCDebug(KFOURINLINE_LOG) << "Init pl";
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
  mDoc->readConfig(KSharedConfig::openConfig().data());

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
    QTimer::singleShot(11500, this,&KWin4App::menuNewGame);
  }
}


// Destruct application
KWin4App::~KWin4App()
{
  qCDebug(KFOURINLINE_LOG) << "~KWin4App()";
  delete mDoc;
  delete mView;
  delete mScene;
  delete mTheme;
  delete mMyChatDlg;
  qCDebug(KFOURINLINE_LOG) << "~KWin4App()";
}


// Called by Qt when the window is closed
void KWin4App::closeEvent(QCloseEvent *event)
{
  endGame();
  saveProperties();
  KXmlGuiWindow::closeEvent(event);
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
int KWin4App::themeIdxFromName(const QString &name)
{
  QStringList list(mThemeFiles.keys());
  list.sort();
  for (int i=0; i < list.size(); ++i)
  {
    if (list[i] == name) return i;
  }
  qCCritical(KFOURINLINE_LOG) << "Theme index lookup failed for " << name;
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
  KStandardGameAction::gameNew(this, &KWin4App::menuNewGame, actionCollection());
  KStandardGameAction::load(this, &KWin4App::menuOpenGame, actionCollection());
  KStandardGameAction::save(this, &KWin4App::menuSaveGame, actionCollection());
  action = KStandardGameAction::end(this, &KWin4App::endGame, actionCollection());
  action->setWhatsThis(i18n("Ends a currently played game. No winner will be declared."));
  KStandardGameAction::hint(this, &KWin4App::askForHint, actionCollection());
  KStandardGameAction::quit(this, &KWin4App::close, actionCollection());
  
  action = actionCollection()->addAction( QStringLiteral( "network_conf" ));
  action->setText(i18n("&Network Configuration..."));
  connect(action, &QAction::triggered, this, &KWin4App::configureNetwork);

  action = actionCollection()->addAction( QStringLiteral( "network_chat" ));
  action->setText(i18n("Network Chat..."));
  connect(action, &QAction::triggered, this, &KWin4App::configureChat);

  action = actionCollection()->addAction( QStringLiteral( "statistics" ));
  action->setIcon(QIcon::fromTheme( QStringLiteral( "view-statistics" )));
  action->setText(i18n("&Show Statistics"));
  connect(action, &QAction::triggered, this, &KWin4App::showStatistics);
  action->setToolTip(i18n("Show statistics."));

  // Move
  KStandardGameAction::undo(this, &KWin4App::undoMove, actionCollection());
  KStandardGameAction::redo(this, &KWin4App::redoMove, actionCollection());

  KStandardAction::preferences(this, &KWin4App::configureSettings, actionCollection());

  // Add all theme files to the menu
  QStringList themes(mThemeFiles.keys());
  themes.sort();

  KSelectAction *themeAction = new KSelectAction(i18n("Theme" ), this);
  actionCollection()->addAction( QStringLiteral( "theme" ) , themeAction );
  themeAction->setIcon(QIcon::fromTheme( QStringLiteral( "games-config-theme" )));
  themeAction->setItems(themes);
  connect(themeAction, &KSelectAction::indexTriggered, this, &KWin4App::changeTheme);
  qCDebug(KFOURINLINE_LOG) << "Setting current theme item to" << mThemeIndexNo;
  themeAction->setCurrentItem(mThemeIndexNo);

  // Debug
  if (global_debug>0)
  {
    action = actionCollection()->addAction( QStringLiteral( "file_debug" ));
    action->setText(i18n("Debug KGame"));
    connect(action, &QAction::triggered, this, &KWin4App::debugKGame);
  }
}


// Change the theme of the game
void KWin4App::changeTheme(int idx)
{
  mThemeIndexNo = idx;
  QString themeFile = themefileFromIdx(idx);
  qCDebug(KFOURINLINE_LOG) << "Select theme" << themeFile;
  mTheme->updateTheme(themeFile);
  updateStatusNames();
}


// Create the status bar with the message part, the player part.
void KWin4App::initStatusBar()
{
  mStatusMsg = new QLabel();
  mStatusMover = new QLabel();
  statusBar()->addWidget(mStatusMsg);
  statusBar()->addPermanentWidget(mStatusMover);

  displayStatusMessage(i18n("Welcome to Four Wins"));
}


// Set up the document, i.e. the KGame object
// and connect all signals emitted by it
void KWin4App::connectDocument()
{
  // KGame signals
  connect(mDoc, &KWin4Doc::signalGameOver, this, &KWin4App::slotGameOver);
  connect(mDoc, &KWin4Doc::signalNextPlayer, this, &KWin4App::moveDone);
  connect(mDoc, &KWin4Doc::signalClientLeftGame, this, &KWin4App::networkBroken);
  connect(mDoc, &KWin4Doc::signalGameRun, this, &KWin4App::gameRun);
}


// Enable or disable an action
void KWin4App::changeAction(const char* action, bool enable)
{
  if (!action)
  {
    return;
  }

  QAction* act=actionCollection()->action(QLatin1String(action));
  if (act)
  {
    act->setEnabled(enable);
  }
}


// Save instance-specific properties. The function is
void KWin4App::saveProperties(KConfigGroup& grp)
{
  qCDebug(KFOURINLINE_LOG) << "SAVE PROPERTIES for GROUP" << grp.name();

  // Save current game?
  QString name = QStringLiteral("current_game")+grp.name();
  QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QLatin1Char('/') + name;
  bool isRunning = (mDoc->gameStatus()==KGame::Run);
  if (isRunning)
  {
    qCDebug(KFOURINLINE_LOG) << "Saving" << filename;
    mDoc->save(filename);
    grp.writeEntry("CurrentGame", filename);
  }
  else
  {
    QFile file(filename);
    qCDebug(KFOURINLINE_LOG) << "Deleting" << file.fileName();
    file.remove();
    grp.deleteEntry("CurrentGame");
  }
}


// Read instance-specific properties.
void KWin4App::readProperties(const KConfigGroup& grp)
{
  qCDebug(KFOURINLINE_LOG) << "READ PROPERTIES for GROUP" << grp.name();

  QString filename = grp.readEntry("CurrentGame", QString());
  qCDebug(KFOURINLINE_LOG) << "Filename is" << filename;
  
  if(!filename.isNull() && QFile::exists(filename))
  {
    qCDebug(KFOURINLINE_LOG) << "Loading" << filename;
    // TODO: CRASHES mDoc->load(filename);
    qCDebug(KFOURINLINE_LOG) << "Loading" << filename << "done";
  }

}


// Store the current game
void KWin4App::saveProperties()
{
  KConfig *config = KSharedConfig::openConfig().data();

  // Program data
  KConfigGroup cfg = config->group("ProgramData");
  cfg.writeEntry("ThemeIndexNo", mThemeIndexNo);

  mDoc->writeConfig(config);

  config->sync();
  qCDebug(KFOURINLINE_LOG) << "SAVED PROPERTIES";
}

// Load current game back
void KWin4App::readProperties()
{
  KConfig *config = KSharedConfig::openConfig().data();

  // Program data
  KConfigGroup cfg = config->group("ProgramData");
  mThemeIndexNo = cfg.readEntry("ThemeIndexNo", themeIdxFromName(mThemeDefault));
  if (mThemeIndexNo >= mThemeFiles.size()) mThemeIndexNo = 0;
  qCDebug(KFOURINLINE_LOG) << "Index = " << mThemeIndexNo << " def index=" << themeIdxFromName(mThemeDefault);


  qCDebug(KFOURINLINE_LOG) << "LOADED PROPERTIES";
}


// Load a game menu
void KWin4App::menuOpenGame()
{
  const QString file =
    (global_debug < 1) ?
    QFileDialog::getOpenFileName(this) :
    QStringLiteral("/tmp/kwin.save");
  mDoc->load(file,true);
  checkMenus();
}

// Save game menu
void KWin4App::menuSaveGame()
{
  const QString file =
    (global_debug < 1) ?
    QFileDialog::getSaveFileName(this) :
    QStringLiteral("/tmp/kwin.save");
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
  QTimer::singleShot(0, this,&KWin4App::menuNewGame);
}


// Start a new game menu
void KWin4App::menuNewGame()
{
  qCDebug(KFOURINLINE_LOG) << "MENU NEW GAME";
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
  QPointer<QDialog> dlg = new QDialog(this);
  Ui::Statistics ui;
  ui.setupUi(dlg);

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

  if(dlg->exec() == QDialog::Rejected)
  {
    mDoc->resetStatistic();
  }
  delete dlg;
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
  mStatusMsg->setText(text);
}


// Set the string in the statusbar window for
// the player currently moving change status mover permanently
void KWin4App::displayStatusbarMover(const QString& text)
{
  mStatusMover->setText(text);
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
    QTimer::singleShot(10000, this,&KWin4App::menuNewGame);
  }
}


// Set the names in the mover field
void KWin4App::updateStatusNames()
{
  QString msg;
  if (!(mDoc->gameStatus()==KGame::Run))
    msg=i18n("No game  ");
  else if (mDoc->getCurrentPlayer()==Yellow)
    msg=i18n(" %1 - %2 ", mDoc->getName(Yellow), mTheme->colorNamePlayer(0));
  else if (mDoc->getCurrentPlayer())
    msg=i18n(" %1 - %2 ", mDoc->getName(Red), mTheme->colorNamePlayer(1));
  else
    msg=i18n("Nobody  ");
  displayStatusbarMover(msg);
}

// Notification that the network connection is lost.
void KWin4App::networkBroken(int /*id*/, int oldstatus ,KGame * /*game */)
{
  qCDebug(KFOURINLINE_LOG) <<  "KWin4App::networkBroken("<<oldstatus<<")";

  // Set all input devices back to default
  if (mDoc->playedBy(Yellow)==0)
    mDoc->setPlayedBy(Yellow,KGameIO::MouseIO);
  if (mDoc->playedBy(Red)==0)
    mDoc->setPlayedBy(Red,KGameIO::MouseIO);

  qCDebug(KFOURINLINE_LOG) << "CurrrentPlayer=" << mDoc->getCurrentPlayer();
  qCDebug(KFOURINLINE_LOG) << "   " <<  mDoc->getPlayer(mDoc->getCurrentPlayer());

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
  qCDebug(KFOURINLINE_LOG) << "KWin4App::slotGameOver";
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
    qCCritical(KFOURINLINE_LOG) << "Gameover with status" << status << ". This is unexpected and a serious problem";
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
  KGameDialog dlg(mDoc, nullptr, i18n("Network Configuration"), this);
  dlg.networkConfig()->setDefaultNetworkInfo(host, port);
  dlg.networkConfig()->setDiscoveryInfo(QStringLiteral("_kfourinline._tcp"),Prefs::gamename());

  QWidget *box=dlg.configPage();
  QLayout *l=box->layout();

  mColorGroup=new QGroupBox(box);
  QVBoxLayout *grouplay=new QVBoxLayout(mColorGroup);
  connect(dlg.networkConfig(), &KGameDialogNetworkConfig::signalServerTypeChanged, this, &KWin4App::serverTypeChanged);

  QRadioButton *b1 = new QRadioButton(i18n("Black should be played by remote player"), mColorGroup);
  QRadioButton *b2 = new QRadioButton(i18n("Red should be played by remote player"), mColorGroup);
  grouplay->addWidget(b1);
  grouplay->addWidget(b2);
  l->addWidget(mColorGroup);
  b1->setChecked(true);
  remoteChanged(0);

  connect(b1, &QAbstractButton::toggled, this, [this](bool toggled) { if (toggled) remoteChanged(0); });
  connect(b2, &QAbstractButton::toggled, this, [this](bool toggled) { if (toggled) remoteChanged(1); });

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
    connect(mDoc, &KWin4Doc::signalChatChanged, mMyChatDlg, &ChatDlg::setPlayer);
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
  if(KConfigDialog::showDialog(QStringLiteral("settings")))
  {
    // The dialog need to refresh the buttons as they are not connectable via a signal-slot 
    // in KConfigDialog
    ui.kcfg_startcolourred->setChecked(Prefs::startcolourred());
    ui.kcfg_startcolourred->setText(mTheme->colorNamePlayer(0));
    ui.kcfg_startcolouryellow->setChecked(Prefs::startcolouryellow());
    ui.kcfg_startcolouryellow->setText(mTheme->colorNamePlayer(1));
    ui.kcfg_level->setValue(Prefs::level());
    ui.Input0->setTitle(i18n("%1 Plays With", mTheme->colorNamePlayer(0)));
    ui.Input1->setTitle(i18n("%1 Plays With", mTheme->colorNamePlayer(1)));
    ui.kcfg_input0mouse->setChecked(Prefs::input0mouse());
    ui.kcfg_input0key->setChecked(Prefs::input0key());
    ui.kcfg_input0ai->setChecked(Prefs::input0ai());
    ui.kcfg_input1mouse->setChecked(Prefs::input1mouse());
    ui.kcfg_input1key->setChecked(Prefs::input1key());
    ui.kcfg_input1ai->setChecked(Prefs::input1ai());

    return;
  }

  KConfigDialog* dialog = new KConfigDialog(this, QStringLiteral("settings"), Prefs::self());
  dialog->setFaceType(KPageDialog::Plain);
  dialog->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Help);
  dialog->button(QDialogButtonBox::Ok)->setDefault(true);
  dialog->setModal(true);
  //QT5 dialog->setHelp(QString(),"kfourinline");
  QWidget* frame = new QWidget(dialog);
  ui.setupUi(frame);
  ui.kcfg_startcolourred->setText(mTheme->colorNamePlayer(0));
  ui.kcfg_startcolouryellow->setText(mTheme->colorNamePlayer(1));
  ui.Input0->setTitle(i18n("%1 Plays With", mTheme->colorNamePlayer(0)));
  ui.Input1->setTitle(i18n("%1 Plays With", mTheme->colorNamePlayer(1)));
  dialog->addPage(frame, i18n("General"), QStringLiteral("games-config-options"));
  connect(dialog, &KConfigDialog::settingsChanged, mDoc, &KWin4Doc::loadSettings);
  dialog->show();
}


