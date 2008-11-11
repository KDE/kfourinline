/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

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

/** Note: The AI engine of kwin4 does on purpose not implement a perfect 
  *       Connect-4(tm) engine but tries to play more human as playing against
  *       a perfect engine is only frustrating. Connect four is proven to
  *       always win for the first player with perfect play.
  *       see e.g. the Valena AI Engine http://www.ce.unipr.it/~gbe/velena.html
  */

// Header includes
#include "kwin4doc.h"

// include files for Qt
#include <QDir>
#include <QTimer>
#include <QKeyEvent>
#include <QList>
#include <QMouseEvent>

// include files for KDE
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <krandomsequence.h>
#include <kglobal.h>
#include <kcomponentdata.h>

// application specific includes
#include "kwin4view.h"
#include "scoresprite.h"
#include "prefs.h"
#include "score.h"
#include "ui_statuswidget.h"
#include <config-src.h>


#define FIELD_SIZE_X 7
#define FIELD_SIZE_Y 6

// Constructor
KWin4Doc::KWin4Doc(QWidget *parent) : KGame(1234,parent), pView(0), mHintProcess(0)
{
  mStatus = new Score(parent);

  connect(this,SIGNAL(signalPropertyChanged(KGamePropertyBase *,KGame *)),
          this,SLOT(gamePropertyChanged(KGamePropertyBase *,KGame *)));

  dataHandler()->Debug();
  //kDebug(12010) << "Property 7 policy=" << dataHandler()->find(7)->policy();
  setPolicy(KGame::PolicyDirty,true);

  //kDebug(12010) << "Property 7 policy=" << dataHandler()->find(7)->policy();

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
  // Careful only in new resetGame! Maybe unlocal it there!
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
  mStartPlayer=Yellow;
  mStartPlayer.registerData(dataHandler(),KGamePropertyBase::PolicyDirty,QString("mStartPlayer"));
  setCurrentPlayer((COLOUR)mStartPlayer.value()); 
  if (global_debug>1) kDebug(12010) << "amZug policy=" << mAmzug.policy();

  mPlayedBy[Yellow] = KGameIO::MouseIO;
  mPlayedBy[Red]    = KGameIO::MouseIO;


  // AI support
  mAIValues.resize(42);

  // last in init
  resetGame(false);
  
  setGameStatus(Intro);

  // Listen to network
  connect(this,SIGNAL(signalMessageUpdate(int, quint32, quint32)),
         this,SLOT(networkMessageUpdate(int, quint32, quint32)));
  connect(this,SIGNAL(signalClientJoinedGame(quint32, KGame*)),
         this,SLOT(clientConnected(quint32, KGame*)));

  // Change global KGame policy
  //dataHandler()->setPolicy(KGamePropertyBase::PolicyDirty,false);
  dataHandler()->Debug();
}


// Destructor
KWin4Doc::~KWin4Doc()
{
  kDebug() << "~KWin4Doc()";
  if (mHintProcess) delete mHintProcess;
  delete mStatus;
  mStatus = 0;
  kDebug() << "~KWin4Doc() done";
}


// Player initialization
void KWin4Doc::initPlayers()
{
  // Create yellow 
  KWin4Player* yellow = (KWin4Player*)createPlayer(1, mPlayedBy[Yellow], false);
  yellow->setUserId(Yellow);
  yellow->setName(Prefs::name1());
  addPlayer(yellow);
  setPlayedBy(Yellow,mPlayedBy[Yellow]);

  // Create Red
  KWin4Player* red = (KWin4Player*)createPlayer(1, mPlayedBy[Red], false);
  red->setUserId(Red);
  red->setName(Prefs::name1());
  addPlayer(red);
  setPlayedBy(Red,mPlayedBy[Red]);
}


// Set the view to the doc
void KWin4Doc::setView(KWin4View *view)
{
  pView=view;
  connect(pView, SIGNAL(signalMoveDone(int)), this, SLOT(moveDone(int)));
}


// Returns colour on the game board
COLOUR KWin4Doc::getColour(int x,int y)
{
  return (COLOUR)mField.at(x+y*FIELD_SIZE_X);
}


// Set the colour on the game board
void KWin4Doc::setColour(int x,int y,COLOUR c)
{
  if (x<0 || x>=FIELD_SIZE_X || y<0 || y>=FIELD_SIZE_Y)
  {
    kFatal(12010) << "ERROR: setColour on wrong position" << x << " " << y;
    return ;
  }
  mField.setAt(x+y*FIELD_SIZE_X,c);
}


// Reset the whole game (and the view)
void KWin4Doc::resetGame(bool initview)
{
  // Reset field
  for (int x=0;x<FIELD_SIZE_X;x++)
  {
    for (int y=FIELD_SIZE_Y-1;y>=0;y--)
    {
      setColour(x,y,Nobody);
    }
  }
  mFieldFilled.fill(0);

  // Reset game vars
  mHistoryCnt=0;
  mCurrentMove=0;
  mMaxMove=0;
  mLastColumn=-1;
  mLastColour=Nobody;
  setScore(0);
  mLastHint=-1;

  // Reset the view
  if (initview)
  {
    pView->initGame(mStatus);
  }
  
  // Who starts this game
  setCurrentPlayer((COLOUR)mStartPlayer.value()); 
}


