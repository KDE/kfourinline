/***************************************************************************
                          kwin4doc.cpp  -  Boardgame for KDE
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

#include "kwin4doc.h"

// include files for Qt
#include <qdir.h>
#include <qtimer.h>

// include files for KDE
#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <krandomsequence.h>
#include <kapplication.h>

// application specific includes
#include "kspritecache.h"
#include "kwin4view.h"
#include "scorewidget.h"
#include "prefs.h"
#include "statuswidget.h"

Kwin4Doc::Kwin4Doc(QWidget *parent, const char *) : KGame(1234,parent), pView(0), mHintProcess(0)
{
  connect(this,SIGNAL(signalPropertyChanged(KGamePropertyBase *,KGame *)),
          this,SLOT(slotPropertyChanged(KGamePropertyBase *,KGame *)));

  dataHandler()->Debug();
  //kdDebug(12010) << "Property 7 policy=" << dataHandler()->find(7)->policy() << endl; 
  setPolicy(KGame::PolicyDirty,true);

  //kdDebug(12010) << "Property 7 policy=" << dataHandler()->find(7)->policy() << endl; 

  // Game design
  setMaxPlayers(2);
  setMinPlayers(2);

  // Game initialization
  mField.resize(42);

  // ****************************************
  // NOTE: Do not i18n the strings here. They
  //       are for debugging only
  // ****************************************
  
  // The field array needs not be updated as any move will change it
  // Careful only in new ResetGame! Maybe unlocal it there!
  //  mField.setPolicy(KGamePropertyBase::PolicyLocal);  
  mField.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mField"));

  mFieldFilled.resize(7);
  mHistory.resize(43);
  mHistory.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mHistory"));

  mAmzug.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mAmzug"));
  mCurrentMove.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mCurrentMove"));
  mMaxMove.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mMaxMove"));
  mFieldFilled.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mFieldFilled"));
  mHistoryCnt.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mHistoryCnt"));
  mLastColumn.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mLastColumn"));
  mLastHint.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mLastHint"));
  mLastColour.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mLastColour"));
  mScore.registerData(dataHandler(),KGamePropertyBase::PolicyLocal,QString("mScore"));

  // game startup parameter
  mStartPlayer=Gelb;
  mStartPlayer.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mStartPlayer"));
  SetCurrentPlayer((FARBE)mStartPlayer.value()); 
  if (global_debug>1) kdDebug(12010) << "amZug policy=" << mAmzug.policy() << endl;

  mPlayedBy[Gelb]=KGameIO::MouseIO;
  mPlayedBy[Rot]=KGameIO::MouseIO;

  // last in init
  ResetGame(false);
  
  setGameStatus(Intro);

  // Listen to network
  connect(this,SIGNAL(signalMessageUpdate(int,Q_UINT32,Q_UINT32)),
         this,SLOT(slotMessageUpdate(int, Q_UINT32,Q_UINT32)));
  connect(this,SIGNAL(signalClientJoinedGame(Q_UINT32,KGame *)),
         this,SLOT(slotClientConnected(Q_UINT32, KGame *)));

  // Debug only
  connect(this,SIGNAL(signalGameOver(int, KPlayer *,KGame *)),
         this,SLOT(slotGameOver(int, KPlayer *,KGame *)));

  // Change global KGame policy
  //dataHandler()->setPolicy(KGamePropertyBase::PolicyDirty,false);
  dataHandler()->Debug();
}

/**
 * Player initialization
 */ 
void Kwin4Doc::initPlayers()
{
  // Create yellow 
  Kwin4Player *yellow = (Kwin4Player *)createPlayer(1, mPlayedBy[Gelb], false);
  yellow->setUserId(Gelb);
  yellow->setName(Prefs::name1());
  addPlayer(yellow);
  setPlayedBy(Gelb,mPlayedBy[Gelb]);

  // Create Red
  Kwin4Player *red = (Kwin4Player *)createPlayer(1, mPlayedBy[Rot], false);
  red->setUserId(Rot);
  red->setName(Prefs::name1());
  addPlayer(red);
  setPlayedBy(Rot,mPlayedBy[Rot]);
}

Kwin4Doc::~Kwin4Doc()
{
  WriteConfig(kapp->config());
  if (mHintProcess)
    delete mHintProcess;
}

void Kwin4Doc::setView(Kwin4View *view)
{
  pView=view;
}

/**
 * Returns colour
 */
