/***************************************************************************
                          FILENAME|  -  description
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
#include <qprinter.h>
#include <qpainter.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qprogressdialog.h>

#include <stdio.h>
#include <unistd.h>

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

// application specific includes
#include "kwin4.h"
#include "kwin4view.h"
#include "kwin4doc.h"
#include "KEMessage.h"

#include "namedlg.h"
#include "networkdlg.h"
#include "statdlg.h"
#include "geom.h"
#include "kenddlg.h"
#include "msgdlg.h"
#include "kwin4aboutdlg.h"

#define ACTION(x)   (actionCollection()->action(x))
#define ID_STATUS_MSG               1003
#define ID_STATUS_MOVER              1002
#define ID_STATUS_TIME               1001

Kwin4App::Kwin4App() : KMainWindow(0)
{
  config=kapp->config();
  mAppTitle=i18n("Four wins");


   // localise data file
   QString file="kwin4/grafix/aboute.png";
   mGrafix=kapp->dirs()->findResourceDir("data",file); 
   if (mGrafix.isNull()) mGrafix="grafix/";
   else mGrafix+="kwin4/grafix/";
   if (global_debug>3) kdDebug() << "Localised datafile" << (const char *)mGrafix.latin1() << endl;


  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initGUI();
  initStatusBar();
  setHelpMenuEnabled(false);
  createGUI(QString::null, false);
  if (toolBar()) toolBar()->hide();

  
  initDocument();
  initView();
	
  readOptions();

  mInput=new KEInput(this);
  doc->SetInputHandler(mInput);
  connect(mInput,SIGNAL(signalPrepareProcessMove(KEMessage *)),
          this,SLOT(slotPrepareProcessMove(KEMessage *)));
  connect(mInput,SIGNAL(signalPrepareRemoteMove(KEMessage *)),
          this,SLOT(slotPrepareRemoteMove(KEMessage *)));
  connect(mInput,SIGNAL(signalPrepareInteractiveMove(KEMessage *)),
          this,SLOT(slotPrepareInteractiveMove(KEMessage *)));
  connect(mInput,SIGNAL(signalReceiveInput(KEMessage *, int)),
          this,SLOT(slotReceiveInput(KEMessage *,int )));

  ///////////////////////////////////////////////////////////////////
  // disable menu and toolbar items at startup


  setMinimumSize(580,400);      // TODO
  setMaximumSize(800,600);
  resize( 580, 440 );

  // better be last in init
  checkMenus();
}

Kwin4App::~Kwin4App()
{
   delete mInput;
   delete statusTimer;
   delete blinkTimer;
}

void Kwin4App::checkMenus(int menu)
{
  if (!menu || (menu&CheckFileMenu))
  {
    // Show Hint ?
    if (!doc->IsRunning() || !doc->IsUser(doc->QueryCurrentPlayer())
        || mInput->QueryType(2)==KG_INPUTTYPE_PROCESS)
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
    else if ( doc->QueryServer() ||
              (!doc->IsRemote(Rot)&&!doc->IsRemote(Gelb) ) )
    {
      enableAction("new_game");
    }
    else if ( !doc->QueryServer() &&
              ( (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE &&
                mInput->QueryStatus(0)>0) ||
                (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE &&
                mInput->QueryStatus(1)>0)
              )
            )
    {
      disableAction("new_game");
    }
    else
    {
      enableAction("new_game");
    }
 
    // Show message
    if ( (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE &&
          mInput->QueryStatus(0)>0)   ||
        (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE &&
          mInput->QueryStatus(1)>0) )
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
    if (!doc->IsRunning() ||
        doc->IsRemote(Gelb) || doc->IsRemote(Rot)
        ||!doc->IsUser(doc->QueryCurrentPlayer()) )
    {
      disableAction("edit_undo");
    }
    else if (doc->QueryHistoryCnt()==0)
    {
      disableAction("edit_undo");
    }
    else if (doc->QueryCurrentMove()<2 &&
              ( (doc->QueryCurrentPlayer()==Gelb && !doc->IsUser(Rot) )
                || (doc->QueryCurrentPlayer()==Rot && !doc->IsUser(Gelb) )
              ))
    {
      disableAction("edit_undo");
    }
    else
    {
      enableAction("edit_undo");
    }

    // Show redo
    if (!doc->IsRunning() ||
        doc->IsRemote(Gelb) || doc->IsRemote(Rot)
        ||!doc->IsUser(doc->QueryCurrentPlayer()) )
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

    if (doc->IsUser(Gelb)) 
        ((KSelectAction *)ACTION("player1"))->setCurrentItem(0);
    else if (doc->IsComputer(Gelb))
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

    if (doc->IsUser(Rot)) 
        ((KSelectAction *)ACTION("player2"))->setCurrentItem(0);
    else if (doc->IsComputer(Rot))
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
 
    // Network
    if (doc->QueryServer()) ((KToggleAction*)ACTION("network_server"))->setChecked(true);
    else ((KToggleAction*)ACTION("network_server"))->setChecked(false);

    if (doc->QueryBlink()) ((KToggleAction*)ACTION("animations"))->setChecked(true);
    else ((KToggleAction*)ACTION("animations"))->setChecked(false);

  }

}

void Kwin4App::initGUI()
{
  QStringList list;
  
  (void)KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection(), "new_game");
  ACTION("new_game")->setStatusText(i18n("Starting a new game..."));
  ACTION("new_game")->setWhatsThis(i18n("Starting a new game..."));

  (void)new KAction(i18n("&End Game"),"stop", 0, this, SLOT(slotFileClose()),
                      actionCollection(), "end_game");
  ACTION("end_game")->setStatusText(i18n("Ending the current game..."));
  ACTION("end_game")->setWhatsThis(i18n("Aborts a currently played game. No winner will be declared."));

  (void)new KAction(i18n("&Show Statistics..."),"flag", 0, this, SLOT(slotFileStatistics()),
                      actionCollection(), "statistics");
  ACTION("statistics")->setStatusText(i18n("Show all time statistics..."));
  ACTION("statistics")->setWhatsThis(i18n("Shows the all time statistics which is kept in all sessions."));

  (void)new KAction(i18n("&Hint"),"help", CTRL+Key_H, this, SLOT(slotFileHint()),
                      actionCollection(), "hint");
  ACTION("hint")->setStatusText(i18n("Shows a hint on how to move."));
  ACTION("hint")->setWhatsThis(i18n("Shows a hint on how to move."));


  (void)new KAction(i18n("Send &Message..."), CTRL+Key_M, this, SLOT(slotFileMessage()),
                      actionCollection(), "send_message");
  ACTION("send_message")->setStatusText(i18n("Sending message to remote player..."));
  ACTION("send_message")->setWhatsThis(i18n("Allows you to talk with a remote player."));

  (void)KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection(), "game_exit");
  ACTION("game_exit")->setStatusText(i18n("Exiting..."));
  ACTION("game_exit")->setWhatsThis(i18n("Quits the program."));

  (void)KStdAction::undo(this, SLOT(slotEditUndo()), actionCollection(), "edit_undo");
  ACTION("edit_undo")->setStatusText(i18n("Undo last move."));
  ACTION("edit_undo")->setWhatsThis(i18n("Undo last move."));

  (void)KStdAction::redo(this, SLOT(slotEditRedo()), actionCollection(), "edit_redo");
  ACTION("edit_redo")->setStatusText(i18n("Redo last move."));
  ACTION("edit_redo")->setWhatsThis(i18n("Redo last move."));

  (void)new KToggleAction(i18n("&Statusbar"),0,this,SLOT(slotViewStatusBar()),
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
  list.append(i18n("&Player"));
  list.append(i18n("&Computer"));
  list.append(i18n("&Remote"));
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

  (void)new KToggleAction(i18n("Network server"), 0, this, SLOT(slotOptionsNetworkserver()),
                      actionCollection(), "network_server");
  ACTION("network_server")->setStatusText(i18n("Try to act as network master..."));
  ACTION("network_server")->setWhatsThis(i18n("Try to act as network master..."));

  (void)new KToggleAction(i18n("&Animations"), 0, this, SLOT(slotOptionsAnimations()),
                      actionCollection(), "animations");
  ACTION("animations")->setStatusText(i18n("Display animations."));
  ACTION("animations")->setWhatsThis(i18n("Display animations."));


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

/** It is unclear why we need setAccel..changeMenuAccel should do
    it....
*/

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

  blinkTimer=new QTimer(this);
  connect(blinkTimer,SIGNAL(timeout()),this,SLOT(slotBlinkTimer()));



}

