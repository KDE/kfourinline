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
#include <qdir.h>
//#include <qpainter.h>
//#include <qfiledialog.h>
#include <qstring.h>
//#include <qprogressdialog.h>
#include <qdir.h>
#include <qlayout.h>
//#include <qbuffer.h>
//#include <qlayout.h>
#include <qhgroupbox.h>
#include <qvbuttongroup.h>
#include <qvbox.h>
#include <qradiobutton.h>


#include <stdio.h>
//#include <unistd.h>

// include files for KDE
#include <kstddirs.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kaction.h>

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

#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG               1003
#define ID_STATUS_MOVER              1002
#define ID_STATUS_TIME               1001

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

 kdDebug() << "page=" << plainPage()<< endl;
 QGridLayout* mGridLayout=new QGridLayout(plainPage());
 kdDebug() << "grid=" << mGridLayout<< endl;

 QHBoxLayout* h = new QHBoxLayout(plainPage());
 QHGroupBox* b = new QHGroupBox(i18n("Chat"), plainPage());
 kdDebug() << "b=" << b<< endl;
 mChat = new KGameChat(game, 10000, b);
 h->addWidget(b, 1);
 h->addSpacing(10);
 mGridLayout->addLayout(h,0,0);

 
 QPushButton *mButton=new QPushButton(i18n("Configure..."),plainPage());
 kdDebug() << "button=" << mButton<< endl;
 mGridLayout->addWidget(mButton,1,1);

 adjustSize();


 mChatDlg=new KChatDialog(mChat,plainPage(),true);
 connect(mButton,SIGNAL(clicked()),mChatDlg,SLOT(show()));
}
void ChatDlg::setPlayer(KPlayer *p)
{
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



Kwin4App::Kwin4App() : KMainWindow(0)
{
  mMyChatDlg=0;
  mChat=0;
  view=0;
  doc=0;
  config=kapp->config();
  mAppTitle=i18n("Four wins");


   // localise data file
#ifdef SRCDIR
    kdDebug() << "SRCDIR=" << SRCDIR << endl;
    KGlobal::dirs()->addResourceDir("data", SRCDIR + QString::fromLatin1("../grafix/default"));
#else
    kdDebug() << "NO SRCDIR"  << endl;
#endif
   QString file="kwin4/grafix/default/grafix.rc";
   mGrafix=kapp->dirs()->findResourceDir("data",file); 
   if (mGrafix.isNull()) mGrafix="grafix/default/";
   else mGrafix+="kwin4/grafix/default/";
   if (global_debug>3) kdDebug() << "Localised datafile " << mGrafix << endl;


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

  ///////////////////////////////////////////////////////////////////
  // disable menu and toolbar items at startup


  setMinimumSize(580,400);      // TODO
  setMaximumSize(800,600);
  resize( 640, 480 );

  // better be last in init
  kdDebug() << "Vor cgheck menues" << endl;
  checkMenus();
  kdDebug() << "Applicatiuon constructor done" << endl;
}

Kwin4App::~Kwin4App()
{
   delete statusTimer;
}

void Kwin4App::checkMenus(int menu)
{
  bool localgame=(doc->playedBy(doc->QueryCurrentPlayer())!=KGameIO::ProcessIO) && (!doc->isNetwork());
  // kdDebug() << "Localgame=" << localgame << endl;
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
    else if (doc->QueryCurrentMove()<2 && ( doc->QueryCurrentPlayer()==Gelb && !localgame) )
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
    if (statusBar()->isHidden()) ((KToggleAction*)ACTION("show_statusbar"))->setChecked(false);
    else ((KToggleAction*)ACTION("show_statusbar"))->setChecked(true);
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

  (void)new KAction(i18n("&Network configuration..."),0, this, SLOT(slotInitNetwork()),
                       actionCollection(), "file_network");
  (void)new KAction(i18n("Chat Widget"),0, this, SLOT(slotChat()),
                       actionCollection(), "file_chat");
  (void)new KAction(i18n("Debug KGame"), 0, this, SLOT(slotDebugKGame()),
                       actionCollection(), "file_debug");
  (void)new KAction(i18n("Debug Disconnect"), 0, this, SLOT(slotDisconnect()),
                       actionCollection(), "file_disconnect");
 
  

  (void)new KAction(i18n("&Show Statistics..."),"flag", 0, this, SLOT(slotFileStatistics()),
                      actionCollection(), "statistics");
  ACTION("statistics")->setStatusText(i18n("Show all time statistics..."));
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

  (void)new KToggleAction(i18n("&View Statusbar"),0,this,SLOT(slotViewStatusBar()),
                      actionCollection(), "show_statusbar");
  ACTION("show_statusbar")->setStatusText(i18n("Toggle the statusbar..."));
  ACTION("show_statusbar")->setWhatsThis(i18n("Toggle the statusbar..."));

  (void)new KSelectAction(i18n("Startplayer"),0,this,SLOT(slotStartplayer()),
                      actionCollection(), "startplayer");
  ACTION("startplayer")->setStatusText(i18n("Changing startplayer..."));
  ACTION("startplayer")->setWhatsThis(i18n("Chooses which player begins the next game."));
  list.clear();
  list.append(i18n("&Yellow"));
  list.append(i18n("&Red"));
  ((KSelectAction *)ACTION("startplayer"))->setItems(list);

  (void)new KSelectAction(i18n("Yellow played by"),0,this,SLOT(slotPlayer1By()),
                      actionCollection(), "player1");
  ACTION("player1")->setStatusText(i18n("Change who plays the yellow player..."));
  ACTION("player1")->setWhatsThis(i18n("Change who plays the yellow player..."));
  list.clear();
  list.append(i18n("&Mouse"));
  list.append(i18n("&Computer"));
  list.append(i18n("&Keyboard"));
  ((KSelectAction *)ACTION("player1"))->setItems(list);
  (void)new KSelectAction(i18n("Red played by"),0,this,SLOT(slotPlayer2By()),
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

  KHelpMenu *helpMenu = new KHelpMenu(this, 0, true, actionCollection());
  connect( helpMenu, SIGNAL(showAboutApplication()), this, SLOT(slotHelpAbout()));
  
}

void Kwin4App::slotClearStatusMsg()
{
  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
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
}

void Kwin4App::initPlayers()
{
  doc->initPlayers();
}
void Kwin4App::initView()
{ 
  view = new Kwin4View(mGrafix,this);
  doc->setView(view);
  setCentralWidget(view);	
  // setCaption(doc->getTitle());
  setCaption(appTitle());
}

void Kwin4App::enableAction(const char *s)
{
  if (s)
  {
    KAction *act=actionCollection()->action(s);
    if (act) act->setEnabled(true);
  }
}

void Kwin4App::disableAction(const char *s)
{
  if (s)
  {
    KAction *act=actionCollection()->action(s);
    if (act) act->setEnabled(false);
  }
}


Kwin4Doc *Kwin4App::getDocument() const
{
  return doc;
}

void Kwin4App::saveOptions()
{	
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
#ifdef USE_TOOLBAR
  config->writeEntry("Show Toolbar", toolBar()->isVisible());
  config->writeEntry("ToolBarPos", (int) toolBar()->barPos());
#endif
  config->writeEntry("Show Statusbar",statusBar()->isVisible());
//  config->writeEntry("Recent Files", recentFiles);

  doc->WriteConfig(config);

}


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

void Kwin4App::saveProperties(KConfig *_cfg)
{
  if(doc->getTitle()!=i18n("Untitled") )
  {
    // saving to tempfile not necessary
  }
  else
  {
    QString filename=doc->getAbsFilePath();	
    _cfg->writeEntry("filename", filename);
		
    QString tempname = kapp->tempSaveName(filename);
    doc->saveDocument(tempname);
  }
}

void Kwin4App::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readEntry("filename", "");
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
  //setCaption(caption+": "+doc->getTitle());
  setCaption(appTitle());
}		

bool Kwin4App::queryClose()
{
  return true;
}

bool Kwin4App::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void Kwin4App::slotOpenFile()
{
  QString dir,filter,file;
  // Avoid file dialog
  if (global_debug>10) file="/tmp/kwin.save";
  else file=KFileDialog::getOpenFileName(dir,filter,this);
  kdDebug() << "slotOpenFile File="<<file<<endl;
  doc->load(file,true);
  kdDebug() << "slot open file done" << endl;
  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);
}
void Kwin4App::slotSaveFile()
{
  kdDebug() << "slotSaveFile" << endl;
  kdDebug() << "4000="<<((KGamePropertyInt *)(doc->findProperty(4000)))->value() << endl;
  QString dir,filter,file;
  // Avoid file dialog
  if (global_debug>10) file="/tmp/kwin.save";
  else file=KFileDialog::getSaveFileName(dir,filter,this);
  kdDebug() << "File="<<file<<endl;
  doc->save(file);

}
void Kwin4App::slotFileNew()
{

  NewGame(1);
  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);
}