// Set current player to setTurn true
void KWin4Doc::activateCurrentPlayer()
{
  if (global_debug>1)
    kDebug(12010) << "Setting the current player to turn";
  getPlayer(getCurrentPlayer())->setTurn(true,true);
}


// End a game. Update statistic and forward end game to view.
void KWin4Doc::endGame(TABLE mode)
{
  setGameStatus(End);
  // TODO pView->clearError();
  pView->endGame();

  // Increase game statisics
  KWin4Player *yellow=getPlayer(Yellow);
  KWin4Player *red=getPlayer(Red);
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
  
}


// Indication that a move has been visually done
void KWin4Doc::moveDone(int /*mode*/ )
{
  if (playerCount()>1)
  {
    playerInputFinished(getPlayer(getCurrentPlayer()));
  }

  // TODO pView->clearError();
}


// Calculate the next players to turn. Here the players just swap.
KPlayer* KWin4Doc::nextPlayer(KPlayer* last, bool /*exclusive*/)
{
  if (global_debug>1)
     kDebug(12010) << "nextPlayer last="<<last->id() << "admin=" << isAdmin();

  // Should be enough if the admin sets the turn
  if (last->userId()==Yellow)
    setCurrentPlayer(Red);
  else
    setCurrentPlayer(Yellow);
  if (global_debug>1)
    kDebug(12010) <<" Current set to "<<getCurrentPlayer();
  if (isAdmin())
    getPlayer(getCurrentPlayer())->setTurn(true,true);
  emit signalNextPlayer(int(getCurrentPlayer()));
  return getPlayer(getCurrentPlayer());
}


// Performs a game move on the given x position. Just calls makeMove()
// and transforms the return value so that true indicates a valid move.
bool KWin4Doc::doMove(int x,int id)
{
  if (global_debug>1)
    kDebug(12010) <<" KWin4Doc::Move pos="<<x<<" id="<<id<<" ";

  return (makeMove(x,0) == GNormal);
}


// Make a game move on the given position. Display it in the view.
// mode=0 normal move, =1: redo move
MOVESTATUS KWin4Doc::makeMove(int x, int mode)
{
  if (x<0 || x>=FIELD_SIZE_X)
  {
    kDebug(12010) << "ERROR: makeMove auf falsche Position" << x;
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
    setColour(mLastHint,hy,Nobody);
    mLastHint=-1;
  }
  if (mode==Tip)
  {
    mLastHint=x;
    setColour(x,y,Tip);
    return GTip ;  // no real move
  }

  mFieldFilled.setAt(x,mFieldFilled.at(x)+1);
  setColour(x,y,getCurrentPlayer());
  
  mHistory.setAt(getHistoryCnt(),x);
  mHistoryCnt=mHistoryCnt.value()+1;

  mLastColour=getCurrentPlayer();
  //if (getCurrentPlayer()==Yellow) setCurrentPlayer(Red);
  //else setCurrentPlayer(Yellow);

  mCurrentMove=mCurrentMove.value()+1;

  // only if a real move isdone the maxmove is raised
  if (mode==0) mMaxMove=mCurrentMove.value();
  mLastColumn=x;

  // Show graphics
  pView->displayMove(x, y, mLastColour, x, mLastColour, mCurrentMove-1 , mode==1?false:true);
 
  return GNormal;
}


// Undo a move.
bool KWin4Doc::undoMove()
{
  if (getHistoryCnt()<1) return false;
  
  if (mLastHint>=0)
  {
    int hy;
    hy=mFieldFilled.at(mLastHint);
    setColour(mLastHint,hy,Nobody);
    mLastHint=-1;
  }
  // kDebug(12010) << "Undo no="<<mHistoryCnt.value();
  mHistoryCnt=mHistoryCnt.value()-1;
  int x=mHistory.at(getHistoryCnt());
  mFieldFilled.setAt(x,mFieldFilled.at(x)-1);
  int y=mFieldFilled.at(x);
  // kDebug(12010) << "Undo x="<<x << "y=" <<y;
  setColour(x,y,Nobody);
  // We have to remove the piece as well...

  mLastColour=getCurrentPlayer();
  if (getCurrentPlayer()==Yellow) setCurrentPlayer(Red);
  else setCurrentPlayer(Yellow);
  mCurrentMove=mCurrentMove.value()-1;

  // Display move and arrow history
  if (getHistoryCnt()>0)
  {
    pView->displayMove(x, y, Nobody, mHistory.at(getHistoryCnt()-1), mLastColour.value(), mCurrentMove, false);
  }
  else
  {
    pView->displayMove(x, y, Nobody, -1, Nobody, mCurrentMove, false);
  }

  if (getHistoryCnt()>0)
    mLastColumn=mHistory.at(getHistoryCnt()-1);
  else
    mLastColumn=-1;

  setScore(0);

  return true;
}