void Kwin4App::initDocument()
{
  doc = new Kwin4Doc(this);
  doc->newDocument(config,mGrafix);
}

void Kwin4App::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new Kwin4View(this);
  doc->addView(view);
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

void Kwin4App::openDocumentFile(const char* _cmdl)
{

  doc->openDocument(QCString(_cmdl));
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
  if(doc->getTitle()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    QString filename=doc->getAbsFilePath();	
    _cfg->writeEntry("filename", filename);
    _cfg->writeEntry("modified", doc->isModified());
		
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
      doc->setModified();
      QFileInfo info(filename);
      doc->setAbsFilePath(info.absFilePath());
      doc->setTitle(info.fileName());
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
  kEndDlg *dlg=new kEndDlg(this);
  dlg->setCaption(appTitle());
  dlg->SetPixmap(doc->m_PixWin4About);
  int res=dlg->exec();
  delete dlg;
  if (res) return true;
  return false;;
}

bool Kwin4App::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void Kwin4App::slotFileNew()
{

  NewGame(1);
  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);

}

bool Kwin4App::MakeInputDevice(int i)
{
  bool res;
  KEMessage *msg;
  short port;
  QString host;
  FARBE farbe;
  // 0 is always Gelb
  if (i==0) farbe=Gelb;
  else farbe=Rot;

    if (doc->IsUser(farbe) && mInput->QueryType(i)!=KG_INPUTTYPE_INTERACTIVE)
    {
      if (global_debug>5) kdDebug() << "Make Interactive " << i << endl;
      res=mInput->SetInputDevice(i,KG_INPUTTYPE_INTERACTIVE);
    }
    else if (doc->IsRemote(farbe)&&
          (mInput->QueryType(i)!=KG_INPUTTYPE_REMOTE || mInput->QueryStatus(i)<=0 ) )
    {
      if (global_debug>5) kdDebug() << "Make Remote " << i << endl;
      if (!slotOptionsNetwork()) return false;
      port=doc->QueryPort();
      host=doc->QueryHost();
      msg=new KEMessage;
      msg->AddData("Port",(short)port);
      msg->AddData("IP",(char *)host.latin1());
      res=mInput->SetInputDevice(i,KG_INPUTTYPE_REMOTE,msg);
      if (!res)
      {
        QProgressDialog *progress;
        QString s;
        int tim,j;
        tim=10000;
        if (host!=""&&host.length()>0)
        {
          s=i18n("Remote connection to %1:%2...").arg(host).arg(port);
        }
        else
        {
          s=i18n("Offering remote connection on port %1 ...").arg(port);
        }
        progress=new QProgressDialog(s, i18n("Abort"), tim, this,0,true );
        progress->setCaption(appTitle());
        for (j=0; j<tim; j++)
        {
          progress->setProgress( j );
          if ( progress->wasCancelled() ) break;
          if (mInput->QueryStatus(i)>0) break;
          usleep(100);
        }
        // progress.setProgress(tim);
        delete progress;
      }
      delete msg;
      if (mInput->QueryStatus(i)>0)
      {
        res=true;

        // Negotiate Client server
        KEMessage *msg=new KEMessage;
        if (doc->QueryServer())
        {
          msg->AddData("RequestServer",(long)doc->QueryID());
        }
        else
        {
          if (global_debug>6) kdDebug() << "We want to be client." << endl;
          msg->AddData("RequestClient",(long)doc->QueryID());
        }
        prepareGame(msg);
        if (global_debug>6) kdDebug() << "Game prepared." << endl;
        mInput->SendMsg(msg,i);
        if (global_debug>6) kdDebug() << "Message send." << endl;

        delete msg;
      }
      else res=false;
    }
    else if (doc->IsComputer(farbe)&& mInput->QueryType(i)!=KG_INPUTTYPE_PROCESS)
    {
      QString path=kapp->dirs()->findExe(doc->QueryProcessName());
      if (path.isNull())
      {
        QFile f(doc->QueryProcessName());
        QFile f1(QString("kproc4/")+doc->QueryProcessName());
        if (f1.exists())
        {
          path=QString("kproc4/")+doc->QueryProcessName();
          kdDebug() << "Using kproc4 path for computer player:" << path << endl;
        }
        else if (f.exists())
        {
          path=doc->QueryProcessName();
          kdDebug() << "Using direct path for computer player:" << path << endl;
        }
        else kdDebug() << "Cannot find computer player file!!!!!" << endl;
      }
      if (global_debug>5)
      { 
        kdDebug() << "Make Process " << i << endl;
        kdDebug() << "Exe file found: " << path << endl;
      }
      if (path.isNull())  return false;
      msg=new KEMessage;
      msg->AddData("ProcessName",(char *)path.latin1());
      res=mInput->SetInputDevice(i,KG_INPUTTYPE_PROCESS,msg);
      delete msg;
    }
    else
    {
      if (global_debug>5) kdDebug() << "No new inputdevice for " << i << " needed" << endl;
      res=true;
    }
   return res;
}

