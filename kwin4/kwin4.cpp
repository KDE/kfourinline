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

// application specific includes
#include "kwin4.h"
#include "kwin4view.h"
#include "kwin4doc.h"
#include "resource.h"
#include "KEMessage.h"

#include "namedlg.h"
#include "networkdlg.h"
#include "statdlg.h"
#include "geom.h"
#include "kenddlg.h"
#include "msgdlg.h"
#include "kwin4aboutdlg.h"


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
  initMenuBar();
  initToolBar();
  initStatusBar();
  initKeyAccel();
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

}

Kwin4App::~Kwin4App()
{
   delete mInput;
   delete statusTimer;
   delete blinkTimer;
}

/** It is unclear why we need setAccel..changeMenuAccel should do
    it....
*/
void Kwin4App::initKeyAccel()
{
  keyAccel = new KAccel(this);

//  keyAccel->setEnabled(true);
	
  // fileMenu accelerators
//  keyAccel->connectItem(KStdAccel::New, this, SLOT(slotFileNew()));
  keyAccel->insertItem( i18n("Start Game"),QCString( "StartGame"), Key_F2 );
  keyAccel->connectItem("StartGame", this, SLOT(slotFileNew()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_NEW, "StartGame");
//  keyAccel->changeMenuAccel(fileMenu, ID_FILE_NEW, KStdAccel::New);
//  fileMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::New)), ID_FILE_NEW);
  fileMenu->setAccel( Key_F2, ID_FILE_NEW);



//  keyAccel->connectItem(KStdAccel::Open, this, SLOT(slotFileOpen()));
//  keyAccel->connectItem(KStdAccel::Save, this, SLOT(slotFileSave()));
  keyAccel->connectItem(KStdAccel::Close, this, SLOT(slotFileClose()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_CLOSE, KStdAccel::Close);
  fileMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Close)), ID_FILE_CLOSE);


//  keyAccel->connectItem(KStdAccel::Print, this, SLOT(slotFilePrint()));
  keyAccel->connectItem(KStdAccel::Quit, this, SLOT(slotFileQuit()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_QUIT, KStdAccel::Quit);
  fileMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Quit)), ID_FILE_QUIT);

  keyAccel->insertItem( i18n("Statistics"), "Statistics", CTRL+Key_I );
  keyAccel->connectItem("Statistics", this, SLOT(slotFileStatistics()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_STATISTICS, "Statistics" );
  fileMenu->setAccel( CTRL + Key_I, ID_FILE_STATISTICS);

  keyAccel->insertItem( i18n("Hint"), "Hint", CTRL+Key_H );
  keyAccel->connectItem("Hint", this, SLOT(slotFileHint()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_HINT, "Hint" );
  fileMenu->setAccel( CTRL + Key_H, ID_FILE_HINT);

  keyAccel->insertItem( i18n("Send Message"), "Send Message", CTRL+Key_M );
  keyAccel->connectItem("Send Message", this, SLOT(slotFileMessage()));
  keyAccel->changeMenuAccel(fileMenu, ID_FILE_MESSAGE, "Send Message" );
  fileMenu->setAccel( CTRL + Key_M, ID_FILE_MESSAGE);


  // editMenu accelerators
  keyAccel->connectItem(KStdAccel::Undo, this, SLOT(slotEditUndo()));
  keyAccel->changeMenuAccel(editMenu, ID_EDIT_UNDO, KStdAccel::Undo );
  editMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Undo)), ID_EDIT_UNDO);

  keyAccel->connectItem(KStdAccel::Redo, this, SLOT(slotEditRedo()));
  keyAccel->changeMenuAccel(editMenu, ID_EDIT_REDO, KStdAccel::Redo );
  editMenu->setAccel(keyAccel->currentKey(KStdAccel::action(KStdAccel::Redo)), ID_EDIT_REDO);

  keyAccel->connectItem(KStdAccel::Help, this, SLOT(appHelpActivated()));
  			
//    keyAccel->changeMenuAccel(editMenu, ID_EDIT_UNDO, KStdAccel::Undo);
//    keyAccel->changeMenuAccel(editMenu, ID_EDIT_REDO, KStdAccel::Redu);

  keyAccel->readSettings();	
}

void Kwin4App::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR

  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu
  fileMenu = new QPopupMenu();