/** Starts a new game */
void Kwin4App::NewGame(int mode)
{
  bool res1,res2;
  // FARBE farbe;
  //

  if (mode&1) doc->ResetGame();

  doc->StartGame();

  slotStatusNames();
  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);
}

void Kwin4App::slotFileClose()
{
  EndGame(TBrk);
  QString  m=i18n(" Game aborted. Please restart next round.");
  slotStatusMsg(m);

  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);
}

void Kwin4App::slotFileQuit()
{
  kdDebug() << "slotFileQuit" << endl;
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(w && !w->close())
	break;
    }
  }	
  kdDebug() << "slotFileQuit DONE" << endl;
}

/** show statistics */
void Kwin4App::slotFileStatistics()
{
  int res;

  StatDlg *dlg=new StatDlg(this,"Game statistics...");
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

void Kwin4App::slotEditUndo()
{

  /*
  if (!doc->IsUser(doc->QueryCurrentPlayer()))
  {
    KMessageBox::sorry(this,
      i18n("It is not your turn. You cannot undo now.\n"),
      appTitle());
    return ;
  }
  */

  doc->UndoMove();
  // Undo twice if computer is moving then
  if (doc->playedBy(doc->QueryCurrentPlayer())==KGameIO::ProcessIO) doc->UndoMove();

  // Prepare menus
  slotStatusNames();
  checkMenus(CheckEditMenu|CheckOptionsMenu);

}

void Kwin4App::slotEditRedo()
{

  /*
  if (!doc->IsUser(doc->QueryCurrentPlayer()))
  {
    KMessageBox::sorry(this,
      i18n("It is not your turn. You cannot redo now.\n"),
      appTitle());
    return ;
  }
  */
  doc->RedoMove();
  if (doc->playedBy(doc->QueryCurrentPlayer())==KGameIO::ProcessIO) doc->RedoMove();
  slotStatusNames();
  checkMenus(CheckEditMenu|CheckOptionsMenu);

}

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


void Kwin4App::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

void Kwin4App::slotStatusMover(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status mover permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MOVER);
}