/** Starts a new game */
void Kwin4App::NewGame(int mode)
{
  bool res1,res2;
  // FARBE farbe;
  //
  blinkTimer->stop();

  /** Fix two remotes */
  if (doc->IsRemote(Gelb) && doc->IsRemote(Rot))
  {
    doc->SetUser(Gelb);
  }

  // Yellow
  res1=MakeInputDevice(0);
  if (!res1 && doc->IsRemote(Gelb))
  {
       doc->SetUser(Gelb);
       res1=MakeInputDevice(0);
  }
  // Yellow
  res2=MakeInputDevice(1);
  if (!res2 && doc->IsRemote(Rot))
  {
       doc->SetUser(Rot);
       res2=MakeInputDevice(1);
  }
  if (!res1 || !res2)
  {
    QString e1;
    if (!res1 && !res2) e1=i18n("It is not possible to start the yellow and red player.\n");
    else if (!res1) e1=i18n("It is not possible to start the yellow player.\n");
    else e1=i18n("It is not possible to start the red player.\n");
    KMessageBox::error(this,
      e1+i18n("This is a serious error. If one player is played by\n"
           "the computer check for the existance of the file '%1'.\n")
           .arg(doc->QueryProcessName()),
      appTitle());
    mInput->RemoveInput(1);
    mInput->RemoveInput(0);
    return ;
  }



  if (mode&1) doc->ResetGame();

  
  if (doc->IsRemote(Gelb)|| doc->IsRemote(Rot) ) 
  {
    KEMessage *msg=new KEMessage;
    if (doc->QueryServer())
    {
      msg->AddData("RequestServer",(long)-1);
      prepareGame(msg);
    }
    else
    {
      msg->AddData("RequestClient",(long)-1);
    }
    if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
    {
      usleep(1000);
      mInput->SendMsg(msg,0);
    }
    if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
    {
      usleep(1000);
      mInput->SendMsg(msg,1);
    }
    delete msg;
  }

  doc->StartGame();

  slotStatusNames();
  doc->slotUpdateAllViews(0);

  mInput->Lock();



  // Start game here only if no remote player
  if (mInput->QueryType(0)!=KG_INPUTTYPE_REMOTE &&
      mInput->QueryType(1)!=KG_INPUTTYPE_REMOTE)
  {
    // player 0 starts
    mInput->Unlock();
    if (doc->QueryPlayerColour(0)==Gelb)
      mInput->Next(0);
    else
      mInput->Next(1);
  }
  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);
}

