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

#include <stdlib.h>
#include <time.h>

// include files for Qt
#include <qdir.h>
#include <qfileinfo.h>
#include <qwidget.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <krandomsequence.h>
#include <qcanvas.h>

// application specific includes
#include "kspritecache.h"
#include "kwin4doc.h"
#include "kwin4.h"
#include "kwin4view.h"
#include "geom.h"
#include "scorewidget.h"
#include "statuswidget.h"

#include <kgameio.h>


Kwin4Doc::Kwin4Doc(QWidget *parent, const char *) : KGame(1234,parent)
{
  mHintProcess=0;
  mLastPlayer=0;
  pView=0;

  connect(this,SIGNAL(signalPropertyChanged(KGamePropertyBase *,KGame *)),
          this,SLOT(slotPropertyChanged(KGamePropertyBase *,KGame *)));

  dataHandler()->Debug();
  kdDebug() << "Property 7 policy=" << dataHandler()->find(7)->policy() << endl; 
  kdDebug() << " KWIN4DOC: setPOLICY " << endl;
  setPolicy(KGame::PolicyDirty,true);

  kdDebug() << "Property 7 policy=" << dataHandler()->find(7)->policy() << endl; 

  // Game design
  setMaxPlayers(2);
  setMinPlayers(2);

  // Game initialization
  mField.resize(42);
  // The field array needs not be updated as any move will change it
  // Careful only in new ResetGame! Maybe unlocal it there!
  mField.setPolicy(KGamePropertyBase::PolicyLocal);  
  mField.registerData(dataHandler());

  mFieldFilled.resize(7);
  // old fields=new FARBE[42];
  //field_filled=new int[7];
  mHistory.resize(43);
  mHistory.registerData(dataHandler(),KGamePropertyBase::PolicyLocal);
  //history=new int[43];


  mAmzug.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mAmzug"));
  mCurrentMove.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mCurrentMove"));
  mMaxMove.registerData(4000,dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mMaxMove"));
  mFieldFilled.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mFieldFilled"));
  mHistoryCnt.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mHistoryCnt"));
  mLastColumn.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mLastColumn"));
  mLastHint.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mLastHint"));
  mLastColour.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mLastColour"));
  mLevel.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mLevel"));
  mLevel.setPolicy(KGamePropertyBase::PolicyLocal);  
  mScore.registerData(dataHandler(),KGamePropertyBase::PolicyUndefined,QString("mScore"));
  mScore.setPolicy(KGamePropertyBase::PolicyLocal);  



  // game startup parameter
  mStartPlayer=Gelb;
  mStartPlayer.registerData(dataHandler());
  SetCurrentPlayer((FARBE)mStartPlayer.value()); 
  kdDebug() << "amZug policy=" << mAmzug.policy() << endl;

  mPlayedBy[Gelb]=KGameIO::MouseIO;
  mPlayedBy[Rot]=KGameIO::MouseIO;

  // calculation
  SetLevel(3);

  // last in init
  ResetGame();
  
  setGameStatus(Intro);
  kdDebug() << "GameStatus set to Intro->"<<gameStatus() << endl;


  // Create a new player
  //connect(this,SIGNAL(signalCreatePlayer(KPlayer *&,int,int,bool,KGame *)),
  //       this,SLOT(slotCreatePlayer(Kwin4Player *&,int,int,bool,Kwin4Doc *)));
  //connect(this,SIGNAL(signalCreatePlayer(KPlayer& *,int,int,bool,KGame *)),
  //       this,SLOT(slotCreatePlayer(KPlayer& *,int,int,bool,KGame *)));

  // Listen to network
  connect(this,SIGNAL(signalMessageUpdate(int,Q_UINT32,Q_UINT32)),
         this,SLOT(slotMessageUpdate(int, Q_UINT32,Q_UINT32)));

  // Listen to network
  connect(this,SIGNAL(signalClientJoinedGame(Q_UINT32,KGame *)),
         this,SLOT(slotClientConnected(Q_UINT32, KGame *)));

  connect(this,SIGNAL(signalGameOver(int, KPlayer *,KGame *)),
         this,SLOT(slotGameOver(int, KPlayer *,KGame *)));

  // Change global KGame policy
  //dataHandler()->setPolicy(KGamePropertyBase::PolicyDirty,false);
  dataHandler()->Debug();
}


void Kwin4Doc::initPlayers()
{

  kdDebug() << " Connecting properties to widgets" << endl;


  // Player initialization
  // First create the two players 
  kdDebug() << "initPlayers" <<endl;
  Kwin4Player *yellow=0;
  yellow=(Kwin4Player *)createPlayer(1, mPlayedBy[Gelb], false);
  //slotCreatePlayer(yellow, 1, mPlayedBy[Gelb], false, this);
  kdDebug() << "Gelb vor sets" << yellow<< endl;
  yellow->setUserId(Gelb);  // Color id's    
  yellow->setName(i18n("Player 1"));

  addPlayer(yellow);
  setPlayedBy(Gelb,mPlayedBy[Gelb]);
  kdDebug() << "Gelb=" << yellow<< endl;

  Kwin4Player *red=0;
  // slotCreatePlayer(red, 1, mPlayedBy[Rot], false, this);
  red=(Kwin4Player *)createPlayer(1, mPlayedBy[Rot], false);
  red->setName(i18n("Player 2"));
  red->setUserId(Rot);

  addPlayer(red);
  setPlayedBy(Rot,mPlayedBy[Rot]);
  kdDebug() << "Rot" << red << endl;
  
  // last in init
  ResetStat();
  kdDebug() << "init player done"<<endl;




}

Kwin4Doc::~Kwin4Doc()
{
  if (mHintProcess) delete mHintProcess;
  // delete fields;
  //delete field_filled;
  //delete history;
}

void Kwin4Doc::setView(Kwin4View *view)
{
  pView=view;
}

void Kwin4Doc::setAbsFilePath(const QString &filename)
{
  absFilePath=filename;
}

const QString &Kwin4Doc::getAbsFilePath() const
{
  return absFilePath;
}

void Kwin4Doc::setTitle(const QString & /*_t*/)
{

//  title=_t;
  title="";
}

const QString &Kwin4Doc::getTitle() const
{
  return title;
}

void Kwin4Doc::slotUpdateAllViews(Kwin4View *sender)
{
  if(pView)
  {
      if(pView!=sender) pView->repaint();
  }
}

bool Kwin4Doc::newDocument(KConfig * /*config*/,QString path)
{
  int res;
  absFilePath=QDir::homeDirPath();
  if (global_debug>1) kdDebug() << "path=" << path << endl;
  return true;
}

bool Kwin4Doc::openDocument(const QString &filename, const char * /*format*/ /*=0*/)
{
  QFileInfo fileInfo(filename);
  title=fileInfo.fileName();
  absFilePath=fileInfo.absFilePath();	
  return true;
}

bool Kwin4Doc::saveDocument(const QString & /*filename*/, const char * /*format*/ /*=0*/)
{
  return true;
}




/** Returns colour */
FARBE Kwin4Doc::QueryColour(int x,int y){
  return (FARBE)mField.at(x+y*geom.field_mx);
}
/** Set the colour */
void Kwin4Doc::SetColour(int x,int y,FARBE c){
  if (x<0 || x>=geom.field_mx || y<0 || y>=geom.field_my)
  {
    kdDebug() << "ERROR: SetColour auf falsche Poition " << x << " " << y << endl;
    return ;
  }
  //kdDebug() << "mField["<<x+y*geom.field_mx<<"="<<c<<endl;
  mField.setAt(x+y*geom.field_mx,c);
}

/** Reset the whole game */
void Kwin4Doc::ResetGame(){
  int x,y;

  kdDebug() << "ResetGame" << endl;

  for (x=0;x<geom.field_mx;x++)
  {
    for (y=geom.field_my-1;y>=0;y--)
    {
      SetColour(x,y,Niemand);
    }
  }
  mFieldFilled.fill(0);

  mHistoryCnt=0;
  mCurrentMove=0;
  mMaxMove=0;
  mLastColumn=-1;
  mLastColour=Niemand;
  SetScore(0);
  setGameStatus(Pause);
  mLastHint=-1;
}

/** Start a new game */
void Kwin4Doc::StartGame(){
  mGameOverStatus=0;
  setGameStatus(Run);
  winc=Niemand;
  // Who starts?
  SetCurrentPlayer((FARBE)mStartPlayer.value()); 
  kdDebug() << "StartGame::startplayer="<<mStartPlayer.value()<<endl;
  kdDebug() << "StartGame::currentplayer="<<QueryCurrentPlayer()<<endl;
  getPlayer(QueryCurrentPlayer())->setTurn(true,true);
  pView->initView(false);

}

/** End a game */
void Kwin4Doc::EndGame(TABLE mode)
{
  setGameStatus(End);
  pView->clearError();
  pView->EndGame();
  Kwin4Player *yellow=getPlayer(Gelb);
  Kwin4Player *red=getPlayer(Rot);
  

  switch(mode)
  {
    case TWin:  yellow->incWin();
                red->incLost();
    break;
    case TLost: yellow->incLost();
                red->incWin();
    break;
    case TRemis: yellow->incRemis();
                 red->incRemis();
    break;
    default:
       // Only break if moves have been made
       if (mMaxMove>0)
       {
          yellow->incBrk();
          red->incBrk();
       }
    break;
  }

  // switch start player
}

/** Is a game running */
bool Kwin4Doc::IsRunning(){
  return gameStatus()==Run;
}
/** Is the introduction on */
bool Kwin4Doc::IsIntro(){
  return gameStatus()==Intro;
}


void Kwin4Doc::moveDone(QCanvasItem *item,int )
{
  kdDebug() << "########################## MOVE DONE ################# " << endl;

  playerInputFinished(getPlayer(QueryCurrentPlayer()));

  pView->clearError();

  KSprite *sprite=(KSprite *)item;
  sprite->deleteNotify();

  emit signalNextPlayer();
  kdDebug() << "signalNextPlayer" << endl;
}



/** Make a game move */
// Play a move
// mode=0 normal move, =1: redo move
MOVESTATUS Kwin4Doc::MakeMove(int x,int mode){
  int y;

  if (x<0 || x>=geom.field_mx)
  {
    kdDebug() << "ERROR: MakeMove auf falsche Position " << x << endl;
    return GNotAllowed;
  }

  //y=field_filled[x];
  y=mFieldFilled.at(x);

  if (y>=geom.field_my)
  {
    return GIllMove;  // no space left in column
  }

  if (mLastHint>=0)
  {
    int hy;
    //hy=field_filled[mLastHint];
    hy=mFieldFilled.at(mLastHint);
    SetColour(mLastHint,hy,Niemand);
    mLastHint=-1;
  }
  if (mode==Tip)
  {
    mLastHint=x;
    SetColour(x,y,Tip);
    return GTip ;  // no real move
  }

  mFieldFilled.setAt(x,mFieldFilled.at(x)+1);
  // field_filled[x]++;
  SetColour(x,y,QueryCurrentPlayer());
  
  mHistory.setAt(QueryHistoryCnt(),x);
  mHistoryCnt=mHistoryCnt.value()+1;

  mLastColour=QueryCurrentPlayer();
  if (QueryCurrentPlayer()==Gelb) SetCurrentPlayer(Rot);
  else SetCurrentPlayer(Gelb);

  mCurrentMove=mCurrentMove.value()+1;

  // only if a real move isdone the maxmove is raised
  if (mode==0) mMaxMove=mCurrentMove.value();
  mLastColumn=x;

  pView->setArrow(x,mLastColour);
  // animation onyl if no redo
  pView->setPiece(x,y,mLastColour,mCurrentMove,mode==1?false:true);
  pView->setHint(0,0,false);
 

  /*
  res=CheckGameOver(x,mLastColour);
  if (res==1)
  {
    if (mLastColour==Gelb) return GYellowWin;
    else return GRedWin;
  }
  else if (res==-1) return GRemis;
  */
  return GNormal;
}


/** Undo a move */
bool Kwin4Doc::UndoMove(){
  int x,y;
  if (QueryHistoryCnt()<1) return false;
  if (mLastHint>=0)
  {
    int hy;
    //hy=field_filled[mLastHint];
    hy=mFieldFilled.at(mLastHint);
    SetColour(mLastHint,hy,Niemand);
    mLastHint=-1;
  }
  kdDebug() << "Undo no="<<mHistoryCnt.value() << endl;
  mHistoryCnt=mHistoryCnt.value()-1;
  x=mHistory.at(QueryHistoryCnt());
  mFieldFilled.setAt(x,mFieldFilled.at(x)-1);
  //field_filled[x]--;
  //y=field_filled[x];
  y=mFieldFilled.at(x);
  kdDebug() << "Undo x="<<x << " y=" <<y << endl;
  SetColour(x,y,Niemand);
  mLastColour=QueryCurrentPlayer();
  if (QueryCurrentPlayer()==Gelb) SetCurrentPlayer(Rot);
  else SetCurrentPlayer(Gelb);
  mCurrentMove=mCurrentMove.value()-1;


  // sprite no, arrow pos, arrow color, enable
  int oldx=-1;
  if (QueryHistoryCnt()>0) oldx=mHistory.at(QueryHistoryCnt()-1);
  pView->setSprite(mCurrentMove+1,oldx,QueryHistoryCnt()>0?mLastColour.value():0,false);
  pView->setHint(0,0,false);

  if (QueryHistoryCnt()>0) mLastColumn=mHistory.at(QueryHistoryCnt()-1);
  else mLastColumn=-1;

  SetScore(0);

  return true;
}

/** Redo a move */
bool Kwin4Doc::RedoMove(){
  int x;
  kdDebug() << "mMaxMove=" << mMaxMove.value() << " historycnt=" << QueryHistoryCnt() << endl;
  if (QueryHistoryCnt()>=mMaxMove) return false;
  x=mHistory.at(QueryHistoryCnt());
  kdDebug() << "Redo x=" << x << endl;
  MakeMove(x,1);
  SetScore(0);
  pView->setHint(0,0,false);
  return true;
}

/** Set the name of col */
void Kwin4Doc::SetName(FARBE i,QString n){
  getPlayer(i)->setName(n);
}
/** Query the name of i */
QString Kwin4Doc::QueryName(FARBE i){
  return getPlayer(i)->name();
}

/**  */
int Kwin4Doc::QueryStat(FARBE i,TABLE mode){
  int r;

  Kwin4Player *player=getPlayer(i);
  r=0;
  switch(mode)
  {
    case TWin: r=player->win();
    break;
    case TRemis: r=player->remis();
    break;
    case TLost: r=player->lost();
    break;
    case TBrk: r=player->brk();
    break;
    case TSum:  r=player->win()+player->remis()+player->lost();
    default:
      break;
  }
  return r;
}

/** Query the colour of player i */
FARBE Kwin4Doc::QueryPlayerColour(int player){
  if (player==0) return (FARBE)mStartPlayer.value();
  if (mStartPlayer.value()==Gelb) return Rot;
  else return Gelb;
}


/**  */
int Kwin4Doc::CheckGameOver(int x, FARBE col){
  int y,xx,i;
  int flag;
  FARBE c;

  // Check dy
  flag=0;
  for (i=0;i<4;i++)
  {
    //y=field_filled[x]-1-i;
    y=mFieldFilled.at(x)-1-i;
    if (y>=0)
    {
       c=QueryColour(x,y);
       if (c==col) flag++;
    }
  }
  if (flag>=4 /*&& doBlink*/)
  {
    // Store win fields
    for (i=0;i<4;i++)
    {
      y=mFieldFilled.at(x)-1-i;
      pView->drawStar(x,y,i);
      winc=QueryColour(x,y);
    }
    return 1;
  }
  else if (flag>=4) return 1;

  // Check dx
  //y=field_filled[x]-1;
  y=mFieldFilled.at(x)-1;
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
     xx=x+i;
     if (xx>=0 && xx<geom.field_mx)
     {
       c=QueryColour(xx,y);
       if (c==col) flag++;
       else flag=0;
     }
  }
  if (flag>=4 /*&& doBlink*/)
  {
    // Store win fields
    //y=field_filled[x]-1;
    y=mFieldFilled.at(x)-1;
    winc=QueryColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<geom.field_mx)
      {
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,i);
        cnt++;
      }
      else break;
    }
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<geom.field_mx)
      {
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,i);
        cnt++;
      }
      else break;
    }
    return 1;
  }
  else if (flag>=4) return 1;


  // Check dy+
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
    xx=x+i;
    if (xx>=0 && xx<geom.field_mx)
    {
      //y=field_filled[x]-1-i;
      y=mFieldFilled.at(x)-1-i;
      if (y>=0 && y<geom.field_my)
      {
        c=QueryColour(xx,y);
        if (c==col) flag++;
        else flag=0;
      }
    }
  }
  if (flag>=4 /*&& doBlink*/) 
  {
    // Store win fields
    //y=field_filled[x]-1;
    y=mFieldFilled.at(x)-1;
    winc=QueryColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<geom.field_mx)
      {
        //y=field_filled[x]-1-i;
        y=mFieldFilled.at(x)-1-i;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,i);
        cnt++;
      }
      else break;
    }
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<geom.field_mx)
      {
        //y=field_filled[x]-1-i;
        y=mFieldFilled.at(x)-1-i;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,i);
        cnt++;
      }
      else break;
    }
    return 1;
  }
  else if (flag>=4) return 1;


  // Check dy-
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
    xx=x+i;
    if (xx>=0 && xx<geom.field_mx)
    {
      //y=field_filled[x]-1+i;
      y=mFieldFilled.at(x)-1+i;
      if (y>=0 && y<geom.field_my)
      {
        c=QueryColour(xx,y);
        if (c==col) flag++;
        else flag=0;
      }
    }
  }
  if (flag>=4 /*&& doBlink*/) 
  {
    printf("DX +\n");
    // Store win fields
    //y=field_filled[x]-1;
    y=mFieldFilled.at(x)-1;
    winc=QueryColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<geom.field_mx)
      {
        y=mFieldFilled.at(x)-1+i;
        //y=field_filled[x]-1+i;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,i);
        cnt++;
      }
      else break;
    }
    printf("Found + cnt=%d\n",cnt);
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<geom.field_mx)
      {
        //y=field_filled[x]-1+i;
        y=mFieldFilled.at(x)-1+i;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,i);
        cnt++;
      }
      else break;
    }
    printf("all cnt=%d\n",cnt);
    return 1;
  }
  else if (flag>=4) return 1;

  if (mCurrentMove>=42) return -1;

  return 0;
}
/** Reset the stats */
void Kwin4Doc::ResetStat(){
  kdDebug() << "reset stats"<<endl;
  // Reset data
  Kwin4Player *p=getPlayer(Gelb);
  p->resetStats();

  p=getPlayer(Rot);
  p->resetStats();
}