// Redo a move
bool KWin4Doc::redoMove()
{
  if (getHistoryCnt()>=mMaxMove) return false;
  
  int x=mHistory.at(getHistoryCnt());
  //kDebug(12010) << "Redo x=" << x;
  makeMove(x,1);
  if (getCurrentPlayer()==Yellow)
    setCurrentPlayer(Red);
  else
    setCurrentPlayer(Yellow);
  setScore(0);
  return true;
}


// Set the name of the player of the given color
void KWin4Doc::setName(COLOUR col, const QString& n)
{
  getPlayer(col)->setName(n);
}


// Retrieve the name of the player of the given color
QString KWin4Doc::getName(COLOUR col)
{
  return getPlayer(col)->name();
}


// Returns the all time statistics for player of given color
// The mode determines what statistics to access.
int KWin4Doc::getStatistic(COLOUR col, TABLE mode)
{
  KWin4Player *player=getPlayer(col);
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


// Retrieve the color of the i-th player. Player 0 is the start
// player and player 1 the follow up player.
COLOUR KWin4Doc::getPlayerColour(int player){
  if (player==0)
    return (COLOUR)mStartPlayer.value();
  
  if (mStartPlayer.value()==Yellow)
    return Red;
  else
    return Yellow;
}


// Check whether the current game has a game over situation
// return -1: remis, 1:won, 0: continue
int KWin4Doc::checkGameOver(KPlayer* p)
{
  if (global_debug>1)
    kDebug(12010) <<"KWin4Doc::checkGameOver::"<<p->userId();
  return checkGameOver(mLastColumn ,(COLOUR)(mLastColour.value()));
}


// Check whether the current game has a game over situation
// return -1: remis, 1:won, 0: continue
int KWin4Doc::checkGameOver(int x, COLOUR col)
{
  int y,i;
  COLOUR c;
  int star=1;
  COLOUR winc=Nobody;

  // Check vertical up
  int flag=0;
  for (i=0;i<4;i++)
  {
    y=mFieldFilled.at(x)-1-i;
    if (y>=0)
    {
       c=getColour(x,y);
       if (c==col) flag++;
    }
  }
  if (flag>=4 )
  {
    // Store win fields
    for (i=0;i<4;i++)
    {
      y=mFieldFilled.at(x)-1-i;
      pView->displayStar(x,y,star++);
      winc=getColour(x,y);
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
       c=getColour(xx,y);
       if (c==col) flag++;
       else flag=0;
     }
  }
  if (flag>=4 )
  {
    // Store win fields
    y=mFieldFilled.at(x)-1;
    winc=getColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        if (getColour(xx,y)!=winc) break;
        pView->displayStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        if (getColour(xx,y)!=winc) break;
        pView->displayStar(xx,y,star++);
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
        c=getColour(xx,y);
        if (c==col) flag++;
        else flag=0;
      }
    }
  }
  if (flag>=4 ) 
  {
    // Store win fields
    y=mFieldFilled.at(x)-1;
    winc=getColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1-i;
        if (y<0) break;
        if (getColour(xx,y)!=winc) break;
        pView->displayStar(xx,y,star++);
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
        if (getColour(xx,y)!=winc) break;
        pView->displayStar(xx,y,star++);
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
        c=getColour(xx,y);
        if (c==col) flag++;
        else flag=0;
      }
    }
  }
  if (flag>=4 ) 
  {
    // Store win fields
    y=mFieldFilled.at(x)-1;
    winc=getColour(x,y);
    int cnt=0;
    for (i=0;i<4;i++)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1+i;
        if (y>=FIELD_SIZE_Y) break;
        if (getColour(xx,y)!=winc) break;
        pView->displayStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    for (i=-1;i>-4 && cnt<4;i--)
    {
      xx=x+i;
      if (xx>=0 && xx<FIELD_SIZE_X)
      {
        y=mFieldFilled.at(x)-1+i;
        if (y<0) break;
        if (getColour(xx,y)!=winc) break;
        pView->displayStar(xx,y,star++);
        cnt++;
      }
      else break;
    }
    return 1;
  }
  else if (flag>=4) return 1;

  if (mCurrentMove>=42) return -1;

  return 0;
}


// Reset all the player stats
void KWin4Doc::resetStatistic()
{
  getPlayer(Yellow)->resetStats();
  getPlayer(Red)->resetStats();
}


// Set computer AI score value
void KWin4Doc::setScore(long value)
{
  mScore.setValue(value);
}