void Kwin4App::slotFileClose()
{
  // close();

  // TODO question box for really end game
  EndGame(TBrk);
  QString  m=i18n(" Game aborted. Please restart next round.");
  slotStatusMsg(m);

  KEMessage *msg=new KEMessage;
  msg->AddData("EndGame",(short)TBrk);
  if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
    mInput->SendMsg(msg,0);
  if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
    mInput->SendMsg(msg,1);

  msg->RemoveAll();
  msg->AddData("Terminate",(short)1);
  if (mInput->QueryType(0)==KG_INPUTTYPE_PROCESS)
    mInput->SendMsg(msg,0);
  if (mInput->QueryType(1)==KG_INPUTTYPE_PROCESS)
    mInput->SendMsg(msg,1);
  delete msg;

  checkMenus(CheckFileMenu|CheckEditMenu|CheckOptionsMenu);

}

void Kwin4App::slotFileQuit()
{
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
}

void Kwin4App::slotFileHint()
{

    QString path=kapp->dirs()->findExe(doc->QueryProcessName());
    if (global_debug>5)
    { 
      kdDebug() << "Make Hint Process" << endl;
      kdDebug() << "Exe file found: " << path << endl;
    }
    if (path.isNull()) 
    {
      KMessageBox::error(this,
        i18n("It is not possbile to start the hint process.\n"
          "This is a serious error. Check for the existance of\n"
          "the file '%1'.\n").arg(doc->QueryProcessName()),
      appTitle());
      return ;
    }

    KEMessage *msg=new KEMessage;
    // msg->AddData("ProcessName",(const char *)doc->QueryProcessName());
    msg->AddData("ProcessName",(char *)path.latin1());
    msg->AddData("Hint",(short)1);
    msg->AddData("Move",(short)0);
    msg->AddData("Hintmove",(short)doc->QueryCurrentMove());
    prepareGame(msg);
    mInput->SetInputDevice(2,KG_INPUTTYPE_PROCESS,msg);
    delete msg;
	
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
  dlg->SetPixmap(doc->m_PixWin4About);
  //dlg->Init();
  res=dlg->exec();
  if (res==-100)
  {
    doc->ResetStat();
    doc->slotUpdateAllViews(0);
  }
	
}