/** Set computer score */
void Kwin4Doc::SetScore(long i){
  mScore.setValue(i);
}

/** Query computer score */
long Kwin4Doc::QueryScore(){
  return mScore.value();
}


/** Height of a column */
int Kwin4Doc::QueryHeight(int x){
  if (x<0 || x>=geom.field_mx)
  {
    kdDebug() << "ERROR: Query Height for wrong x " << x << endl;
    return 0;
  }
  //return field_filled[x];
  return mFieldFilled.at(x);
}

/**  */
int Kwin4Doc::QueryLastHint(){
  return mLastHint;
}

/** read config file */
void Kwin4Doc::ReadConfig(KConfig *config)
{
  config->setGroup("Parameter");
  mHost=config->readEntry("host","localhost");
  mPort=(unsigned short)config->readNumEntry("port",7442);
  QString name;
  name=config->readEntry("Name1",QueryName(Gelb));
  SetName(Gelb,name);
  name=config->readEntry("Name2",QueryName(Rot));
  SetName(Rot,name);

  SetLevel(config->readNumEntry("Level",3));
  KGameIO::IOMode mode;
  mode=(KGameIO::IOMode)config->readNumEntry("Input1",mPlayedBy[Gelb]);
  if (mode==0) mode=KGameIO::MouseIO; // No saved network
  setPlayedBy(Gelb,mode);
  mode=(KGameIO::IOMode)config->readNumEntry("Input2",mPlayedBy[Rot]);
  if (mode==0) mode=KGameIO::MouseIO; // No saved network
  setPlayedBy(Rot,mode);
  
  FARBE col;
  col=(FARBE)config->readNumEntry("Colour1",(int)Gelb);
  // Check against old or strange config files
  if (col!=Rot && col!=Gelb) col=Gelb;
  mStartPlayer.setValue(col);
  kdDebug() << "StartPlayer=" << mStartPlayer.value() << endl;

  Kwin4Player *player;
  player=getPlayer(Gelb);
  config->setGroup("YellowPlayer");
  player->readConfig(config);

  player=getPlayer(Rot);
  config->setGroup("RedPlayer");
  player->readConfig(config);

  kdDebug() << "ReadCOnfig done"<<endl;
}