void Kwin4App::slotStatusTime()
{
  ///////////////////////////////////////////////////////////////////
  // change status time permanently
  QString s;
  QTime now;

  now=QTime::currentTime();
  s=now.toString();
  s.truncate(5);

  statusBar()->clear();
  statusBar()->changeItem(s, ID_STATUS_TIME);
}


void Kwin4App::slotStatusHelpMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 2000);
}

void Kwin4App::slotStartplayer()
{
  int i=((KSelectAction *)ACTION("startplayer"))->currentItem();
  if (i==0) slotStartcolourYellow();
  else slotStartcolourRed();
}

/** change startcolour */
void Kwin4App::slotStartcolourRed()
{
  if (doc->QueryPlayerColour(0)==Gelb)
  {
    doc->SwitchStartPlayer();
    checkMenus(CheckOptionsMenu);
  }
}

/** change startcolour */
void Kwin4App::slotStartcolourYellow()
{

  if (doc->QueryPlayerColour(0)==Rot)
  {
    doc->SwitchStartPlayer();
    checkMenus(CheckOptionsMenu);
  }
}

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

/** change yellow mode */
void Kwin4App::slotYellowPlayer()
{
    doc->setPlayedBy(Gelb,KGameIO::MouseIO);
    checkMenus(CheckOptionsMenu);
	
}
void Kwin4App::slotYellowComputer()
{
    doc->setPlayedBy(Gelb,KGameIO::ProcessIO);
    checkMenus(CheckOptionsMenu);
	
}
void Kwin4App::slotYellowKeyboard()
{
  kdWarning() << "Keyboard not yet supported" << endl;
  // doc->setPlayedBy(Gelb,(KGameIO::IOMode)0);
  checkMenus(CheckOptionsMenu);
	
}
/** change red mode */
void Kwin4App::slotRedPlayer()
{
    doc->setPlayedBy(Rot,KGameIO::MouseIO);
    checkMenus(CheckOptionsMenu);
}