//  fileMenu->insertSeparator();
  fileMenu->insertItem(BarIcon("filenew"), i18n("&New game"), ID_FILE_NEW);
  fileMenu->insertItem(BarIcon("stop"),i18n("&End game"), ID_FILE_CLOSE);
  fileMenu->insertSeparator();
  fileMenu->insertItem(BarIcon("flag"),i18n("&Statistics"), ID_FILE_STATISTICS);
  fileMenu->insertItem(BarIcon("help"),i18n("&Hint"), ID_FILE_HINT);
  fileMenu->insertItem(BarIcon("openterm"),i18n("Send &Message..."), ID_FILE_MESSAGE);
  fileMenu->insertSeparator();
  fileMenu->insertItem(BarIcon("exit"),i18n("E&xit"), ID_FILE_QUIT);

	
  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu = new QPopupMenu();
  editMenu->insertItem(BarIcon("undo"), i18n("&Undo move"), ID_EDIT_UNDO);
  editMenu->insertItem(BarIcon("redo"), i18n("&Redo move"), ID_EDIT_REDO);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu = new QPopupMenu();
  viewMenu->setCheckable(true);
#ifdef USE_TOOLBAR
  viewMenu->insertItem(i18n("&Toolbar"), ID_VIEW_TOOLBAR);