// Load settings from Prefs
void KWin4Doc::loadSettings()
{
  kDebug() << "++++ KWin4Doc::loadSettings() ";
  kDebug() << "Level:" << Prefs::level();
  kDebug() << "Name:" << Prefs::name1();
  kDebug() << "Name2:" << Prefs::name2();
  kDebug() << "input0mouse:" << Prefs::input0mouse();
  kDebug() << "input0key:" << Prefs::input0key();
  kDebug() << "input0ai:" << Prefs::input0ai();
  kDebug() << "input1mouse:" << Prefs::input1mouse();
  kDebug() << "input1key:" << Prefs::input1key();
  kDebug() << "input1ai:" << Prefs::input1ai();
  kDebug() << "start red:"   << Prefs::startcolourred();
  kDebug() << "start yellow" << Prefs::startcolouryellow();
  kDebug() << "Learning     " << Prefs::learning();
  kDebug() << "Lock         " << Prefs::ailock();


  // Store level for score sprite display
  mStatus->setLevel(Prefs::level(), 0);
  mStatus->setLevel(Prefs::level(), 1);

  setName(Yellow, Prefs::name1());
  setName(Red, Prefs::name2());

  KGameIO::IOMode mode = KGameIO::MouseIO;
  
  if(Prefs::input0mouse())      mode = KGameIO::MouseIO;
  else if(Prefs::input0key())   mode = KGameIO::KeyIO;
  else if(Prefs::input0ai())    mode = KGameIO::ProcessIO;
  else kFatal() << "Unknown input device for player 0";
  if (global_demo_mode)         mode = KGameIO::ProcessIO;
  setPlayedBy(Yellow, mode);
  kDebug() << "Played by Yellow="<<mode;  
  
  if(Prefs::input1mouse())      mode = KGameIO::MouseIO;
  else if(Prefs::input1key())   mode = KGameIO::KeyIO;
  else if(Prefs::input1ai())    mode = KGameIO::ProcessIO;
  else kFatal() << "Unknown input device for player 1";
  if (global_demo_mode)         mode = KGameIO::ProcessIO;
  setPlayedBy(Red, mode);
  kDebug() << "Played by Red="<<mode;  

  COLOUR col = Red;
  if (Prefs::startcolouryellow()) col = Yellow; 

  if (Prefs::startcolourred()) mStartPlayer.setValue(Red);
  else if (Prefs::startcolouryellow()) mStartPlayer.setValue(Yellow);
  else kFatal() << "Unknown start color";
  kDebug() << "Setting start player to" << mStartPlayer;


}


// Read config file
void KWin4Doc::readConfig(KConfig *config)
{
  kDebug() << "++++++++++++++++++++++++++++++++++++ KWin4Doc::ReadConfig";
  loadSettings();
  
  KConfigGroup ygrp = config->group("YellowPlayer");
  getPlayer(Yellow)->readConfig(ygrp);

  KConfigGroup rgrp = config->group("RedPlayer");
  getPlayer(Red)->readConfig(rgrp);
}


// Write config file
void KWin4Doc::writeConfig(KConfig *config)
{
  KConfigGroup ygrp = config->group("YellowPlayer");
  getPlayer(Yellow)->writeConfig(ygrp);

  KConfigGroup rgrp = config->group("RedPlayer");
  getPlayer(Red)->writeConfig(rgrp);

  config->sync();
}


// Returns the current player, resp amzug.
COLOUR KWin4Doc::getCurrentPlayer()
{
  return (COLOUR)mAmzug.value();
}


// Set the current player
void KWin4Doc::setCurrentPlayer(COLOUR no)
{
  mAmzug.setValue(no);
}


// Switch the starting player and return the new started
COLOUR KWin4Doc::switchStartPlayer()
{
  if (mStartPlayer.value()==Yellow)
  {
    mStartPlayer.setValue(Red);
    Prefs::setStartcolouryellow(false);
    Prefs::setStartcolourred(true);
    kDebug() << "Setting startplayer to RED";
  }
  else
  {
    mStartPlayer.setValue(Yellow);
    Prefs::setStartcolouryellow(true);
    Prefs::setStartcolourred(false);
    kDebug() << "Setting startplayer to YELLOW";
  }
  Prefs::self()->writeConfig();
  
  return (COLOUR)mStartPlayer.value();
}


// Retrieve the current move number.
int KWin4Doc::getCurrentMove()
{
  return mCurrentMove;
}


// Retrieve the maximum move number before undo
int KWin4Doc::getMaxMove()
{
  return mMaxMove;
}


// Retrieve the amount of history moves stored
int KWin4Doc::getHistoryCnt()
{
  return mHistoryCnt;
}


// Return the filename of the computer player AI process.
QString KWin4Doc::findProcessName()
{
  // Try whether we run from a development source dir
  #ifndef NDEBUG
  #ifdef SRC_DIR
    QString srcname = QString(SRC_DIR)+QString("/src/kfourinlineproc");
    QFile fsrc(srcname);
    if (fsrc.exists())
    {
      if (global_debug>1) kDebug() << "Found SRC_DIR process" << srcname;
      return srcname;
    }
  #endif
  #endif
  

  // First try a local dir override
  QDir dir;
  // TODO: This local filename is not found!!
  QString filename=dir.path()+QString("/kwin4/kfourinlineproc");
  kDebug() << "PROC FILENAME="<<filename;
  QFile flocal(filename);
  if (flocal.exists())
  {
    if (global_debug>1) kDebug() << "Found local process" << filename;
    return filename;
  }
  QString path=KGlobal::mainComponent().dirs()->findExe("kfourinlineproc");
  if (!path.isNull())
  {
    if (global_debug>1) kDebug() << "Found system process" << path;
    return path;
  }
  QString empty;
  kError() <<  "Could not locate the computer player";
  return empty;
}