/** write config file */
void Kwin4Doc::WriteConfig(KConfig *config)
{
  config->setGroup("Parameter");
  config->writeEntry("host",mHost);
  config->writeEntry("port",(int)mPort);
  config->writeEntry("Name1",QueryName(Gelb));
  config->writeEntry("Name2",QueryName(Rot));
  config->writeEntry("Level",QueryLevel());
  // Do not save remote !
  config->writeEntry("Input1",(int)mPlayedBy[Gelb]);
  config->writeEntry("Input2",(int)mPlayedBy[Rot]);
  config->writeEntry("Colour1",mStartPlayer.value());


  Kwin4Player *player;
  player=getPlayer(Gelb);
  config->setGroup("YellowPlayer");
  player->writeConfig(config);

  player=getPlayer(Rot);
  config->setGroup("RedPlayer");
  player->writeConfig(config);

  config->sync();
}

/** Returns the current player, resp amzug */
FARBE Kwin4Doc::QueryCurrentPlayer(){
  return (FARBE)mAmzug.value();
}
void Kwin4Doc::SetCurrentPlayer(FARBE i)
{
  mAmzug.setValue(i);
}
/** Query Computer Level */
int Kwin4Doc::QueryLevel(){
  return mLevel.value();
}
/** Set Computer level */
void Kwin4Doc::SetLevel(int i){
  mLevel.setValue(i);
}

  /** Swtich the starting player and return the new started */