FARBE Kwin4Doc::QueryColour(int x,int y){
  return (FARBE)mField.at(x+y*FIELD_SIZE_X);
}

/**
 * Set the colour
 */
void Kwin4Doc::SetColour(int x,int y,FARBE c){
  if (x<0 || x>=FIELD_SIZE_X || y<0 || y>=FIELD_SIZE_Y)
  {
    kdDebug(12010) << "ERROR: SetColour auf falsche Poition " << x << " " << y << endl;
    return ;
  }
  //kdDebug(12010) << "SetColor::mField["<<x+y*FIELD_SIZE_X<<"="<<c<<endl;
  mField.setAt(x+y*FIELD_SIZE_X,c);
}

/**
 * Reset the whole game
 */
void Kwin4Doc::ResetGame(bool initview){
  // Reset field
  for (int x=0;x<FIELD_SIZE_X;x++)
  {
    for (int y=FIELD_SIZE_Y-1;y>=0;y--)
      SetColour(x,y,Niemand);
  }
  mFieldFilled.fill(0);

  // Reset game vars
  mHistoryCnt=0;
  mCurrentMove=0;
  mMaxMove=0;
  mLastColumn=-1;
  mLastColour=Niemand;
  SetScore(0);
  mLastHint=-1;

  // Reset the view
  if (initview)
    pView->initView(false);
  
  // Who starts this game
  SetCurrentPlayer((FARBE)mStartPlayer.value()); 
}

/**
 * Set current player to setTurn true
 */
void Kwin4Doc::preparePlayerTurn()
{
  if (global_debug>1)
    kdDebug(12010) << "Setting the current player to turn"<<endl;
  getPlayer(QueryCurrentPlayer())->setTurn(true,true);
}

/**
 * End a game
 */
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

void Kwin4Doc::moveDone(QCanvasItem *item,int )
{
  //kdDebug(12010) << "########################## SPRITE MOVE DONE ################# " << endl;
  //Debug();
  //for (KPlayer* p=playerList()->first(); p!= 0; p=playerList()->next() )
  //{
  //  p->Debug();
  //}

  if (playerCount()>1)
    playerInputFinished(getPlayer(QueryCurrentPlayer()));

  pView->clearError();

  KSprite *sprite=(KSprite *)item;
  sprite->deleteNotify();
}

/**
 * Calcualte the next players turn
 */
KPlayer * Kwin4Doc::nextPlayer(KPlayer *last,bool /*exclusive*/)
{
  if (global_debug>1)
     kdDebug(12010) << k_funcinfo << "nextPlayer last="<<last->id() << " admin=" << isAdmin() <<endl;

  // Should be enough if the admin sets the turn
  if (last->userId()==Gelb)
    SetCurrentPlayer(Rot);
  else
    SetCurrentPlayer(Gelb);
  if (global_debug>1)
    kdDebug(12010) <<" Current set to "<<QueryCurrentPlayer()<<endl;
  if (isAdmin())
    getPlayer(QueryCurrentPlayer())->setTurn(true,true);
  emit signalMoveDone(0,0);
  return getPlayer(QueryCurrentPlayer());
}

/**
 * Make a game move
 * mode=0 normal move, =1: redo move
 */