// Debug: Listen to messages
void KWin4Doc::networkMessageUpdate(int /*id*/,quint32 /*sender*/,quint32 /*recv*/)
{
//  kDebug(12010) << "MSG: id=" << id << "sender=" << sender << "receiver="<<recv;
}


// Create a KPlayer
KPlayer *KWin4Doc::createPlayer(int /*rtti*/, int io, bool isvirtual)
{
  KWin4Player *player = new KWin4Player;
  if (!isvirtual)
    createIO(player,(KGameIO::IOMode)io);
  
  connect(player,SIGNAL(signalPropertyChanged(KGamePropertyBase *, KPlayer *)),
          this,SLOT(playerPropertyChanged(KGamePropertyBase *, KPlayer *)));
  player->setStatus(mStatus);
  return player;
}


// Called when a player input is received from the KGame object 
// this is e.g. a mouse event, the AI or the network
bool KWin4Doc::playerInput(QDataStream& msg, KPlayer* /*player*/)
{
  qint32 move, pl;
  msg >> pl >> move;
  kDebug() << "KWin4Doc::playerInput: ================ pl="<<pl<<" and move=" << move << "====================";

  // Perform move and check for success
  if (!doMove(move,pl))
  {
    // Repeat the same input
    QTimer::singleShot(0, this,SLOT(repeatMove()));
  }

  return false;
}


// Reactivate player in case of a move which could not pe performed.
void KWin4Doc::repeatMove()
{
  getPlayer(getCurrentPlayer())->setTurn(true);
}


// Query the IO mode of player og the given color.
KGameIO::IOMode KWin4Doc::playedBy(int col)
{
  return mPlayedBy[col];
}


// Sets the input device mode for the given player color.
void KWin4Doc::setPlayedBy(int col, KGameIO::IOMode io)
{
  if (global_debug>1)
    kDebug(12010) << "  KWin4Doc::setPlayedBy(int "<<col<<",KGameIO::IOMode "<<io<<")";

  KWin4Player *player=getPlayer((COLOUR)col);

  // Modes for the score sprite
  player->status()->setPlayedBy((int)io,player->userId());

  if (mPlayedBy[col]!=io && !player->isVirtual())
  {
    bool myTurn = player->myTurn();
    player->setTurn(false); // turn of move
    mPlayedBy[col]=io;
    player->removeGameIO(0); // remove all IO's
    createIO(player,io);
    player->setTurn(myTurn); // turn on move
  }
}


// Get the io values right after a load game as the io the playedby
// is not set there.
void KWin4Doc::recalcIO()
{
  mPlayedBy[Yellow]=(KGameIO::IOMode)getPlayer(Yellow)->calcIOValue();
  mPlayedBy[Red]=(KGameIO::IOMode)getPlayer(Red)->calcIOValue();
}


// Create player input devicea (KGame)
void KWin4Doc::createIO(KPlayer* player, KGameIO::IOMode io)
{
  if (!player)
    return;
  
  if (global_debug>1)
    kDebug() << "KWin4Doc::createIO(KPlayer *player("<<player->userId()<<"),KGameIO::IOMode "<<io<<") ";

  if (io&KGameIO::MouseIO)
  {
    KGameMouseIO *input;
    if (global_debug>1) kDebug() << "Creating MOUSE IO to "<<pView;
    // We want the player to work over mouse. So please leave the "true" for mouse
    // tracking on !!!
    input=new KGameMouseIO(pView->viewport(), true);
    if (global_debug>1) kDebug(12010) << "MOUSE IO added";
    // Connect mouse input to a function to process the actual input
    connect(input,SIGNAL(signalMouseEvent(KGameIO *,QDataStream &,QMouseEvent *,bool *)),
            pView,SLOT(mouseInput(KGameIO *,QDataStream &,QMouseEvent *,bool *)));
    player->addGameIO(input);
  }
  else if (io&KGameIO::ProcessIO)
  {
    QString file=findProcessName();
    if (global_debug>1) kDebug() << "Creating PROCESS IO" << file;

    KGameProcessIO *input;
    // We want a computer player
    input=new KGameProcessIO(file);
    // Connect computer player to the setTurn
    connect(input,SIGNAL(signalPrepareTurn(QDataStream &,bool,KGameIO *,bool *)),
            this,SLOT(prepareAITurn(QDataStream &,bool,KGameIO *,bool *)));

    connect(input,SIGNAL(signalProcessQuery(QDataStream&, KGameProcessIO*)),
            this,SLOT(processAICommand(QDataStream&, KGameProcessIO*)));

    connect(input,SIGNAL(signalReceivedStderr(QString)),
            this,SLOT(receivedStderr(QString)));
    player->addGameIO(input);
  }
  else if (io&KGameIO::KeyIO)
  {
    if (global_debug>1) kDebug() << "Creating KEYBOARD IO";
    // We want the player to work over keyboard
    KGameKeyIO  *input;
    input=new KGameKeyIO(pView->parentWidget());
    // Connect keys input to a function to process the actual input
    connect((KGameKeyIO *)input,SIGNAL(signalKeyEvent(KGameIO *,QDataStream &,QKeyEvent *,bool *)),
            pView,SLOT(keyInput(KGameIO *,QDataStream &,QKeyEvent *,bool *)));
    player->addGameIO(input);
  }
}