/** send message */
void Kwin4App::slotFileMessage()
{
  int res;

  MsgDlg *dlg=new MsgDlg(this,"Send message...");
  res=dlg->exec();
  if (res==QDialog::Accepted)
  {
    QString s;
    s=dlg->GetMsg();
    if (!s || s.length()<1) s="...";
    KEMessage *msg=new KEMessage;

    msg->AddData("Message",(char *)s.latin1());
     if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
      mInput->SendMsg(msg,0);
     if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
      mInput->SendMsg(msg,1);
    delete msg;
  }
	
}

void Kwin4App::slotEditUndo()
{

  if (!doc->IsUser(doc->QueryCurrentPlayer()))
  {
    KMessageBox::sorry(this,
      i18n("It is not your turn. You cannot undo now.\n"),
      appTitle());
    return ;
  }

  doc->UndoMove();
  // Undo twice if computer is moving then
  if (doc->IsComputer(doc->QueryCurrentPlayer())) doc->UndoMove();

  // Prepare menus
  // TODO koordinates for update
  doc->slotUpdateAllViews(0);
  slotStatusNames();
  checkMenus(CheckEditMenu|CheckOptionsMenu);

}

void Kwin4App::slotEditRedo()
{

  if (!doc->IsUser(doc->QueryCurrentPlayer()))
  {
    KMessageBox::sorry(this,
      i18n("It is not your turn. You cannot redo now.\n"),
      appTitle());
    return ;
  }
  doc->RedoMove();
  if (doc->IsComputer(doc->QueryCurrentPlayer())) doc->RedoMove();
  // TODO koordinates for update
  doc->slotUpdateAllViews(0);
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

void Kwin4App::slotBlinkTimer()
{
  int x,y;
  FARBE c;
  if (doc->QueryWinC()==Rot || doc->QueryWinC()==Gelb)
  {
    for (int i=0;i<4;i++)
    {
      x=doc->QueryWinX(i);
      y=doc->QueryWinY(i);
      if (x<0 || y<0) continue; // bug catch
      if (doc->QueryColour(x,y)==doc->QueryWinC() && doc->QueryBlink())
      {
        if (doc->QueryWinC()==Rot) c=RotWin;
        else c=GelbWin;
        doc->SetColour(x,y,c);
      }
      else
      {
        doc->SetColour(x,y,doc->QueryWinC());
      }
      doc->UpdateViews(UPDATE_XY,doc->QueryWinX(i),doc->QueryWinY(i)+1);
    }
  }
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
    doc->UpdateViews(UPDATE_STATUS|UPDATE_TABLE,0,0);
  }
}

/** change startcolour */
void Kwin4App::slotStartcolourYellow()
{

  if (doc->QueryPlayerColour(0)==Rot)
  {
    doc->SwitchStartPlayer();
    checkMenus(CheckOptionsMenu);
    doc->UpdateViews(UPDATE_STATUS|UPDATE_TABLE,0,0);
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
        slotYellowRemote();
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
        slotRedRemote();
    break;
  }
}