void Kwin4App::slotRedComputer()
{
  doc->setPlayedBy(Rot,KGameIO::ProcessIO);
  checkMenus(CheckOptionsMenu);
}
void Kwin4App::slotRedKeyboard()
{
  //doc->setPlayedBy(Rot,(KGameIO::IOMode)0);
  kdWarning() << "Keyboard not yet supported" << endl;
  checkMenus(CheckOptionsMenu);

}

void Kwin4App::slotLevel()
{
  int i=((KSelectAction *)ACTION("choose_level"))->currentItem();
  i++; // we start at 1

  doc->SetLevel(i);
  checkMenus(CheckOptionsMenu);
  	
}

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

  /*
int Kwin4App::extractGame(KEMessage *msg)
{
  if (!msg) return 0;

    doc->ResetGame();

    short sh1,sh2;
    QString s;
    char *fl;
    int i,j,size;
    bool switchcolour=false;

    msg->GetData("Beginner",sh1);
    msg->GetData("RemoteBeginner",sh2);


    if (sh2 && doc->IsRemote((FARBE)sh1))
    {
      switchcolour=true;
    }
    msg->GetData("Second",sh1);
    msg->GetData("RemoteSecond",sh2);
    if (sh2 && doc->IsRemote((FARBE)sh1))
    {
      switchcolour=true;
    }


    FARBE piece;
    for (i=0;i<geom.field_my;i++)
    {
      s.sprintf("Row%d",i);
      msg->GetData(s,fl,size);
      // if size!=7 we are fucked up...


      for (j=0;j<geom.field_mx;j++)
      {
         if (fl[j]!=0)
         {
            piece=(FARBE)fl[j];
            if (switchcolour && piece==Gelb) piece=Rot;
            else if (switchcolour && piece==Rot) piece=Gelb;
            doc->SetCurrentPlayer(piece) ;
            doc->MakeMove(j);
         }
      }
    }

    // Need to switch players
    msg->GetData("Beginner",sh1);
    if (switchcolour)
    {
      if ((FARBE)sh1==Gelb) sh1=(short)Rot;
      else sh1=(short)Gelb;
    }
    if ((FARBE)sh1!=doc->QueryPlayerColour(0))
    {
      if ((FARBE)sh1==Gelb) slotStartcolourYellow();
      else slotStartcolourRed();
    }

    msg->GetData("Aktzug",sh1);
    doc->SetCurrentMove((int)sh1);
    msg->GetData("Amzug",sh1);
    if (switchcolour)
    {
      if ((FARBE)sh1==Gelb) sh1=(short)Rot;
      else sh1=(short)Gelb;
    }
    doc->SetCurrentPlayer((FARBE)sh1);

    // msg->GetData("Second",sh1);
    msg->GetData("Level",sh1);
    doc->SetLevel((int)sh1);

    NewGame(0);

   // If we switched, player 2 has to begin game
   if (!switchcolour) return 0;
   else return 1;
}
*/


/*
void Kwin4App::prepareGame(KEMessage *msg)
{
  if (!msg) return ;
  msg->AddData("Aktzug",(short)doc->QueryCurrentMove());
  msg->AddData("Amzug",(short)doc->QueryCurrentPlayer());
  msg->AddData("Beginner",(short)doc->QueryPlayerColour(0));
  msg->AddData("Second",(short)doc->QueryPlayerColour(1));

    
  msg->AddData("Level",(short)doc->QueryLevel());
  msg->AddData("RemoteBeginner",(short)(doc->IsRemote(doc->QueryPlayerColour(0))));
  msg->AddData("RemoteSecond",(short)(doc->IsRemote(doc->QueryPlayerColour(1))));
  QString s;
  char *fl=new char[8];
  int i,j;
  for (i=0;i<geom.field_my;i++)
  {
    s.sprintf("Row%d",i);
    for (j=0;j<geom.field_mx;j++)
    {
      fl[j]=(char)doc->QueryColour(j,i);
    }
    msg->AddData(s,fl,(int)geom.field_mx);
  }
}
  */