void KWin4Doc::receivedStderr(const QString &s)
{
  if (global_debug>0)
    kDebug() << "##### AI:" << s;
}


// This slot is called when a computer move should be generated
void KWin4Doc::prepareAITurn(QDataStream& stream, bool b, KGameIO* input, bool* sendit)
{
  if (global_debug>1)
    kDebug(12010) << "KWin4Doc::prepareAITurn b="<<b;
 
  // Set defaults
  *sendit = false;

  // Our player
  KPlayer* player=input->player();
  if (!player->myTurn()) return ;
  if (!b) return ; // only create move on setTurn(true)

  qint32 pl;
  if (global_debug>1) kDebug(12010) << "slotPrepareComputerTurn for player id=" << player->id();
  pl=player->userId();

  // Pack the game into the message
  prepareGameMessage(stream,pl);

  // Do send
  *sendit=true;
}

// Sends the current game status to the computer player
// Careful: The data needs to be exatcly the same as the computer
// player reading on the other side
void KWin4Doc::prepareGameMessage(QDataStream& stream, qint32 pl)
{
  if (global_debug>1) kDebug(12010) << "          sending col=" << pl;
  stream << pl ;
  // This needs to be the same than the computer player reads!
  stream << (qint32)getCurrentMove();
  stream << (qint32)getCurrentPlayer();
  stream << (qint32)getPlayerColour(0);
  stream << (qint32)getPlayerColour(1);
  stream << (qint32)Prefs::level();

  bool learning = Prefs::learning();
  // Allow learning only for one AI
  if ( mPlayedBy[Yellow] == KGameIO::ProcessIO &&
       mPlayedBy[Red]    == KGameIO::ProcessIO &&
       pl == Yellow) learning = false;
  stream << (qint32)learning;

  // Where to save the learn cache
  QString learnPath =  KGlobal::dirs()->saveLocation("data","kwin4",true);
  stream << learnPath;

  int i,j;
  for (i=0;i<FIELD_SIZE_Y;i++)
  {
    for (j=0;j<FIELD_SIZE_X;j++)
    {
       qint8 col;
       col=getColour(j,i);
       stream << col;
    }
    if (global_debug>1) kDebug(12010)
      << getColour(0,i)  << " "
      << getColour(1,i)  << " "
      << getColour(2,i)  << " "
      << getColour(3,i)  << " "
      << getColour(4,i)  << " "
      << getColour(5,i)  << " "
      << getColour(6,i);
  }
  stream << (qint32)421256;
}


// The AI send a command, e.g. the game value to us
void KWin4Doc::processAICommand(QDataStream& in, KGameProcessIO* io)
{
  qint8 cid;

  // Receive command
  in >> cid;
  switch(cid)
  {
    case 1:  // game value
    {
      AIBoard aiBoard;
      qint32 value, moveNo, level;
      in >> value >> moveNo >> level >> aiBoard;
      if (global_debug>1) kDebug(12010) << "#### Computer thinks move" << moveNo << "value is" << value;
      // Store AI data
      mAIValues[moveNo] = value;
      setScore(value);

      // Check AI mistakes
      if (moveNo>=2)
      {
        long delta = mAIValues[moveNo]-mAIValues[moveNo-2];

        // Send game to process
        QByteArray buffer;
        QDataStream outstream(&buffer, QIODevice::WriteOnly);
        // Send last board to learn with current value
        outstream << aiBoard << value << (qint32)delta << level;
        io->sendMessage(outstream, 3, 0, gameId());
      }
    }
    break;
    default:
      kError() << "KWin4Doc::processAICommand: Unknown id" << cid;
    break;
  }
}