#endif
  viewMenu->insertItem(i18n("&Statusbar"), ID_VIEW_STATUSBAR);


  ///////////////////////////////////////////////////////////////////
  // menuBar entry optionsMenu
    QPopupMenu *optionsMenu = new QPopupMenu;
    popStartcolor = new QPopupMenu;
      popStartcolor->setCheckable(TRUE);

      popStartcolor->insertItem(i18n("&Yellow"),ID_STARTCOLOUR_YELLOW);
      popStartcolor->insertItem(i18n("&Red"),ID_STARTCOLOUR_RED);

    popYellow = new QPopupMenu;
      popYellow->setCheckable(TRUE);

      popYellow->insertItem(i18n("&Player"),ID_YELLOW_PLAYER);
      popYellow->insertItem(i18n("&Computer"),ID_YELLOW_COMPUTER);
      popYellow->insertItem(i18n("&Remote"),ID_YELLOW_REMOTE);

    popRed = new QPopupMenu;
      popRed->setCheckable(TRUE);

      popRed->insertItem(i18n("&Player"),ID_RED_PLAYER);
      popRed->insertItem(i18n("&Computer"),ID_RED_COMPUTER);
      popRed->insertItem(i18n("&Remote"),ID_RED_REMOTE);

    popLevel = new QPopupMenu;
      popLevel->setCheckable(TRUE);

      popLevel->insertItem(i18n("&1"),ID_LEVEL_1);
      popLevel->insertItem(i18n("&2"),ID_LEVEL_2);
      popLevel->insertItem(i18n("&3"),ID_LEVEL_3);
      popLevel->insertItem(i18n("&4"),ID_LEVEL_4);
      popLevel->insertItem(i18n("&5"),ID_LEVEL_5);
      popLevel->insertItem(i18n("&6"),ID_LEVEL_6);
      popLevel->insertSeparator();
      // On purpose no accelerators for high levels
      // cause all levels from here on are experimental
      // and might take a long time to play on slow machines
      popLevel->insertItem(i18n("7"),ID_LEVEL_7);
      popLevel->insertItem(i18n("8"),ID_LEVEL_8);
      popLevel->insertItem(i18n("9"),ID_LEVEL_9);
      popLevel->insertItem(i18n("10"),ID_LEVEL_10);

      optionsMenu->insertItem(i18n("&Starting color"),popStartcolor);
      optionsMenu->insertItem(i18n("&Yellow played by"),popYellow);
      optionsMenu->insertItem(i18n("&Red played by"),popRed);
      optionsMenu->insertItem(i18n("&Level"),popLevel);
      optionsMenu->insertItem(i18n("Change &Names..."),ID_OPTIONS_NAMES);
      optionsMenu->insertItem(i18n("Network server..."), ID_OPTIONS_NETWORK);
      optionsMenu->insertItem(i18n("&Animations"), ID_OPTIONS_ANIMATIONS);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
 KHelpMenu *mHelpMenu = new KHelpMenu( this,0,false );
 helpMenu_=mHelpMenu->menu();

 //helpMenu_ = helpMenu();
 connect( mHelpMenu, SIGNAL(showAboutApplication()),
      this, SLOT(slotHelpAbout()));

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  // insert your popup menus with the according menubar entries in the order
  // they will appear later from left to right
  menuBar()->insertItem(i18n("&Game"), fileMenu);
  menuBar()->insertItem(i18n("&Edit"), editMenu);
  menuBar()->insertItem(i18n("&View"), viewMenu);
  menuBar()->insertItem(i18n("&Options"), optionsMenu);

  menuBar()->insertSeparator();
  menuBar()->insertItem(i18n("&Help"), helpMenu_);

  ///////////////////////////////////////////////////////////////////
  // CONNECT THE MENU SLOTS WITH SIGNALS
  // for execution slots and statusbar messages

  connect(fileMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(fileMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(fileMenu, SIGNAL(aboutToShow()), SLOT(slotFileToShow()));

  connect(editMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(editMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(editMenu, SIGNAL(aboutToShow()), SLOT(slotEditToShow()));

  connect(viewMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(viewMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));

  connect(optionsMenu, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(optionsMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(optionsMenu, SIGNAL(aboutToShow()), SLOT(slotOptionsToShow()));

  connect(popStartcolor, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popStartcolor, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popStartcolor, SIGNAL(aboutToShow()), SLOT(slotStartcolorToShow()));

  connect(popYellow, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popYellow, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popYellow, SIGNAL(aboutToShow()), SLOT(slotYellowToShow()));

  connect(popRed, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popRed, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popRed, SIGNAL(aboutToShow()), SLOT(slotRedToShow()));

  connect(popLevel, SIGNAL(activated(int)), SLOT(commandCallback(int)));
  connect(popLevel, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  connect(popLevel, SIGNAL(aboutToShow()), SLOT(slotLevelToShow()));
}

void Kwin4App::initToolBar()
{
#ifdef USE_TOOLBAR

  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  toolBar()->insertButton(BarIcon("filenew"), ID_FILE_NEW, true, i18n("New File"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(BarIcon("editcut"), ID_EDIT_CUT, true, i18n("Cut"));
  toolBar()->insertButton(BarIcon("editcopy"), ID_EDIT_COPY, true, i18n("Copy"));
  toolBar()->insertButton(BarIcon("editpaste"), ID_EDIT_PASTE, true, i18n("Paste"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(BarIcon("help"), ID_HELP_CONTENTS, SIGNAL(clicked()),
  				this, SLOT(appHelpActivated()), true,i18n("Help"));

  ///////////////////////////////////////////////////////////////////
  // INSERT YOUR APPLICATION SPECIFIC TOOLBARS HERE WITH toolBar(n)


  ///////////////////////////////////////////////////////////////////
  // CONNECT THE TOOLBAR SLOTS WITH SIGNALS - add new created toolbars by their according number
  // connect for invoking the slot actions
  connect(toolBar(), SIGNAL(clicked(int)), SLOT(commandCallback(int)));
  // connect for the status help on holing icons pressed with the mouse button
  connect(toolBar(), SIGNAL(pressed(int)), SLOT(statusCallback(int)));
#endif
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

void Kwin4App::enableCommand(int id_)
{
  ///////////////////////////////////////////////////////////////////
  // enable menu and toolbar functions by their ID's
  menuBar()->setItemEnabled(id_, true);
#ifdef USE_TOOLBAR
  toolBar()->setItemEnabled(id_, true);
#endif
}

void Kwin4App::disableCommand(int id_)
{
  ///////////////////////////////////////////////////////////////////
  // disable menu and toolbar functions by their ID's
  menuBar()->setItemEnabled(id_, false);
#ifdef USE_TOOLBAR
  toolBar()->setItemEnabled(id_, false);
#endif
}

void Kwin4App::openDocumentFile(const char* _cmdl)
{
  slotStatusMsg(i18n("Opening file..."));

  doc->openDocument(QCString(_cmdl));
  slotStatusMsg(i18n("Ready"));
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
	
  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  viewMenu->setItemChecked(ID_VIEW_STATUSBAR, bViewStatusbar);
  if(bViewStatusbar)
    statusBar()->show();
  else
    statusBar()->hide();

  #ifdef USE_TOOLBAR
  // bar position settings
  KToolBar::BarPosition toolBarPos;
  toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
  toolBar()->setBarPos(toolBarPos);
#endif	
  // initialize the recent file list
  // recentFiles.setAutoDelete(TRUE);
  // config->readListEntry("Recent Files", recentFiles);
/*	
  for (int i=0; i < (int) recentFiles.count(); i++)
  {
    recentFilesMenu->insertItem(recentFiles.at(i));
  }
 */
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
  slotStatusMsg(i18n("Starting new game..."));

  NewGame(1);

  slotStatusMsg(i18n("Ready"));
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
}

void Kwin4App::slotFileClose()
{
  slotStatusMsg(i18n("Ending game..."));
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


//  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
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
//  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::slotFileHint()
{
    slotStatusMsg(i18n("Showing hint...  "));

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
	
//  slotStatusMsg(i18n("Ready"));
}
/** show statistics */
void Kwin4App::slotFileStatistics()
{
  int res;
  slotStatusMsg(i18n("Showing game statistics..."));

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
	
  slotStatusMsg(i18n("Ready"));
}

/** send message */
void Kwin4App::slotFileMessage()
{
  int res;
  slotStatusMsg(i18n("Sending message to remote player..."));

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
	
  slotStatusMsg(i18n("Ready"));
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

  slotStatusMsg(i18n("Undo move..."));
  doc->UndoMove();
  // Undo twice if computer is moving then
  if (doc->IsComputer(doc->QueryCurrentPlayer())) doc->UndoMove();

  // Prepare menus
  // TODO koordinates for update
  doc->slotUpdateAllViews(0);
  slotStatusNames();

  slotStatusMsg(i18n("Ready"));
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
  slotStatusMsg(i18n("Redo move..."));
  doc->RedoMove();
  if (doc->IsComputer(doc->QueryCurrentPlayer())) doc->RedoMove();
  // TODO koordinates for update
  doc->slotUpdateAllViews(0);
  slotStatusNames();

  slotStatusMsg(i18n("Ready"));
}




void Kwin4App::slotViewToolBar()
{
#ifdef USE_TOOLBAR
  slotStatusMsg(i18n("Toggle the toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if( viewMenu->isItemChecked(ID_VIEW_TOOLBAR))
  {
    viewMenu->setItemChecked(ID_VIEW_TOOLBAR, false);
    toolBar()->hide();
  }
  else
  {
    viewMenu->setItemChecked(ID_VIEW_TOOLBAR, true);
    toolBar()->show();
  }		

  slotStatusMsg(i18n("Ready"));
#endif
}

void Kwin4App::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if( viewMenu->isItemChecked(ID_VIEW_STATUSBAR))
  {
    viewMenu->setItemChecked(ID_VIEW_STATUSBAR, false);
    statusBar()->hide();
  }
  else
  {
    viewMenu->setItemChecked(ID_VIEW_STATUSBAR, true);
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready"));
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

/** change startcolour */
void Kwin4App::slotStartcolourRed()
{
  slotStatusMsg(i18n("Changing start color to red..."));

  if (doc->QueryPlayerColour(0)==Gelb)
  {
    doc->SwitchStartPlayer();
    slotStartcolorToShow();
    doc->UpdateViews(UPDATE_STATUS|UPDATE_TABLE,0,0);
  }
  slotStatusMsg(i18n("Ready"));
}

/** change startcolour */
void Kwin4App::slotStartcolourYellow()
{
  slotStatusMsg(i18n("Changing start color to yellow..."));

  if (doc->QueryPlayerColour(0)==Rot)
  {
    doc->SwitchStartPlayer();
    slotStartcolorToShow();
    doc->UpdateViews(UPDATE_STATUS|UPDATE_TABLE,0,0);
  }
  slotStatusMsg(i18n("Ready"));
}

/** change yellow mode */
void Kwin4App::slotYellowPlayer()
{
  slotStatusMsg(i18n("Changing yellow to interactive player..."));

    doc->SetUser(Gelb);
    if (doc->IsRunning())
    {
      MakeInputDevice(0);
    }
    slotYellowToShow();
    doc->UpdateViews(UPDATE_STATUS,0,0);
	
  slotStatusMsg(i18n("Ready"));
}
void Kwin4App::slotYellowComputer()
{
  slotStatusMsg(i18n("Changing yellow to computer player..."));
    doc->SetComputer(Gelb);
    if (doc->IsRunning())
    {
      MakeInputDevice(0);
    }
    slotYellowToShow();
    doc->UpdateViews(UPDATE_STATUS,0,0);
	
  slotStatusMsg(i18n("Ready"));
}
void Kwin4App::slotYellowRemote()
{
  slotStatusMsg(i18n("Changing yellow to remote player..."));

  doc->SetRemote(Gelb);
  if (doc->IsRunning())
  {
    if (!MakeInputDevice(0) )
    {
      slotYellowPlayer();
    }
  }
  slotYellowToShow();
  doc->UpdateViews(UPDATE_STATUS,0,0);
	
  slotStatusMsg(i18n("Ready"));
}
/** change red mode */
void Kwin4App::slotRedPlayer()
{
  slotStatusMsg(i18n("Changing red to interactive player..."));

    doc->SetUser(Rot);
    if (doc->IsRunning())
    {
      MakeInputDevice(1 );
    }
    slotRedToShow();
    doc->UpdateViews(UPDATE_STATUS,0,0);

  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::slotRedComputer()
{
  slotStatusMsg(i18n("Changing red to computer player..."));
  if (!doc->IsComputer(Rot))
  {
    doc->SetComputer(Rot);
    if (doc->IsRunning())
    {
      MakeInputDevice(1);
    }
    slotRedToShow();
    doc->UpdateViews(UPDATE_STATUS,0,0);
  }
	
  slotStatusMsg(i18n("Ready"));
}
void Kwin4App::slotRedRemote()
{
  slotStatusMsg(i18n("Changing red to remote player..."));
  doc->SetRemote(Rot);
  if (doc->IsRunning())
  {
    if (!MakeInputDevice(1) )
    {
      slotRedPlayer();
    }
  }
  slotRedToShow();
  doc->UpdateViews(UPDATE_STATUS,0,0);

  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::slotLevel(int i)
{
  slotStatusMsg( i18n("Changing computer level to %1 ...").arg(i));

  doc->SetLevel(i);
  slotLevelToShow();
  doc->UpdateViews(UPDATE_STATUS,0,0);
  	
  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::slotOptionsAnimations()
{
  doc->SetBlink(1-doc->QueryBlink());
}
void Kwin4App::slotOptionsNames()
{
  slotStatusMsg(i18n("Configure player names..."));
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

  	
  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::slotOptionsNetworkserver()
{
  slotStatusMsg(i18n("Trying to become network server if possible..."));
  if (doc->QueryServer())
  {
    doc->SetServer(false);
  }
  else
  {
    doc->SetServer(true);
  }
  slotOptionsToShow();


  slotStatusMsg(i18n("Ready"));
}

int Kwin4App::slotOptionsNetwork()
{
  slotStatusMsg(i18n("Configure network..."));
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
  slotStatusMsg(i18n("Ready"));
  if (res==QDialog::Accepted) return 1;
  else return 0;
}

void Kwin4App::commandCallback(int id_)
{
  switch (id_)
  {
    case ID_FILE_NEW:
    	 slotFileNew();
         break;

    case ID_FILE_QUIT:
         slotFileQuit();
         break;

    case ID_FILE_STATISTICS:
         slotFileStatistics();
         break;


    case ID_FILE_HINT:
         slotFileHint();
         break;

    case ID_FILE_MESSAGE:
         slotFileMessage();
         break;

    case ID_FILE_CLOSE:
         slotFileClose();
         break;

    case ID_EDIT_UNDO:
         slotEditUndo();
         break;

    case ID_EDIT_REDO:
         slotEditRedo();
         break;
  
    case ID_VIEW_TOOLBAR:
         slotViewToolBar();
         break;

    case ID_VIEW_STATUSBAR:
         slotViewStatusBar();
         break;

    case ID_STARTCOLOUR_YELLOW:
         slotStartcolourYellow();
         break;

    case ID_STARTCOLOUR_RED:
         slotStartcolourRed();
         break;

    case ID_YELLOW_PLAYER:
         slotYellowPlayer();
         break;

    case ID_YELLOW_COMPUTER:
         slotYellowComputer();
         break;

    case ID_YELLOW_REMOTE:
         slotYellowRemote();
         break;

    case ID_RED_PLAYER:
         slotRedPlayer();
         break;

    case ID_RED_COMPUTER:
         slotRedComputer();
         break;

    case ID_RED_REMOTE:
         slotRedRemote();
         break;

    case ID_LEVEL_1:
    case ID_LEVEL_2:
    case ID_LEVEL_3:
    case ID_LEVEL_4:
    case ID_LEVEL_5:
    case ID_LEVEL_6:
    case ID_LEVEL_7:
    case ID_LEVEL_8:
    case ID_LEVEL_9:
    case ID_LEVEL_10:
         slotLevel(id_-ID_LEVEL_1+1);
         break;

    case ID_OPTIONS_NAMES:
         slotOptionsNames();
         break;

    case ID_OPTIONS_NETWORK:
         slotOptionsNetworkserver();
         break;

    case ID_OPTIONS_ANIMATIONS:
         slotOptionsAnimations();
         break;

    default:
         break;
  }
}

void Kwin4App::statusCallback(int id_)
{
  QString s;
  switch (id_)
  {
    case ID_FILE_NEW:
         slotStatusHelpMsg(i18n("Starts a new game"));
         break;

    case ID_FILE_CLOSE:
         slotStatusHelpMsg(i18n("Ends the current game"));
         break;

    case ID_FILE_QUIT:
         slotStatusHelpMsg(i18n("Quits %1").arg(appTitle()));
         break;

    case ID_FILE_STATISTICS:
         slotStatusHelpMsg(i18n("Shows statistical information"));
         break;

    case ID_FILE_STOPREMOTE:
         slotStatusHelpMsg(i18n("Closes remote connection"));
         break;

    case ID_FILE_REMOTE:
         slotStatusHelpMsg(i18n("Connect to a remote game"));
         break;

    case ID_FILE_HINT:
         slotStatusHelpMsg(i18n("Shows you a hint"));
         break;

    case ID_FILE_MESSAGE:
         slotStatusHelpMsg(i18n("Sends a message to a remote player"));
         break;

    case ID_EDIT_UNDO:
         slotStatusHelpMsg(i18n("Undoes a move"));
         break;

    case ID_EDIT_REDO:
         slotStatusHelpMsg(i18n("Redoes a previously undone move"));
         break;

    case ID_VIEW_TOOLBAR:
         slotStatusHelpMsg(i18n("Enables/disables the toolbar"));
         break;

    case ID_VIEW_STATUSBAR:
         slotStatusHelpMsg(i18n("Enables/disables the statusbar"));
         break;

    case ID_STARTCOLOUR_RED:
         slotStatusHelpMsg(i18n("Lets red start the next game"));
         break;

    case ID_STARTCOLOUR_YELLOW:
         slotStatusHelpMsg(i18n("Lets yellow start the next game"));
         break;

    case ID_YELLOW_PLAYER:
         slotStatusHelpMsg(i18n("Yellow is played interactively"));
         break;

    case ID_YELLOW_COMPUTER:
         slotStatusHelpMsg(i18n("Yellow is played by the computer"));
         break;

    case ID_YELLOW_REMOTE:
         slotStatusHelpMsg(i18n("Yellow is played over the network"));
         break;

    case ID_RED_PLAYER:
         slotStatusHelpMsg(i18n("Red is played interactively"));
         break;

    case ID_RED_COMPUTER:
         slotStatusHelpMsg(i18n("Red is played by the computer"));
         break;

    case ID_RED_REMOTE:
         slotStatusHelpMsg(i18n("Red is played over the network"));
         break;

    case ID_LEVEL_1:
    case ID_LEVEL_2:
    case ID_LEVEL_3:
    case ID_LEVEL_4:
    case ID_LEVEL_5:
    case ID_LEVEL_6:
    case ID_LEVEL_7:
    case ID_LEVEL_8:
    case ID_LEVEL_9:
    case ID_LEVEL_10:
         s.setNum((int)(id_-ID_LEVEL_1+1));
         s=i18n("Set the level of the computer player to %1").arg(s);
         slotStatusHelpMsg(i18n(s.latin1()));
         break;

    case ID_OPTIONS_NAMES:
         slotStatusHelpMsg(i18n("Changes the names of the players"));
         break;

    case ID_OPTIONS_NETWORK:
         slotStatusHelpMsg(i18n("Try to become network server, if possible..."));
         break;

    case ID_OPTIONS_ANIMATIONS:
         slotStatusHelpMsg(i18n("Switch animations on/off..."));
         break;

    default:
         break;
  }
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
        message=QString(i18n("Remote connection lost for yellow..."));
        KMessageBox::information(this,message,appTitle());
        slotStatusMsg(i18n(message.latin1()));
        slotYellowPlayer();
      }
      else
      {
        message=QString(i18n("Remote connection lost for red..."));
        KMessageBox::information(this,message,appTitle());
        slotStatusMsg(i18n(message.latin1()));
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
    slotStatusMsg(i18n(message.latin1()));
  }
  if (msg->HasKey("Move"))
  {
     slotStatusMsg(i18n("Ready"));
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
     slotStatusMsg(i18n("Ready"));
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
  //  slotStatusMsg(i18n("Ready"));
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
    msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(Gelb));
    slotStatusMsg(msg);

    msg=i18n("%1 (Yellow) has won the game!").arg(doc->QueryName(Gelb));
    KMessageBox::information(this, msg, appTitle());
    return true;
  }
  else if (res==GRedWin)
  {
    EndGame(TLost);
    msg=i18n("%1 won the game. Please restart next round.").arg(doc->QueryName(Rot));
    slotStatusMsg(msg);

    msg=i18n("%1 (Red) has won the game!").arg(doc->QueryName(Rot));
    KMessageBox::information(this,msg, appTitle());
    return true;
  }
  else if (res==GRemis)
  {
    EndGame(TRemis);
    slotStatusMsg(i18n("The game is drawn. Please restart next round."));
    KMessageBox::information(this,i18n("The game ended remis!\n"),appTitle());
    return true;
  }
  slotStatusNames();

  mInput->Unlock();

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
/** Check a menuitem */
void Kwin4App::checkCommand(int id){
  menuBar()->setItemChecked(id, true);
}
/** Uncheck Menuitem */
void Kwin4App::uncheckCommand(int id){
  menuBar()->setItemChecked(id, false);
}
/** Is the menuitem enabled? */
bool Kwin4App::isEnabled(int id){
  return menuBar()->isItemEnabled(id);

}

void Kwin4App::slotFileToShow()
{

  // Show Hint ?
  if (!doc->IsRunning() || !doc->IsUser(doc->QueryCurrentPlayer())
      || mInput->QueryType(2)==KG_INPUTTYPE_PROCESS)
  {
    disableCommand(ID_FILE_HINT);
  }
  else
  {
    enableCommand(ID_FILE_HINT);
  }

  // Show new game?
  if (doc->IsRunning() )
  {
    disableCommand(ID_FILE_NEW);
  }
  else if ( doc->QueryServer() ||
            (!doc->IsRemote(Rot)&&!doc->IsRemote(Gelb) ) )
  {
    enableCommand(ID_FILE_NEW);
  }
  else if ( !doc->QueryServer() &&
            ( (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE &&
               mInput->QueryStatus(0)>0) ||
              (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE &&
               mInput->QueryStatus(1)>0)
            )
          )
  {
    disableCommand(ID_FILE_NEW);
  }
  else
  {
    enableCommand(ID_FILE_NEW);
  }

  // Show message
  if ( (mInput->QueryType(0)==KG_INPUTTYPE_REMOTE &&
        mInput->QueryStatus(0)>0)   ||
       (mInput->QueryType(1)==KG_INPUTTYPE_REMOTE &&
        mInput->QueryStatus(1)>0) )
  {
    enableCommand(ID_FILE_MESSAGE);
  }
  else
  {
    disableCommand(ID_FILE_MESSAGE);
  }

  // Show game end?
  if (doc->IsRunning())
  {
    enableCommand(ID_FILE_CLOSE);
  }
  else
  {
    disableCommand(ID_FILE_CLOSE);
  }
}

void Kwin4App::slotEditToShow()
{

  // Show undo?
  if (!doc->IsRunning() ||
      doc->IsRemote(Gelb) || doc->IsRemote(Rot)
      ||!doc->IsUser(doc->QueryCurrentPlayer()) )
  {
    disableCommand(ID_EDIT_UNDO);
  }
  else if (doc->QueryHistoryCnt()==0)
  {
    disableCommand(ID_EDIT_UNDO);
  }
  else if (doc->QueryCurrentMove()<2 &&
            ( (doc->QueryCurrentPlayer()==Gelb && !doc->IsUser(Rot) )
              || (doc->QueryCurrentPlayer()==Rot && !doc->IsUser(Gelb) )
            ))
  {
    disableCommand(ID_EDIT_UNDO);
  }
  else
  {
    enableCommand(ID_EDIT_UNDO);
  }

  // Show redo
  if (!doc->IsRunning() ||
      doc->IsRemote(Gelb) || doc->IsRemote(Rot)
      ||!doc->IsUser(doc->QueryCurrentPlayer()) )
  {
    disableCommand(ID_EDIT_REDO);
  }
  else if (doc->QueryHistoryCnt()==doc->QueryMaxMove())
  {
    disableCommand(ID_EDIT_REDO);
  }
  else
  {
    enableCommand(ID_EDIT_REDO);
  }

}
void Kwin4App::slotOptionsToShow()
{
  // Network
  if (doc->QueryServer())
  {
     checkCommand(ID_OPTIONS_NETWORK);
  }
  else
  {
     uncheckCommand(ID_OPTIONS_NETWORK);
  }
  if (doc->QueryBlink())
  {
     checkCommand(ID_OPTIONS_ANIMATIONS);
  }
  else
  {
     uncheckCommand(ID_OPTIONS_ANIMATIONS);
  }

}
void Kwin4App::slotStartcolorToShow()
{
  if (doc->QueryPlayerColour(0)==Gelb)
  {
     checkCommand(ID_STARTCOLOUR_YELLOW );
     uncheckCommand(ID_STARTCOLOUR_RED );
  }
  else
  {
    uncheckCommand(ID_STARTCOLOUR_YELLOW );
    checkCommand(ID_STARTCOLOUR_RED );
  }
  if (doc->IsRunning())
  {
    disableCommand(ID_STARTCOLOUR_YELLOW);
    disableCommand(ID_STARTCOLOUR_RED);
  }
  else
  {
    enableCommand(ID_STARTCOLOUR_YELLOW);
    enableCommand(ID_STARTCOLOUR_RED);
  }

}
void Kwin4App::slotRedToShow()
{
  uncheckCommand(ID_RED_PLAYER);
  uncheckCommand(ID_RED_COMPUTER);
  uncheckCommand(ID_RED_REMOTE);
  if (doc->IsUser(Rot)) checkCommand(ID_RED_PLAYER);
  else if (doc->IsComputer(Rot)) checkCommand(ID_RED_COMPUTER);
  else checkCommand(ID_RED_REMOTE);

  if (doc->IsRunning() && doc->QueryCurrentPlayer()==Rot)
  {
    disableCommand(ID_RED_PLAYER);
    disableCommand(ID_RED_REMOTE);
    disableCommand(ID_RED_COMPUTER);
  }
  else
  {
    enableCommand(ID_RED_PLAYER);
    enableCommand(ID_RED_REMOTE);
    enableCommand(ID_RED_COMPUTER);
  }
  if (doc->IsRemote(Gelb))
  {
    disableCommand(ID_RED_REMOTE);
  }
}
void Kwin4App::slotYellowToShow()
{
  uncheckCommand(ID_YELLOW_PLAYER);
  uncheckCommand(ID_YELLOW_COMPUTER);
  uncheckCommand(ID_YELLOW_REMOTE);
  if (doc->IsUser(Gelb)) checkCommand(ID_YELLOW_PLAYER);
  else if (doc->IsComputer(Gelb)) checkCommand(ID_YELLOW_COMPUTER);
  else checkCommand(ID_YELLOW_REMOTE);

  if (doc->IsRunning() && doc->QueryCurrentPlayer()==Gelb)
  {
    disableCommand(ID_YELLOW_PLAYER);
    disableCommand(ID_YELLOW_REMOTE);
    disableCommand(ID_YELLOW_COMPUTER);
  }
  else
  {
    enableCommand(ID_YELLOW_PLAYER);
    enableCommand(ID_YELLOW_REMOTE);
    enableCommand(ID_YELLOW_COMPUTER);
  }
  if (doc->IsRemote(Rot))
  {
    disableCommand(ID_YELLOW_REMOTE);
  }
}

void Kwin4App::slotLevelToShow()
{
  int i;
  for (i=0;i<10;i++)
  {
    uncheckCommand(ID_LEVEL_1+i);
  }
  checkCommand(ID_LEVEL_1-1+doc->QueryLevel());
}

void Kwin4App::slotHelpAbout()
{
  slotStatusMsg(i18n("About Kwin4..."));

  // kwin4AboutDlg *dlg=new kwin4AboutDlg(&doc->m_PixWin4About,this);
  aboutDlg *dlg=new aboutDlg(this);
  dlg->SetPixmap(doc->m_PixWin4About);
  dlg->exec();
  slotStatusMsg(i18n("Ready"));
}

void Kwin4App::SetGrafix(QString s)
{
  mGrafix=s;
}
#include "kwin4.moc"