FARBE Kwin4Doc::SwitchStartPlayer()
{
  int col=mStartPlayer.value();
  if (col==Gelb) mStartPlayer.setValue(Rot);
  else mStartPlayer.setValue(Gelb);
  return (FARBE)mStartPlayer.value();
}
int Kwin4Doc::QueryLastcolumn()
{
  return mLastColumn;
}
FARBE Kwin4Doc::QueryLastcolour()
{
  return (FARBE)(mLastColour.value());
}

int Kwin4Doc::QueryCurrentMove()
{
  return mCurrentMove;
}
void Kwin4Doc::SetCurrentMove(int i)
{
  mCurrentMove=i;
}

int Kwin4Doc::QueryMaxMove()
{
  return mMaxMove;
}

int Kwin4Doc::QueryHistoryCnt()
{
  return mHistoryCnt;
}
/** Random number between 0..max-1 */
int Kwin4Doc::Random(int max)
{
  return (int)(random()->getLong(max));
}

/** Return the name of the computer player process */
QString Kwin4Doc::QueryProcessName()
{
  // First try a local dir override
  QDir dir;
  QString filename=dir.path()+QString("/kwin4/kwin4proc");
  QFile flocal(filename);
  if (flocal.exists())
  {
    kdDebug() << " Found local process " << filename << endl;
    return filename;
  }
  QString path=kapp->dirs()->findExe("kwin4proc");
  if (!path.isNull())
  {
    kdDebug() << " Found system process " << path << endl;
    return path;
  }
  QString empty;
  kdError() <<  "Could not locate the computer player" << endl;
  return empty;
}