// This slot is called by the signal of KGame to indicated
// that the network connection is done and a new client is
// connected
// cid is the id of the client connected. if this is equal
// gameId() WE are the client
void KWin4Doc::clientConnected(quint32 cid, KGame* /* me */)
{
  if (global_debug>1) kDebug(12010) << "void KWin4Doc::clientConnected id="<<cid << "we=" <<
  gameId() << "we admin=" << isAdmin() << "master)" << isMaster();

  if (playerList()->count()!=2)
  {
    kError() << "SERIOUS ERROR: We do not have two players...Trying to disconnect!";
    disconnect();
    return ;
  }

  // Get the two players - more are not possible
  KWin4Player* p1=(KWin4Player *)playerList()->at(0);
  KWin4Player* p2=(KWin4Player *)playerList()->at(1);
  if (!p1->isVirtual())
  {
    emit signalChatChanged(p1);  
    if (global_debug>1) kDebug(12010) << "CHAT to player 0";
  }
  else
  {
    emit signalChatChanged(p2);  
    if (global_debug>1) kDebug(12010) << "CHAT to player 1";
  }

  // Now check whose turn it is. The Admin will rule this
  if (isAdmin())
  {
    if (global_debug>1) kDebug(12010) << "WE are ADMIN == COOL ! ";
    // p1 is local
    if (!p1->isVirtual())
    {
      if (global_debug>1) kDebug(12010) << "p1 id=" << p1->userId() << "is local turn="<<p1->myTurn();
      // Exclusive setting of the turn
      p1->setTurn(p1->myTurn(),true);
      p2->setTurn(!p1->myTurn(),true);
    }
    else if (!p2->isVirtual())
    {
      if (global_debug>1) kDebug(12010) << "p2 id=" << p2->userId() << "is local turn="<<p2->myTurn();
      // Exclusive setting of the turn
      p2->setTurn(p2->myTurn(),true);
      p1->setTurn(!p2->myTurn(),true);
    }
  }
}


// Get the KPlayer from the color by searching all players
// users id's
KWin4Player* KWin4Doc::getPlayer(COLOUR col)
{
 for (KGamePlayerList::const_iterator it = playerList()->constBegin(); it!= playerList()->constEnd(); it++ )
 {
   if ((*it)->userId()==col)
     return (KWin4Player *)(*it);
 }
 kError() << "SERIOUS ERROR: Cannot find player with colour" << col << ".  CRASH imminent";
 return 0;
}


// We create a process which calulcates a computer move which is shown as hint to the player.
void KWin4Doc::calculateHint()
{
  // We allocate the hint process only if it is needed
  if (!mHintProcess)
  {
    QString file = findProcessName();
    if (global_debug>1) kDebug(12010) << "Creating HINT PROCESS";

    // We want a computer player
    mHintProcess=new KGameProcessIO(file);

    connect(mHintProcess,SIGNAL(signalProcessQuery(QDataStream&, KGameProcessIO*)),
            this,SLOT(processAIHintCommand(QDataStream&, KGameProcessIO*)));
  }

  // Send game to process
  qint32 pl;
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  pl=getCurrentPlayer();
  prepareGameMessage(stream, pl);
  mHintProcess->sendMessage(stream, 2, 0, gameId());
}


// The compute process sent a hint which we show in the game board.
void KWin4Doc::processAIHintCommand(QDataStream& in,KGameProcessIO* /*io*/)
{
  qint8 cid;
  // Read command
  in >> cid;
  switch(cid)
  {
    case 2:  // Hint
    {
      qint32 pl;
      qint32 move;
      qint32 value;
      // Read parameters of command
      in >>  pl >> move  >> value;
      if (global_debug>1) kDebug(12010) << "#### Computer thinks pl=" << pl << "move =" << move;
      if (global_debug>1) kDebug(12010) << "#### Computer thinks hint is" << move << "and value is" << value;

      // Display hint
      int x = move;
      int y = mFieldFilled.at(x);
      pView->displayHint(x,y);
    }
    break;
    default:
      kError() << "KWin4Doc::processAIHintCommand: Unknown id" << cid;
    break;
  }
}
   

// Called when a player property has changed. We check whether the name
// changed and then update the score widget
// We should maybe do this for the other properties too to update
// the status widget...I am not sure here...we'll see
void KWin4Doc::playerPropertyChanged(KGamePropertyBase* prop,KPlayer* player)
{
  if (!pView) return ;

  // Check for name changes
  if (prop->id()==KGamePropertyBase::IdName)
  {
    if (global_debug>1) kDebug(12010) << "Player name id=" << player->userId() << "changed to" << player->name();
    mStatus->setPlayerName(player->name(),player->userId());
  }

}