/** change yellow mode */
void Kwin4App::slotYellowPlayer()
{

    doc->SetUser(Gelb);
    if (doc->IsRunning())
    {
      MakeInputDevice(0);
    }
    checkMenus(CheckOptionsMenu);
    doc->UpdateViews(UPDATE_STATUS,0,0);
	
}
void Kwin4App::slotYellowComputer()
{
    doc->SetComputer(Gelb);
    if (doc->IsRunning())
    {
      MakeInputDevice(0);
    }
    checkMenus(CheckOptionsMenu);
    doc->UpdateViews(UPDATE_STATUS,0,0);
	
}
void Kwin4App::slotYellowRemote()
{

  doc->SetRemote(Gelb);
  if (doc->IsRunning())
  {
    if (!MakeInputDevice(0) )
    {
      slotYellowPlayer();
    }
  }
  checkMenus(CheckOptionsMenu);
  doc->UpdateViews(UPDATE_STATUS,0,0);
	
}
/** change red mode */
void Kwin4App::slotRedPlayer()
{

    doc->SetUser(Rot);
    if (doc->IsRunning())
    {
      MakeInputDevice(1 );
    }
    checkMenus(CheckOptionsMenu);
    doc->UpdateViews(UPDATE_STATUS,0,0);

}

void Kwin4App::slotRedComputer()
{
  if (!doc->IsComputer(Rot))
  {
    doc->SetComputer(Rot);
    if (doc->IsRunning())
    {
      MakeInputDevice(1);
    }
    checkMenus(CheckOptionsMenu);
    doc->UpdateViews(UPDATE_STATUS,0,0);
  }
	
}
void Kwin4App::slotRedRemote()
{
  doc->SetRemote(Rot);
  if (doc->IsRunning())
  {
    if (!MakeInputDevice(1) )
    {
      slotRedPlayer();
    }
  }
  checkMenus(CheckOptionsMenu);
  doc->UpdateViews(UPDATE_STATUS,0,0);

}

void Kwin4App::slotLevel()
{
  int i=((KSelectAction *)ACTION("choose_level"))->currentItem();
  i++; // we start at 1


  doc->SetLevel(i);
  checkMenus(CheckOptionsMenu);
  doc->UpdateViews(UPDATE_STATUS,0,0);
  	
}

void Kwin4App::slotOptionsAnimations()
{
  doc->SetBlink(1-doc->QueryBlink());
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
    doc->UpdateViews(UPDATE_TABLE|UPDATE_STATUS,0,0);
    slotStatusNames();
  }

  	
}

void Kwin4App::slotOptionsNetworkserver()
{
  if (doc->QueryServer())
  {
    doc->SetServer(false);
  }
  else
  {
    doc->SetServer(true);
  }
  checkMenus(CheckOptionsMenu);
}

int Kwin4App::slotOptionsNetwork()
{
  int res;
  QString server;

  NetworkDlg *dlg=new NetworkDlg(this,"Configure a network game...");
  dlg->SetPort(doc->QueryPort());
  dlg->SetHost(doc->QueryHost());
  res=dlg->exec();
  doc->SetPort(dlg->QueryPort());
  doc->SetHost(dlg->QueryHost());
  if (global_debug>3)
    kdDebug() << "Network Dlg " << dlg->QueryHost() << ":" << dlg->QueryPort() << endl;
  delete dlg;
  if (res==QDialog::Accepted) return 1;
  else return 0;
}

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
      /** Coulour's of row i */
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
    /** Coulour's of row i */
    for (j=0;j<geom.field_mx;j++)
    {
      fl[j]=(char)doc->QueryColour(j,i);
    }
    msg->AddData(s,fl,(int)geom.field_mx);
  }
}