unsigned short Kwin4Doc::QueryPort()
{
  return mPort;
}
QString Kwin4Doc::QueryHost()
{
  return mHost;
}
void Kwin4Doc::SetPort(unsigned short port)
{
  mPort=port;
}

void Kwin4Doc::SetHost(QString host)
{
  mHost=host;
}

void Kwin4Doc::slotMessageUpdate(int id,Q_UINT32 sender,Q_UINT32 recv)
{
//  kdDebug() << "MSG: id=" << id << " sender=" << sender << " receiver="<<recv<< endl;
}

// KGame Player handling
KPlayer *Kwin4Doc::createPlayer(int rtti,int io,bool isvirtual)
//void Kwin4Doc::slotCreatePlayer(Kwin4Player *&player,int rtti,int io,bool isvirtual,Kwin4Doc * /*game*/)
{
  KPlayer *player=new Kwin4Player;
  //player=(KPlayer *)new Kwin4Player;
  if (!isvirtual)
  {
    createIO(player,(KGameIO::IOMode)io);
  }
  kdDebug() << "Kwin4Doc::slotCreatePlayer should create player " << player << " with rtti=" <<rtti << " virtual="<<isvirtual << endl;
  connect(player,SIGNAL(signalPropertyChanged(KGamePropertyBase *,KPlayer *)),
          this,SLOT(slotPlayerPropertyChanged(KGamePropertyBase *,KPlayer *)));
  ((Kwin4Player *)player)->setWidget(pView->statusWidget());
  return player;
}