MOVESTATUS Kwin4Doc::MakeMove(int x,int mode){
  if (x<0 || x>=FIELD_SIZE_X)
  {
    kdDebug(12010) << "ERROR: MakeMove auf falsche Position " << x << endl;
    return GNotAllowed;
  }

  int y=mFieldFilled.at(x);

  if (y>=FIELD_SIZE_Y)
  {
    return GIllMove;  // no space left in column
  }

  if (mLastHint>=0)
  {
    int hy;
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
  SetColour(x,y,QueryCurrentPlayer());
  
  mHistory.setAt(QueryHistoryCnt(),x);
  mHistoryCnt=mHistoryCnt.value()+1;

  mLastColour=QueryCurrentPlayer();
  //if (QueryCurrentPlayer()==Gelb) SetCurrentPlayer(Rot);
  //else SetCurrentPlayer(Gelb);

  mCurrentMove=mCurrentMove.value()+1;

  // only if a real move isdone the maxmove is raised
  if (mode==0) mMaxMove=mCurrentMove.value();
  mLastColumn=x;

  pView->setArrow(x,mLastColour);
  // animation onyl if no redo
  pView->setPiece(x,y,mLastColour,mCurrentMove-1,mode==1?false:true);
  pView->setHint(0,0,false);
 
  return GNormal;
}


/**
 * Undo a move
 */
bool Kwin4Doc::UndoMove(){
  //kdDebug(12010) <<" undo: current player="<<QueryCurrentPlayer() << endl;
  //kdDebug(12010) <<" undo: history="<<QueryHistoryCnt() << endl;
  if (QueryHistoryCnt()<1)
    return false;
  
  if (mLastHint>=0)
  {
    int hy;
    hy=mFieldFilled.at(mLastHint);
    SetColour(mLastHint,hy,Niemand);
    mLastHint=-1;
  }
  // kdDebug(12010) << "Undo no="<<mHistoryCnt.value() << endl;
  mHistoryCnt=mHistoryCnt.value()-1;
  int x=mHistory.at(QueryHistoryCnt());
  mFieldFilled.setAt(x,mFieldFilled.at(x)-1);
  int y=mFieldFilled.at(x);
  // kdDebug(12010) << "Undo x="<<x << " y=" <<y << endl;
  SetColour(x,y,Niemand);
  // We have to remove the piece as well...
  pView->setPiece(x,y,Niemand,mCurrentMove-1,false);

  mLastColour=QueryCurrentPlayer();
  if (QueryCurrentPlayer()==Gelb) SetCurrentPlayer(Rot);
  else SetCurrentPlayer(Gelb);
  mCurrentMove=mCurrentMove.value()-1;

  // sprite no, arrow pos, arrow color, enable
  int oldx=-1;
  if (QueryHistoryCnt()>0) oldx=mHistory.at(QueryHistoryCnt()-1);
  pView->setSprite(mCurrentMove+1,oldx,QueryHistoryCnt()>0?mLastColour.value():0,false);
  pView->setHint(0,0,false);

  if (QueryHistoryCnt()>0)
    mLastColumn=mHistory.at(QueryHistoryCnt()-1);
  else
    mLastColumn=-1;

  SetScore(0);

  return true;
}

/**
 * Redo a move
 */
bool Kwin4Doc::RedoMove(){
  //kdDebug(12010) << "mMaxMove=" << mMaxMove.value() << " historycnt=" << QueryHistoryCnt() << endl;
  if (QueryHistoryCnt()>=mMaxMove)
    return false;
  
  int x=mHistory.at(QueryHistoryCnt());
  //kdDebug(12010) << "Redo x=" << x << endl;
  MakeMove(x,1);
  if (QueryCurrentPlayer()==Gelb)
    SetCurrentPlayer(Rot);
  else
    SetCurrentPlayer(Gelb);
  SetScore(0);
  pView->setHint(0,0,false);
  return true;
}

/**
 * Set the name of col
 */
void Kwin4Doc::SetName(FARBE i, const QString &n){
  getPlayer(i)->setName(n);
}

/**
 * Query the name of i
 */
QString Kwin4Doc::QueryName(FARBE i){
  return getPlayer(i)->name();
}

/**
 * Returns the all time statistics for player i
 */
int Kwin4Doc::QueryStat(FARBE i,TABLE mode){
  Kwin4Player *player=getPlayer(i);
  switch(mode)
  {
    case TWin: return player->win();
    break;
    case TRemis: return player->remis();
    break;
    case TLost: return player->lost();
    break;
    case TBrk: return player->brk();
    break;
    case TSum:  return (player->win()+player->remis()+player->lost());
    default:
      break;
  }
  return 0;
}

/**
 * Query the colour of player i
 */
FARBE Kwin4Doc::QueryPlayerColour(int player){
  if (player==0)
    return (FARBE)mStartPlayer.value();
  
  if (mStartPlayer.value()==Gelb)
    return Rot;
  else
    return Gelb;
}

/**  */
int Kwin4Doc::CheckGameOver(int x, FARBE col){
  int y,i;
  FARBE c;
  int star=1;
  FARBE winc=Niemand;

  // Check vertical up
  int flag=0;
  for (i=0;i<4;i++)
  {
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
      pView->drawStar(x,y,star++);
      winc=QueryColour(x,y);
    }
    return 1;
  }
  else if (flag>=4) return 1;

  int xx;
  // Check horizontal to the right
  y=mFieldFilled.at(x)-1;
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
     xx=x+i;
     if (xx>=0 && xx<FIELD_SIZE_X)
     {
       c=QueryColour(xx,y);
       if (c==col) flag++;
       else flag=0;
     }
  }
  if (flag>=4 /*&& doBlink*/)
  {
    // Store win fields
    y=mFieldFilled.at(x)-1;
    winc=QueryColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,star++);
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
    if (xx>=0 && xx<FIELD_SIZE_X)
    {
      y=mFieldFilled.at(x)-1-i;
      if (y>=0 && y<FIELD_SIZE_Y)
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
    y=mFieldFilled.at(x)-1;
    winc=QueryColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1-i;
        if (y<0) break;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1-i;
        if (y>=FIELD_SIZE_Y) break;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,star++);
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
    if (xx>=0 && xx<FIELD_SIZE_X)
    {
      y=mFieldFilled.at(x)-1+i;
      if (y>=0 && y<FIELD_SIZE_Y)
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
    y=mFieldFilled.at(x)-1;
    winc=QueryColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1+i;
        if (y>=FIELD_SIZE_Y) break;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    //kdDebug(12010) << "Found + cnt=" << cnt <<endl;
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1+i;
        if (y<0) break;
        if (QueryColour(xx,y)!=winc) break;
        pView->drawStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    //kdDebug(12010) << "all cnt=" << cnt<<endl;
    return 1;
  }
  else if (flag>=4) return 1;

  if (mCurrentMove>=42) return -1;

  return 0;
}