// Called by KGame when a game property has changed. 
void KWin4Doc::gamePropertyChanged(KGamePropertyBase* prop, KGame* /* me */)
{
   if (!pView) return;
  
   // Move number
   if (prop->id()==mCurrentMove.id())
   {
     // TODO pView->scoreWidget()->setMove(mCurrentMove);
   }

   // Computer AI value
   else if (prop->id()==mScore.id())
   {
     int sc=mScore/10000;
     if (sc==0 && mScore.value()>0) sc=1;
     else if (sc==0 && mScore.value()<0) sc=-1;
     // TODO pView->scoreWidget()->setChance(sc);
   }

   // Whose turn is it
   else if (prop->id()==mAmzug.id())
   {
     if (global_debug>1) kDebug(12010) << "Amzug changed to" << mAmzug.value();
     mStatus->setTurn(mAmzug);
   }

   // The game status
   else if (prop->id()==KGamePropertyBase::IdGameStatus)
   {
     if (gameStatus()==Abort)
     {
       if (global_debug>1) kDebug(12010) << "PropertyChanged::status signal game abort +++";
       emit signalGameOver(2,getPlayer(getCurrentPlayer()),this); // 2 indicates Abort
     }
     else if (gameStatus()==Run)
     {
       if (global_debug>1) kDebug(12010) << "PropertyChanged::status signal game run +++";
       if (playerList()->count()==2)
       {
         activateCurrentPlayer(); // Set the current player to play
         emit signalGameRun();
       }
       if (global_debug>1) kDebug(12010) << "PropertyChanged::status signal game run done +++";
     }
     else if (gameStatus()==Init)
     {
       if (global_debug>1) kDebug(12010) << "PropertyChanged::status signal game INIT +++";
       resetGame(true);
     }
     else if (gameStatus()==End)
     {
       if (global_debug>1) kDebug(12010) << "PropertyChanged::status signal game END +++";
     }
     else
     {
       if (global_debug>1) kDebug(12010) << "PropertyChanged::other status signal +++";
     }
     
   }
}


// This is an overwritten function of KGame which is called
// when a game is loaded. This can either be via a networ
// connect or via a real load from file
bool KWin4Doc::loadgame(QDataStream &stream,bool network,bool reset)
{
  if (global_debug>1)
    kDebug () << "loadgame() network=" << network << "reset="<< reset;
  if (!network) setGameStatus(End);

  // Clear out the old game 
  if (global_debug>1) kDebug(12010)<<"loadgame wants to reset the game";
  resetGame(true);

  // load the new game
  bool res=KGame::loadgame(stream,network,reset);
  if (global_debug>1) kDebug(12010) << "amzug loaded to ="<<mAmzug.value();

  // Replay the game be undoing and redoing
  if (global_debug>1) kDebug(12010) << "REDRAW GAME using undo/redo";
  if (global_debug>1) kDebug(12010) << "history cnt="<<mHistoryCnt.value();
  if (global_debug>1) kDebug(12010) << "amzug ="<<mAmzug.value();
  int cnt=0;
  while(undoMove())
  {
    cnt++;
    if (global_debug>1) kDebug(12010) << "Undoing move "<<cnt;
  }
  if (global_debug>1) kDebug(12010) << "amzug ="<<mAmzug.value();
  while(cnt>0)
  {
    redoMove();
    cnt--;
    if (global_debug>1) kDebug(12010) << "Redoing move "<<cnt;
  }
  if (global_debug>1) kDebug(12010) << "amzug ="<<mAmzug.value();

  // Set the input devices
  recalcIO();
  // And set the right player to turn
  activateCurrentPlayer();

  if (global_debug>1)
    kDebug(12010)  << "loadgame done +++";
  return res;
}


// This is also an overwritten function of KGame. It is
// Called in the game negotiation upon connect. Here
// the games have to determine what player is remote and
// what is local
// This function is only called in the Admin.
void KWin4Doc::newPlayersJoin(KGamePlayerList* /*oldList*/,KGamePlayerList* newList,QList<int> &inactivate)
{
  if (global_debug>1)
    kDebug(12010) << "newPlayersJoin: START";
  
  KWin4Player *yellow=getPlayer(Yellow);
  KWin4Player *red=getPlayer(Red);
  // Take the master player with the higher priority. Prioirty is set
  // be the network dialog
  if (yellow->networkPriority()>red->networkPriority())
  {
    // Deactivate the lower one 
    inactivate.append(red->id());
    if (global_debug>1) kDebug(12010) << "ADMIN keeps yellow and kicks red=" << red->id()<<" userId/col="<<red->userId();
    // loop all client players and deactivate the one which have the color
    // yellow
    for ( KGamePlayerList::const_iterator it = newList->constBegin(); it != newList->constEnd(); it++ ) 
    {
      KPlayer *player = *it;
      if (player->userId()==yellow->userId()) 
      {
        inactivate.append(player->id());
        if (global_debug>1) kDebug(12010) << "Deactivate C1" << player->id()<<" col="<<player->userId();
      }
    }
  }
  else
  {
    // Deactivate the lower one 
    inactivate.append(yellow->id());
    if (global_debug>1) kDebug(12010) << "ADMIN keeps red and kicks yellow=" << yellow->id()<<" userId/col="<<yellow->userId();
    // loop all client players and deactivate the one which have the color
    // red
    for ( KGamePlayerList::const_iterator it = newList->constBegin(); it != newList->constEnd(); it++ ) 
    {
      KPlayer *player = *it;
      if (player->userId()==red->userId()) 
      {
        inactivate.append(player->id());
        if (global_debug>1) kDebug(12010) << "Deactivate C2" << player->id()<<" col="<<player->userId();
      }
    }
  }
  if (global_debug>1)
    kDebug(12010) << "newPlayersJoin: DONE";
}

#include "kwin4doc.moc"