bool Kwin4Doc::playerInput(QDataStream &msg,KPlayer *player)
{
  Kwin4Player *p=(Kwin4Player *)player;
  Q_INT32 move,pl;
  msg >> pl >> move;
  kdDebug()  << " Player " << pl << " moves to " << move << "***************" << endl;
  Move(move,pl);
  return false; // Stop game sequence
}


/** PErforms a game move */
bool Kwin4Doc::Move(int x,int id)
{
  MOVESTATUS res;
  int hintx,lastx;

  kdDebug() <<" Kwin4Doc::Move("<<x<<","<<id<<")"<<endl;

  hintx=QueryLastHint();
  lastx=QueryLastcolumn();
  //int isremote=IsRemote(QueryCurrentPlayer());
  res=MakeMove(x,0);
  /*
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
  */

  int y;
  // Clear hint as well
  if (hintx>=0)
  {
    y=QueryHeight(hintx)+1;
    //UpdateViews(UPDATE_XY,hintx,y);
  }

  y=QueryHeight(x);
  //UpdateViews(UPDATE_XY|UPDATE_ARROW|UPDATE_STATUS,x,y);
  //UpdateViews(UPDATE_ARROW,lastx,0);
  emit signalMoveDone(x,y);
  //return NextMove(res);
  return true;
}

int Kwin4Doc::checkGameOver(KPlayer *)
{
  // -1: remis, 1:won, 0: continue
  return CheckGameOver(QueryLastcolumn(),QueryLastcolour());
}

KGameIO::IOMode Kwin4Doc::playedBy(int col)
{
  return mPlayedBy[col];
}
void Kwin4Doc::setPlayedBy(int col,KGameIO::IOMode io)
{
  kdDebug() << "  Kwin4Doc::setPlayedBy(int "<<col<<",KGameIO::IOMode "<<io<<")" << endl;

  Kwin4Player *player=getPlayer((FARBE)col);

  if (mPlayedBy[col]!=io)
  {
    mPlayedBy[col]=io;
    player->removeGameIO(0); // remove all IO's
    createIO(player,io);
  }
  if (io==0) player->setNetworkPriority(0);
  else if (io==KGameIO::ProcessIO) player->setNetworkPriority(10);
  else  player->setNetworkPriority(50);
}