void Kwin4App::slotPrepareProcessMove(KEMessage *msg)
{
  if (global_debug>3)
    kdDebug() << "+++ main should prepare process move" << endl;
  slotStatusMsg(i18n("Waiting for the computer to move..."));

  msg->AddData("Hint",(short)0);
  msg->AddData("Move",(short)1);

  if (global_debug>3)
    kdDebug() << "PREPARE GAME in processmove" << endl;
  prepareGame(msg);

}
void Kwin4App::slotPrepareRemoteMove(KEMessage * /*msg*/)
{
  if (global_debug>3)
    kdDebug() << "+++ main should prepare remote move" << endl;
  slotStatusMsg(i18n("Waiting for remote player..."));
}
void Kwin4App::slotPrepareInteractiveMove(KEMessage * /*msg*/)
{
  if (global_debug>3)
    kdDebug() << "+++ main should prepare interactive move" << endl;
  slotStatusMsg(i18n("Please make your move..."));
}
void Kwin4App::slotReceiveInput(KEMessage *msg,int id)
{
  if (global_debug>3)
    kdDebug() << "+++ main should process input with next=" << mInput->QueryNext() << " id=" << id << endl;
  if (global_debug>10)
  {
    QStrList *keys=msg->QueryKeys();
    char *it;
    kdDebug() << "  MESSAGESIZE=" << msg->QueryNumberOfKeys() << endl;
    for (it=keys->first();it!=0;it=keys->next())
    {
      kdDebug() << "    Key '" << it << "' type=" << msg->QueryType(QCString(it)) << endl;
    }
  }

  short move;
  long score;
  int size,start;
  char *p;
  QString message;

  if (msg->HasKey("EndGame"))
  {
    msg->GetData("EndGame",move);
    message=QString(i18n("Remote player ended game..."));
    KMessageBox::information(this,message,appTitle());
    message=i18n("Remote player ended game.");
    slotStatusMsg(message);

    KEMessage *msg2=new KEMessage;

    msg2->AddData("Terminate",(short)1);
    if (mInput->QueryType(0)==KG_INPUTTYPE_PROCESS)
      mInput->SendMsg(msg2,0);
    if (mInput->QueryType(1)==KG_INPUTTYPE_PROCESS)
      mInput->SendMsg(msg2,1);
    delete msg2;

    EndGame((TABLE)move);
  }
  if (msg->HasKey("RequestServer"))
  {
    msg->GetData("RequestServer",score);
    // We lost server challenge
    if (global_debug>3)
      kdDebug() << "CHALLENGE Server: id=" << doc->QueryID() << " , remoteid=" << score << endl;
    message=i18n("Game started as remote slave");
    if (doc->QueryServer() && (doc->QueryID()<=score || score==-1) )
    {
      slotOptionsNetworkserver();
      start=extractGame(msg);
      doc->slotUpdateAllViews(0);
    }
    else if (!doc->QueryServer())
    {
      start=extractGame(msg);
      doc->slotUpdateAllViews(0);
    }
    else
    {
        start=0;
        message=i18n("Game started as remote master");
    }
    slotStatusMsg(message);
    if (global_debug>3)
      kdDebug() << message << endl;
    
    mInput->Unlock();
    if (doc->QueryCurrentPlayer()==Gelb)
      mInput->Next(0,true);
    else
      mInput->Next(1,true);
  }
  else if (msg->HasKey("RequestClient"))
  {
    msg->GetData("RequestClient",score);
    // We lost client challenge
    if (global_debug>3)
      kdDebug() << "CHALLENGE client: id=" << doc->QueryID() << ", remoteid=" << score << endl;
    if (!doc->QueryServer() && doc->QueryID()>score)
    {
      slotOptionsNetworkserver();
      message=i18n("Game started as remote master");
      start=0;
    }
    else if (!doc->QueryServer())
    {
      start=extractGame(msg);
      doc->slotUpdateAllViews(0);
      message=i18n("Game started as remote slave");
    }
    else
    {
      message=i18n("Game started as remote master");
      start=0;
    }
    slotStatusMsg(message);
    mInput->Unlock();
    if (doc->QueryCurrentPlayer()==Gelb )
      mInput->Next(0);
    else
      mInput->Next(1);
  }

  if (msg->HasKey("ConnectionLost"))
  {
    if (msg->GetData("ConnectionLost",move))
    {
      if (move==0)
      {
        message=i18n("Remote connection lost for yellow...");
        KMessageBox::information(this,message,appTitle());
        slotStatusMsg(message);
        slotYellowPlayer();
      }
      else
      {
        message=i18n("Remote connection lost for red...");
        KMessageBox::information(this,message,appTitle());
        slotStatusMsg(message);
        slotRedPlayer();
      }
    }
  }
  if (msg->HasKey("Message"))
  {
    if (msg->GetData("Message",p,size))
    {
      message=QString("Message from remote player:\n")+p;
      KMessageBox::information(this,message,appTitle());
      if (global_debug>3)
        kdDebug() << "MESSAGE **** " << message << " ****" << endl;
    }
  }
  if (msg->HasKey("error"))
  {
    message=i18n("Received an error from the remote player");
    slotStatusMsg(message);
  }
  if (msg->HasKey("Move"))
  {
     slotClearStatusMsg();
     if (msg->GetData("Move",move))
     {
       Move(move,id);
     }
     if (msg->GetData("Score",score))
     {
       doc->SetScore(score);
     }
  }
  if (msg->HasKey("Hint"))
  {
     slotClearStatusMsg();
     if (msg->GetData("Hint",move))
     {
       short oldm;
       msg->GetData("Hintmove",oldm);
       if (doc->QueryCurrentMove()==oldm)
       {
         int hintx=doc->QueryLastHint();
         doc->MakeMove(move,Tip);
         int y=getDocument()->QueryHeight(move);
         doc->UpdateViews(UPDATE_XY|UPDATE_ARROW|UPDATE_STATUS,move,y+1);
         // Clear hint as well
         if (hintx>=0)
         {
           y=doc->QueryHeight(hintx)+1;
           doc->UpdateViews(UPDATE_XY,hintx,y);
         }
       }

       mInput->RemoveInput(2);

     }
  }

}
/** Ends the current game */
void Kwin4App::EndGame(TABLE mode){
//  mInput->RemoveInput(1);
//  mInput->RemoveInput(0);
  doc->EndGame(mode);

  blinkTimer->start(500,FALSE);

  doc->SwitchStartPlayer();
  slotStatusNames();
  doc->slotUpdateAllViews(0);

}