/**
 * Reset the stats
 */
void Kwin4Doc::ResetStat(){
  getPlayer(Gelb)->resetStats();
  getPlayer(Rot)->resetStats();
}

/**
 * Set computer score
 */
void Kwin4Doc::SetScore(long i){
  mScore.setValue(i);
}

/**
 * Height of a column
 */
int Kwin4Doc::QueryHeight(int x){
  if (x<0 || x>=FIELD_SIZE_X)
  {
    kdError() << "ERROR: Query Height for wrong x " << x << endl;
    return 0;
  }
  return mFieldFilled.at(x);
}

int Kwin4Doc::QueryLastHint(){
  return mLastHint;
}

void Kwin4Doc::loadSettings(){
  // TODO find out what to do with this...
  // mLevel.setValue(Prefs::level());

  SetName(Gelb, Prefs::name1());
  SetName(Rot, Prefs::name2());

  KGameIO::IOMode mode = KGameIO::MouseIO;
  
  int m = Prefs::input1();
  if(m == 0) mode = KGameIO::MouseIO;
  if(m == 1) mode = KGameIO::ProcessIO;
  if(m == 2) mode = KGameIO::KeyIO;
  setPlayedBy(Gelb, mode);
  
  m = Prefs::input2();
  if(m == 0) mode = KGameIO::MouseIO;
  if(m == 1) mode = KGameIO::ProcessIO;
  if(m == 2) mode = KGameIO::KeyIO;
  setPlayedBy(Rot, mode);

  FARBE col = (FARBE)Prefs::colour1();
  if (QueryPlayerColour(0)!=col)
    SwitchStartPlayer();
}

/**
 * read config file
 */
void Kwin4Doc::ReadConfig(KConfig *config)
{
  loadSettings();
  
  config->setGroup("YellowPlayer");
  getPlayer(Gelb)->readConfig(config);

  config->setGroup("RedPlayer");
  getPlayer(Rot)->readConfig(config);
}

/**
 * write config file
 */
void Kwin4Doc::WriteConfig(KConfig *config)
{
  config->setGroup("YellowPlayer");
  getPlayer(Gelb)->writeConfig(config);

  config->setGroup("RedPlayer");
  getPlayer(Rot)->writeConfig(config);

  config->sync();
}

/**
 * Returns the current player, resp amzug
 */
FARBE Kwin4Doc::QueryCurrentPlayer(){
  return (FARBE)mAmzug.value();
}

void Kwin4Doc::SetCurrentPlayer(FARBE i)
{
  mAmzug.setValue(i);
}

/**
 * Swtich the starting player and return the new started
 */