void Kwin4Doc::createIO(KPlayer *player,KGameIO::IOMode io)
{
  kdDebug() << " Kwin4Doc::createIO(KPlayer *player,KGameIO::IOMode io) " << endl;


  if (io&KGameIO::MouseIO)
  {
    KGameMouseIO *input;
    kdDebug() << "*******************Creating MOUSE IO to "<<pView<< endl;
    // We want the player to work over mouse
    input=new KGameMouseIO(pView);
    kdDebug() << "MOUSE IO added " << endl;
    // Connect mouse input to a function to process the actual input
    connect(input,SIGNAL(signalMouseEvent(KGameIO *,QDataStream &,QMouseEvent *,bool *)),
            pView,SLOT(slotMouseInput(KGameIO *,QDataStream &,QMouseEvent *,bool *)));
    player->addGameIO(input);
  }
  else if (io&KGameIO::ProcessIO)
  {
    QString file=QueryProcessName();
    kdDebug() << "Creating PROCESS IO " << file  << endl;

    KGameProcessIO *input;
    // We want a computer player
    input=new KGameProcessIO(file);
    // Connect computer player to the setTurn
    connect(input,SIGNAL(signalPrepareTurn(QDataStream &,bool,KGameIO *,bool *)),
            this,SLOT(slotPrepareTurn(QDataStream &,bool,KGameIO *,bool *)));

    connect(input,SIGNAL(signalProcessQuery(QDataStream &,KGameProcessIO *)),
            this,SLOT(slotProcessQuery(QDataStream &,KGameProcessIO *)));
    player->addGameIO(input);
  }

/*
    if (io&KGameIO::KeyIO)
    {
      kdDebug() << "Creating KEYBOARD IO " << endl;
      // We want the player to work over keyboard
      input2=(KGameIO *)new KGameKeyIO(this);
      // Connect keys input to a function to process the actual input
      connect((KGameKeyIO *)input2,SIGNAL(signalKeyEvent(KGameIO *,QDataStream &,QKeyEvent *,bool &)),
              this,SLOT(slotKeyInput(KGameIO *,QDataStream &,QKeyEvent *,bool &)));
    }
    */
  kdDebug() << "Done b" << endl;
}

// This slot is called when a computer move should be generated
void Kwin4Doc::slotPrepareTurn(QDataStream &stream,bool b,KGameIO *input,bool *sendit)
{
  kdDebug() << " @@@@@@@@@@@@@q Kwin4Doc::slotPrepareTurn b="<<b << endl;
  *sendit=false;
  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn()) return ;
  if (!b) return ; // only on setTurn(true)

  Q_INT32 pl;
  kdDebug() << "slotPrepareComputerTurn for player id= " << player->id() << endl;
  pl=player->userId();

  prepareGameMessage(stream,pl);

  *sendit=true;
}

void Kwin4Doc::prepareGameMessage(QDataStream &stream, Q_INT32 pl)
{
  kdDebug() << "          sending col=" << pl << endl;
  stream << pl ;
  // This needs to be the same than the computer player reads!!!!!
  stream << (Q_INT32)QueryCurrentMove();
  stream << (Q_INT32)QueryCurrentPlayer();
  stream << (Q_INT32)QueryPlayerColour(0);
  stream << (Q_INT32)QueryPlayerColour(1);
  stream << (Q_INT32)QueryLevel();
  kdDebug() << "Level=" << QueryLevel() << endl;

  int i,j;
  for (i=0;i<geom.field_my;i++)
  {
    for (j=0;j<geom.field_mx;j++)
    {
       Q_INT8 col;
       col=QueryColour(j,i);
       stream << col;
    }
    kdDebug()
      << QueryColour(0,i)  << " "
      << QueryColour(1,i)  << " "
      << QueryColour(2,i)  << " "
      << QueryColour(3,i)  << " "
      << QueryColour(4,i)  << " "
      << QueryColour(5,i)  << " "
      << QueryColour(6,i)  << endl;
  }
  stream << (Q_INT32)421256;
}

void Kwin4Doc::slotProcessQuery(QDataStream &in,KGameProcessIO *me)
{
  Q_INT8 cid;
  in >> cid;
  switch(cid)
  {
    case 1:  // value
      long value;
      in >> value;
      kdDebug() << "#### Computer thinks value is " << value << endl;
      SetScore(value);
    break;
    default:
      kdError() << "Kwin4Doc::slotProcessQuery: Unknown id " << cid << endl;
    break;
  }
}

void Kwin4Doc::slotClientConnected(Q_UINT32 cid,KGame *)
{
  kdDebug() << "void Kwin4Doc::slotClientConnected id="<<cid << " we=" <<
  gameId() << " we admin=" << isAdmin() << "master)" << isMaster() << endl;

  if (!isAdmin()) return ;

 
  if (playerList()->count()!=2)
  {
    kdError() << "SERIOUS ERROR: We do not have two players...Trying to disconnect."<<endl;
    disconnect();
    return ;
  }

  Kwin4Player *p1=(Kwin4Player *)playerList()->at(0);
  Kwin4Player *p2=(Kwin4Player *)playerList()->at(1);

  // Check for colours, i.e. user ID 
  if (p1->userId()==p2->userId())
  {
    kdDebug() << " User id is the same. Switching remote player's colour" << endl;
    if (p1->isVirtual() && p1->userId()!=Gelb) p1->setUserId(Gelb);
    else if (p1->isVirtual() && p1->userId()==Gelb) p1->setUserId(Rot);
    else if (p2->isVirtual() && p2->userId()!=Gelb) p2->setUserId(Gelb);
    else if (p2->isVirtual() && p2->userId()==Gelb) p2->setUserId(Rot);

    kdDebug() << "Colours: "<<p1->id()<<" =" << p1->userId() << endl;
    kdDebug() << "Colours: "<<p2->id()<<" =" << p2->userId() << endl;
  }

  // Check for myTurn
  if (p1->myTurn()==p2->myTurn())
  {
    if (p1->isVirtual()) p1->setTurn(!p1->myTurn(),true);
    else if (p2->isVirtual()) p2->setTurn(!p2->myTurn(),true);
    kdDebug() << "Turn: "<<p1->id()<<" =" << p1->myTurn() << endl;
    kdDebug() << "Turn: "<<p2->id()<<" =" << p2->myTurn() << endl;
  }

}