/** Ends the current game */
void Kwin4App::EndGame(TABLE mode){
//  mInput->RemoveInput(1);
//  mInput->RemoveInput(0);
  doc->EndGame(mode);


  doc->SwitchStartPlayer();
  slotStatusNames();
  checkMenus();

}


//  readdy for next move
/*
bool Kwin4App::NextMove( MOVESTATUS res)
{
  QString msg;


  if (res==GYellowWin || res==GRedWin || res==GRemis)
  {
    doc->UpdateViews(UPDATE_TABLE,0,0);
  }

  if (res==GIllMove || res==GNotAllowed)
  {
     KMessageBox::sorry(this,
        i18n("This move is not possible.\n"),
        appTitle());
    return true;
  }
  else if (res==GYellowWin)
  {
    EndGame(TWin);
    checkMenus(CheckEditMenu);
    msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(Gelb));
    slotStatusMsg(msg);

    msg=i18n("%1 (Yellow) has won the game!").arg(doc->QueryName(Gelb));
    KMessageBox::information(this, msg, appTitle());
    return true;
  }
  else if (res==GRedWin)
  {
    EndGame(TLost);
    checkMenus(CheckEditMenu);
    msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(Rot));
    slotStatusMsg(msg);

    msg=i18n("%1 (Red) has won the game!").arg(doc->QueryName(Rot));
    KMessageBox::information(this,msg, appTitle());
    return true;
  }
  else if (res==GRemis)
  {
    EndGame(TRemis);
    checkMenus(CheckEditMenu);
    slotStatusMsg(i18n("The game is drawn. Please restart next round."));
    KMessageBox::information(this,i18n("The game ended remis!\n"),appTitle());
    return true;
  }
  slotStatusNames();

  mInput->Unlock();
  checkMenus(CheckEditMenu);

  if (Gelb==doc->QueryCurrentPlayer())
  {
    mInput->Next(0);
  }
  else if (Rot==doc->QueryCurrentPlayer())
  {
    mInput->Next(1);
  }
  else return false;

  return true;

}
*/



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
  checkMenus(CheckEditMenu);
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
    //KMessageBox::information(this,i18n("The game ended remis!\n"),appTitle());
  }
  else if (status==1)
  {
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

  QString host = "localhost";
  int port=4242;


 // just for testing - should be non-modal
  KGameDialog dlg(doc, 0, i18n("Configuration"), this,true,
   KGameDialog::NetworkConfig, 20000);
  dlg.networkConfig()->setDefaultNetworkInfo(host, port);

  QVBox *box=dlg.configPage(KGameDialog::NetworkConfig);
  /*
  QVBoxLayout *layout=(QVBoxLayout *)(dlg.networkConfig()->layout());
  kdDebug() << "Kwin4App::layout=" <<layout<< endl;
  */

  QVBoxLayout *l=(QVBoxLayout *)(box->layout());
  kdDebug() << "Kwin4App::layout=" <<l<< endl;

  QVButtonGroup *group=new QVButtonGroup(box);
  kdDebug() << "group="<<group<<endl;
  connect(group, SIGNAL(clicked(int)), this, SLOT(slotRemoteChanged(int)));

  
  (void)new QRadioButton(i18n("Yellow should be played by remote"), group);
  (void)new QRadioButton(i18n("Red should be played by remote"), group);
  l->addWidget(group);
  group->setButton(0);
  slotRemoteChanged(0);


  dlg.exec();// note: we don't have to check for the result - maybe a bug
  
/*  
  // start KGame configuration
  mDialog = new KGameDialog(mGame, i18n("Configure KGame"), this);
  connect(mDialog, SIGNAL(finished()), this, SLOT(slotDialogFinished())); 
  mDialog->show();*/
}

void Kwin4App::slotRemoteChanged(int button)
{
  kdDebug() << "Remote changed to "<<button<<endl;
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

// File Menu slot
// A debug function to force the computer to make a turn. This is
// necessary as the whole computer turn thingy is not working yet
void Kwin4App::slotChat()
{
  kdDebug() << "Kwin4App::Chat" << endl;

  if (!mMyChatDlg)
  {
    kdDebug() << "new KMyChatDialog" << endl;
    mMyChatDlg=new ChatDlg(doc,this);
    kdDebug() << "new KMyChatDialog DONE" << endl;
    mMyChatDlg->setPlayer(doc->getPlayer(Gelb));
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




#include "kwin4.moc"