/** PErforms a game move */
bool Kwin4App::Move(int x,int id){
  MOVESTATUS res;
  int hintx,lastx;

  if (!doc->IsRunning()) return false;

  if (mInput->QueryNext()!=id || !mInput->IsLocked())
  {
       kdDebug() << "Trying to perform a move for a wrong player next=" << mInput->QueryNext() << "id=" << id << "locked=" << mInput->IsLocked() << endl;
       return false;
   }


  // The whole block can be removed. As it does not hurt
  // I keep it in until the next big change or release
  // Martin 211100
  if (doc->IsLocked() )
  {
     if (mInput->IsInteractive())
     {
      KMessageBox::sorry(this,
          i18n("Please wait... Computer is thinking.\n"),
          appTitle()
          );
     }
     else
     {
       kdDebug() << "Trying to perform a move but document is locked." << endl;
     }
    return false;
  }
  //  End of block to be removed

  hintx=doc->QueryLastHint();
  lastx=doc->QueryLastcolumn();
  int isremote=doc->IsRemote(doc->QueryCurrentPlayer());
  res=doc->MakeMove(x);
  if (res!=GIllMove && res!=GTip && !isremote)
  {
     KEMessage *msg=new KEMessage;
     msg->AddData("Move",(short)x);
     if (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE)
      mInput->SendMsg(msg,0);
     if (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE)
      mInput->SendMsg(msg,1);
     delete msg;
  }

  int y;
  // Clear hint as well
  if (hintx>=0)
  {
    y=doc->QueryHeight(hintx)+1;
    doc->UpdateViews(UPDATE_XY,hintx,y);
  }

  y=getDocument()->QueryHeight(x);
  doc->UpdateViews(UPDATE_XY|UPDATE_ARROW|UPDATE_STATUS,x,y);
  doc->UpdateViews(UPDATE_ARROW,lastx,0);
  return NextMove(res);


}

//  readdy for next move
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

  // kwin4AboutDlg *dlg=new kwin4AboutDlg(&doc->m_PixWin4About,this);
  aboutDlg *dlg=new aboutDlg(this);
  dlg->SetPixmap(doc->m_PixWin4About);
  dlg->exec();
}

void Kwin4App::SetGrafix(QString s)
{
  mGrafix=s;
}
#include "kwin4.moc"