FARBE Kwin4Doc::SwitchStartPlayer()
{
  if (mStartPlayer.value()==Gelb)
    mStartPlayer.setValue(Rot);
  else
    mStartPlayer.setValue(Gelb);
  
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

/**
 * Return the name of the computer player process
 */
QString Kwin4Doc::QueryProcessName()
{
  // First try a local dir override
  QDir dir;
  QString filename=dir.path()+QString("/kwin4/kwin4proc");
  QFile flocal(filename);
  if (flocal.exists())
  {
    if (global_debug>1) kdDebug(12010) << " Found local process " << filename << endl;
    return filename;
  }
  QString path=kapp->dirs()->findExe("kwin4proc");
  if (!path.isNull())
  {
    if (global_debug>1) kdDebug(12010) << " Found system process " << path << endl;
    return path;
  }
  QString empty;
  kdError() <<  "Could not locate the computer player" << endl;
  return empty;
}

void Kwin4Doc::slotMessageUpdate(int /*id*/,Q_UINT32 /*sender*/,Q_UINT32 /*recv*/)
{
//  kdDebug(12010) << "MSG: id=" << id << " sender=" << sender << " receiver="<<recv<< endl;
}

/**
 * Create a KPlayer
 */ 
KPlayer *Kwin4Doc::createPlayer(int /*rtti*/,int io,bool isvirtual)
{
  KPlayer *player = new Kwin4Player;
  if (!isvirtual)
    createIO(player,(KGameIO::IOMode)io);
  
  connect(player,SIGNAL(signalPropertyChanged(KGamePropertyBase *, KPlayer *)),
          this,SLOT(slotPlayerPropertyChanged(KGamePropertyBase *, KPlayer *)));
  ((Kwin4Player *)player)->setWidget(pView->statusWidget());
  return player;
}

/** 
 * Called when a player input is received from the KGame object 
 * this is e-.g. a mouse event
 */
bool Kwin4Doc::playerInput(QDataStream &msg, KPlayer * /*player*/)
{
  int move, pl;
  msg >> pl >> move;
  if (!Move(move,pl))
    QTimer::singleShot(0, this,SLOT(slotRepeatMove()));

  return false;
}

/**
 * Reactivate player in case of a move which could not pe performed
 */
void Kwin4Doc::slotRepeatMove()
{
  getPlayer(QueryCurrentPlayer())->setTurn(true);
}

/**
 * Performs a game move
 */
bool Kwin4Doc::Move(int x,int id)
{
  if (global_debug>1)
    kdDebug(12010) <<" Kwin4Doc::Move("<<x<<","<<id<<")"<<endl;

  return (MakeMove(x,0) == GNormal);
}

/**
 * return -1: remis, 1:won, 0: continue
 */
int Kwin4Doc::checkGameOver(KPlayer *p)
{
  if (global_debug>1)
    kdDebug(12010) <<"kwin4doc::checkGameOver::"<<p->userId()<<endl;
  return CheckGameOver(QueryLastcolumn(),QueryLastcolour());
}

KGameIO::IOMode Kwin4Doc::playedBy(int col)
{
  return mPlayedBy[col];
}

void Kwin4Doc::setPlayedBy(int col, KGameIO::IOMode io)
{
  if (global_debug>1)
    kdDebug(12010) << "  Kwin4Doc::setPlayedBy(int "<<col<<",KGameIO::IOMode "<<io<<")" << endl;

  Kwin4Player *player=getPlayer((FARBE)col);

  if (mPlayedBy[col]!=io && !player->isVirtual())
  {
    mPlayedBy[col]=io;
    player->removeGameIO(0); // remove all IO's
    createIO(player,io);
  }
}

/* Get the io values right after a load game as the io the playedby
 * is not set there
 */
void Kwin4Doc::recalcIO()
{
  mPlayedBy[Gelb]=(KGameIO::IOMode)getPlayer(Gelb)->calcIOValue();
  mPlayedBy[Rot]=(KGameIO::IOMode)getPlayer(Rot)->calcIOValue();
}

void Kwin4Doc::createIO(KPlayer *player,KGameIO::IOMode io)
{
  if (!player)
    return;
  
  if (global_debug>1)
    kdDebug(12010) << " Kwin4Doc::createIO(KPlayer *player("<<player->userId()<<"),KGameIO::IOMode "<<io<<") " << endl;

  if (io&KGameIO::MouseIO)
  {
    KGameMouseIO *input;
    if (global_debug>1) kdDebug(12010) << "Creating MOUSE IO to "<<pView<< endl;
    // We want the player to work over mouse
    input=new KGameMouseIO(pView);
    if (global_debug>1) kdDebug(12010) << "MOUSE IO added " << endl;
    // Connect mouse input to a function to process the actual input
    connect(input,SIGNAL(signalMouseEvent(KGameIO *,QDataStream &,QMouseEvent *,bool *)),
            pView,SLOT(slotMouseInput(KGameIO *,QDataStream &,QMouseEvent *,bool *)));
    player->addGameIO(input);
  }
  else if (io&KGameIO::ProcessIO)
  {
    QString file=QueryProcessName();
    if (global_debug>1) kdDebug(12010) << "Creating PROCESS IO " << file  << endl;

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
  else if (io&KGameIO::KeyIO)
  {
    if (global_debug>1) kdDebug(12010) << "Creating KEYBOARD IO " << endl;
    // We want the player to work over keyboard
    KGameKeyIO  *input;
    input=new KGameKeyIO(pView->parentWidget());
    // Connect keys input to a function to process the actual input
    connect((KGameKeyIO *)input,SIGNAL(signalKeyEvent(KGameIO *,QDataStream &,QKeyEvent *,bool *)),
            pView,SLOT(slotKeyInput(KGameIO *,QDataStream &,QKeyEvent *,bool *)));
    player->addGameIO(input);
  }
}

/**
 * This slot is called when a computer move should be generated
 */
void Kwin4Doc::slotPrepareTurn(QDataStream &stream,bool b,KGameIO *input,bool *sendit)
{
  if (global_debug>1)
    kdDebug(12010) << " Kwin4Doc::slotPrepareTurn b="<<b << endl;
 
  *sendit=false;
  // Our player
  KPlayer *player=input->player();
  if (!player->myTurn()) return ;
  if (!b) return ; // only on setTurn(true)

  Q_INT32 pl;
  if (global_debug>1) kdDebug(12010) << "slotPrepareComputerTurn for player id= " << player->id() << endl;
  pl=player->userId();

  prepareGameMessage(stream,pl);

  *sendit=true;
}

/** 
 * Sends the current game status to the computer player
 * Careful: The data needs to be the same than the computer
 * player reading on the other side
 **/
void Kwin4Doc::prepareGameMessage(QDataStream &stream, Q_INT32 pl)
{
  if (global_debug>1) kdDebug(12010) << "          sending col=" << pl << endl;
  stream << pl ;
  // This needs to be the same than the computer player reads!
  stream << (Q_INT32)QueryCurrentMove();
  stream << (Q_INT32)QueryCurrentPlayer();
  stream << (Q_INT32)QueryPlayerColour(0);
  stream << (Q_INT32)QueryPlayerColour(1);
  stream << (Q_INT32)Prefs::level();

  int i,j;
  for (i=0;i<FIELD_SIZE_Y;i++)
  {
    for (j=0;j<FIELD_SIZE_X;j++)
    {
       Q_INT8 col;
       col=QueryColour(j,i);
       stream << col;
    }
    if (global_debug>1) kdDebug(12010)
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

void Kwin4Doc::slotProcessQuery(QDataStream &in,KGameProcessIO * /*me*/)
{
  Q_INT8 cid;
  in >> cid;
  switch(cid)
  {
    case 1:  // value
      long value;
      in >> value;
      if (global_debug>1) kdDebug(12010) << "#### Computer thinks value is " << value << endl;
      SetScore(value);
    break;
    default:
      kdError() << "Kwin4Doc::slotProcessQuery: Unknown id " << cid << endl;
    break;
  }
}

/** 
 * This slot is called by the signal of KGame to indicated
 * that the network connection is done and a new client is
 * connected
 * cid is the id of the client connected. if this is equal
 * gameId() WE are the client
 */
void Kwin4Doc::slotClientConnected(Q_UINT32 cid,KGame *)
{
  if (global_debug>1) kdDebug(12010) << " void Kwin4Doc::slotClientConnected id="<<cid << " we=" <<
  gameId() << " we admin=" << isAdmin() << "master)" << isMaster() << endl;

  if (playerList()->count()!=2)
  {
    kdError() << "SERIOUS ERROR: We do not have two players...Trying to disconnect!"<<endl;
    disconnect();
    return ;
  }

  // Get the two players - more are not possible
  Kwin4Player *p1=(Kwin4Player *)playerList()->at(0);
  Kwin4Player *p2=(Kwin4Player *)playerList()->at(1);
  if (!p1->isVirtual())
  {
    emit signalChatChanged(p1);  
    if (global_debug>1) kdDebug(12010) << "CHAT to player 0 " << endl;
  }
  else
  {
    emit signalChatChanged(p2);  
    if (global_debug>1) kdDebug(12010) << "CHAT to player 1 " << endl;
  }

  // Now check whose turn it is. The Admin will rule this
  if (isAdmin())
  {
    if (global_debug>1) kdDebug(12010) << "WE are ADMIN == COOL ! " << endl;
    // p1 is local
    if (!p1->isVirtual())
    {
      if (global_debug>1) kdDebug(12010) << "p1 id=" << p1->userId() << " is local turn="<<p1->myTurn()<< endl;
      // Exclusive setting of the turn
      p1->setTurn(p1->myTurn(),true);
      p2->setTurn(!p1->myTurn(),true);
    }
    else if (!p2->isVirtual())
    {
      if (global_debug>1) kdDebug(12010) << "p2 id=" << p2->userId() << " is local turn="<<p2->myTurn()<<  endl;
      // Exclusive setting of the turn
      p2->setTurn(p2->myTurn(),true);
      p1->setTurn(!p2->myTurn(),true);
    }
  }
}

/** 
 * Get the KPlayer from the color by searching all players
 * users id's
 **/
Kwin4Player *Kwin4Doc::getPlayer(FARBE col)
{
 Kwin4Player *p;
 for ( p=(Kwin4Player *)playerList()->first(); p!= 0; p=(Kwin4Player *)playerList()->next() )
 {
   if (p->userId()==col)
     return p;
 }
 kdError() << "SERIOUS ERROR: Cannot find player with colour " << col << ".  CRASH imminent" << endl;
 return 0;
}

/**
 * We create a process which calulcates a computer move
 * which is shown as hint
 **/
void Kwin4Doc::calcHint()
{
  // We allocate the hint process only if it is needed
  if (!mHintProcess)
  {
    QString file=QueryProcessName();
    if (global_debug>1) kdDebug(12010) << "Creating HINT PROCESS IO " << endl;

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

/**
 * The compute rprocess sent a hint which we show in the
 * game board
 **/
void Kwin4Doc::slotProcessHint(QDataStream &in,KGameProcessIO * /*me*/)
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
      if (global_debug>1) kdDebug(12010) << "#### Computer thinks pl=" << pl << " move =" << move << endl;
      if (global_debug>1) kdDebug(12010) << "#### Computer thinks hint is " << move << " and value is " << value << endl;
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
   
/**
 * Called when a player property has changed. We check whether the name
 * changed and then update the score widget
 * We should maybe do this for the other properties too to update
 * the status widget...I am not sure here...we'll see
 **/
void Kwin4Doc::slotPlayerPropertyChanged(KGamePropertyBase *prop,KPlayer *player)
{
  if (!pView) return ;
   if (prop->id()==KGamePropertyBase::IdName)
   {
     if (global_debug>1) kdDebug(12010) << "Player name id=" << player->userId() << " changed to " << player->name()<<endl;
     pView->scoreWidget()->setPlayer(player->name(),player->userId());
   }
}

void Kwin4Doc::slotPropertyChanged(KGamePropertyBase *prop,KGame *)
{
   if (!pView)
     return ;
  
   if (prop->id()==mCurrentMove.id())
   {
     pView->scoreWidget()->setMove(mCurrentMove);
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
     if (global_debug>1) kdDebug(12010) << "Amzug changed to " << mAmzug.value()<<endl;
     pView->scoreWidget()->setTurn(mAmzug);
   }
   else if (prop->id()==KGamePropertyBase::IdGameStatus)
   {
     if (gameStatus()==Abort)
     {
       if (global_debug>1) kdDebug(12010) << "PropertyChanged::status signal game abort +++" << endl;
       emit signalGameOver(2,getPlayer(QueryCurrentPlayer()),0); // 2 indicates Abort
     }
     else if (gameStatus()==Run)
     {
       if (global_debug>1) kdDebug(12010) << "PropertyChanged::status signal game run +++" << endl;
       preparePlayerTurn(); // Set the current player to play
       emit signalGameRun();
     }
     else if (gameStatus()==Init)
     {
       if (global_debug>1) kdDebug(12010) << "PropertyChanged::status signal game INIT +++" << endl;
       ResetGame(true);
     }
     else
     {
       if (global_debug>1) kdDebug(12010) << "PropertyChanged::other status signal +++" << endl;
     }
     
   }
}

/** 
 * Called by KGame if the game has ended. 
 * DEBUG only as we do not need any extension to
 * the KGame behavior
 */
void Kwin4Doc::slotGameOver(int status, KPlayer * p, KGame * /*me*/)
{
  if (global_debug>1) kdDebug(12010) << "SlotGameOver: status="<<status<<" lastplayer uid="<<p->userId()<<endl;
  
}

/**
 * This is an overwritten function of KGame which is called
 * when a game is loaded. This can either be via a networ
 * connect or via a real load from file
 **/
bool Kwin4Doc::loadgame(QDataStream &stream,bool network,bool reset)
{
  if (global_debug>1)
    kdDebug () << "loadgame() network=" << network << " reset="<< reset << endl;
  if (!network) setGameStatus(End);

  // Clear out the old game 
  if (global_debug>1) kdDebug(12010)<<"loadgame wants to reset the game"<<endl;
  ResetGame(true);

  // load the new game
  bool res=KGame::loadgame(stream,network,reset);
  if (global_debug>1) kdDebug(12010) << "amzug loaded to ="<<mAmzug.value() << endl;

  // Replay the game be undoing and redoing
  if (global_debug>1) kdDebug(12010) << "REDRAW GAME using undo/redo" << endl;
  if (global_debug>1) kdDebug(12010) << "history cnt="<<mHistoryCnt.value() << endl;
  if (global_debug>1) kdDebug(12010) << "amzug ="<<mAmzug.value() << endl;
  int cnt=0;
  while(UndoMove())
  {
    cnt++;
    if (global_debug>1) kdDebug(12010) << "Undoing move "<<cnt<<endl;
  }
  if (global_debug>1) kdDebug(12010) << "amzug ="<<mAmzug.value() << endl;
  while(cnt>0)
  {
    RedoMove();
    cnt--;
    if (global_debug>1) kdDebug(12010) << "Redoing move "<<cnt<<endl;
  }
  if (global_debug>1) kdDebug(12010) << "amzug ="<<mAmzug.value() << endl;

  // Set the input devices
  recalcIO();
  // And set the right player to turn
  preparePlayerTurn();

  if (global_debug>1)
    kdDebug(12010)  << "loadgame done +++" << endl;
  return res;
}

/** 
 * This is also an overwritten function of KGame. It is
 * Called in the game negotiation upon connect. Here
 * the games have to determine what player is remote and
 * what is local
 * This function is only called in the Admin.
 */
void Kwin4Doc::newPlayersJoin(KGamePlayerList * /*oldList*/,KGamePlayerList *newList,QValueList<int> &inactivate)
{
  if (global_debug>1)
    kdDebug(12010) << "newPlayersJoin: START"<<endl;
  
  Kwin4Player *yellow=getPlayer(Gelb);
  Kwin4Player *red=getPlayer(Rot);
  KPlayer *player;
  // Take the master player with the higher priority. Prioirty is set
  // be the network dialog
  if (yellow->networkPriority()>red->networkPriority())
  {
    // Deactivate the lower one 
    inactivate.append(red->id());
    if (global_debug>1) kdDebug(12010) << "ADMIN keeps yellow and kicks red= " << red->id()<<" userId/col="<<red->userId()<<endl;
    // loop all client players and deactivate the one which have the color
    // yellow
    for ( player=newList->first(); player != 0; player=newList->next() ) 
    {
      if (player->userId()==yellow->userId()) 
      {
        inactivate.append(player->id());
        if (global_debug>1) kdDebug(12010) << "Deactivate C1 " << player->id()<<" col="<<player->userId()<<endl;
      }
    }
  }
  else
  {
    // Deactivate the lower one 
    inactivate.append(yellow->id());
    if (global_debug>1) kdDebug(12010) << "ADMIN keeps red and kicks yellow= " << yellow->id()<<" userId/col="<<yellow->userId()<<endl;
    // loop all client players and deactivate the one which have the color
    // red
    for ( player=newList->first(); player != 0; player=newList->next() ) 
    {
      if (player->userId()==red->userId()) 
      {
        inactivate.append(player->id());
        if (global_debug>1) kdDebug(12010) << "Deactivate C2 " << player->id()<<" col="<<player->userId()<<endl;
      }
    }
  }
  if (global_debug>1)
    kdDebug(12010) << "newPlayersJoin: DONE"<<endl;
}

#include "kwin4doc.moc"