Kwin4Player *Kwin4Doc::getPlayer(FARBE col)
{
 Kwin4Player *p;
 for ( p=(Kwin4Player *)playerList()->first(); p!= 0; p=(Kwin4Player *)playerList()->next() )
 {
   if (p->userId()==col) return p;
 }
 kdError() << "SERIOUS ERROR: Cannot find player with colour " << col << ".  CRASH imminent" << endl;
 return 0;

}

void Kwin4Doc::calcHint()
{
  // We allocate the hint process only if it is needed
  if (!mHintProcess)
  {
    QString file=QueryProcessName();
    kdDebug() << "Creating HINT PROCESS IO " << endl;

    // We want a computer player
    mHintProcess=new KGameProcessIO(file);

    connect(mHintProcess,SIGNAL(signalProcessQuery(QDataStream &,KGameProcessIO *)),
            this,SLOT(slotProcessHint(QDataStream &,KGameProcessIO *)));
  }
  Q_INT32 pl;
  QByteArray buffer;
  QDataStream stream(buffer,IO_WriteOnly);
  pl=QueryCurrentPlayer();
  prepareGameMessage(stream,pl);
  mHintProcess->sendMessage(stream,2,0,gameId());
}

void Kwin4Doc::slotProcessHint(QDataStream &in,KGameProcessIO *me)
{
  Q_INT8 cid;
  in >> cid;
  switch(cid)
  {
    case 2:  // Hint
    {
      Q_INT32 pl;
      Q_INT32 move;
      long value;
      in >>  pl >> move  >> value;
      kdDebug() << "#### Computer thinks pl=" << pl << " move =" << move << endl;
      kdDebug() << "#### Computer thinks hint is " << move << " and value is " << value << endl;
      int x=move;
      int y=mFieldFilled.at(x);
      pView->setHint(x,y,true);
    }
    break;
    default:
      kdError() << "Kwin4Doc::slotProcessHint: Unknown id " << cid << endl;
    break;
  }
}
   
void Kwin4Doc::slotPlayerPropertyChanged(KGamePropertyBase *prop,KPlayer *player)
{
  if (!pView) return ;
   if (prop->id()==KGamePropertyBase::IdName)
   {
     kdDebug() << "Player name id=" << player->userId() << " changed to " << player->name()<<endl;
     pView->scoreWidget()->setPlayer(player->name(),player->userId());
   }
}
void Kwin4Doc::slotPropertyChanged(KGamePropertyBase *prop,KGame *)
{
   if (!pView) return ;
   if (prop->id()==mCurrentMove.id())
   {
     pView->scoreWidget()->setMove(mCurrentMove);
   }
   else if (prop->id()==mLevel.id())
   {
     pView->scoreWidget()->setLevel(mLevel);
   }
   else if (prop->id()==mScore.id())
   {
     int sc=mScore/10000;
     if (sc==0 && mScore.value()>0) sc=1;
     else if (sc==0 && mScore.value()<0) sc=-1;
     pView->scoreWidget()->setChance(sc);
   }
   else if (prop->id()==mAmzug.id())
   {
     pView->scoreWidget()->setTurn(mAmzug);
   }
   else if (prop->id()==KGamePropertyBase::IdGameStatus)
   {
   }
}

void Kwin4Doc::slotGameOver(int status, KPlayer * p, KGame * /*me*/)
{
  setGameStatus(End);
  mLastPlayer=p;
  mGameOverStatus=status;
  
}


// Redraw game after load/network
bool Kwin4Doc::loadgame(QDataStream &stream,bool network,bool reset)
{
  pView->initView(false);
  bool res=KGame::loadgame(stream,network,reset);

  kdDebug() << "REDRAW GAME" << endl;
  int cnt=0;
  while(UndoMove())
  {
    cnt++;
  }
  while(cnt>0)
  {
    RedoMove();
    cnt--;
  }

  return res;
}

#include "kwin4doc.moc"
