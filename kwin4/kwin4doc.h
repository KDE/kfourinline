/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2000 by Martin Heni
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

#ifndef KWIN4DOC_H
#define KWIN4DOC_H

#include <kgame.h>
#include <kgameio.h>
#include <kgamepropertyarray.h>

#include "kwin4player.h"

class Kwin4View;
class QCanvasItem;

extern int global_debug;

// The user or color?
typedef enum e_Farbe {Niemand=-1,Gelb=0,Rot=1,Tip=3,Rand=4,GelbWin=8,RotWin=9} FARBE;
// The type of player
typedef enum  {Men=0,Computer=1,Remote=2} PLAYER;
typedef enum  {TSum,TWin,TRemis,TLost,TBrk} TABLE;
typedef enum  {GIllMove=-2,GNotAllowed=-1,GNormal=0,GYellowWin=1,GRedWin=2,GRemis=3,GTip=4} MOVESTATUS;

#define NOOFPLAYER 2

#define FIELD_SIZE_X 7
#define FIELD_SIZE_Y 6
#define FIELD_SPACING 40

/**
 * The board "engine"
 */
class Kwin4Doc : public KGame
{
Q_OBJECT

public:
  Kwin4Doc(QWidget *parent, const char *name=0);
  ~Kwin4Doc();

  /** adds a view to the document which represents the document contents. Usually this is your main view. */
  void setView(Kwin4View *view);
  /** initializes the players */
  void initPlayers();
  /** saves the document under filename and format.*/	
  bool loadgame(QDataStream &stream, bool network, bool reset);

  int QueryLastHint();
  int QueryHeight(int x);
  void SetScore(long i);
  void ResetStat();
  int CheckGameOver(int x, FARBE col);
  FARBE QueryPlayerColour(int player);
  int QueryStat(FARBE i, TABLE mode);
  QString QueryName(FARBE i);
  void SetName(FARBE i, const QString &n);

  /**
  * Set and query the IO mode of player Gelb/Rot
  */
  KGameIO::IOMode playedBy(int col);
  void setPlayedBy(int col,KGameIO::IOMode mode);

  /** 
  * create and add an IO device to an given player. 
  * The old ones have to be removed manually before
  */
  void createIO(KPlayer *player,KGameIO::IOMode io);

  Kwin4Player *getPlayer(FARBE col);
  
  bool RedoMove();
  bool UndoMove();

  /** Make a game move */
  MOVESTATUS MakeMove(int x,int mode);
  /** End a game */
  void EndGame(TABLE mode);
  /** Reset the whole game */
  void ResetGame(bool initview);
  /** Set the colour */
  void SetColour(int x,int y,FARBE c);
  /** Returns colour */
  FARBE QueryColour(int x,int y);

  void ReadConfig(KConfig *config);
  void WriteConfig(KConfig *config);

  FARBE QueryCurrentPlayer();
  void SetCurrentPlayer(FARBE i);

  FARBE SwitchStartPlayer();

  int QueryLastcolumn();   // last x moved
  FARBE QueryLastcolour(); // last colour moved
  int QueryCurrentMove();  // 0..42
  void SetCurrentMove(int );  // 0..42
  int QueryMaxMove();      // 0..42
  int QueryHistoryCnt();  // 0..42
  QString QueryProcessName();

  KPlayer *createPlayer(int rtti, int io, bool isvirtual);
  KPlayer * nextPlayer(KPlayer *last, bool exclusive=true);

  void newPlayersJoin(KGamePlayerList *,KGamePlayerList *,QValueList<int> &);

protected:
  bool Move(int x,int id);
  /** Check whether the field has a game over situation */
  int checkGameOver(KPlayer *);
  /** Send to the computer player */
  void prepareGameMessage(QDataStream &stream, Q_INT32 pl);
  /** Main function to do player input */
  bool playerInput(QDataStream &msg,KPlayer *player);
  /** Set the IO devices new */
  void recalcIO();
  /** Set the turn of the current player to true */
  void preparePlayerTurn();


public slots:
  void calcHint();
  
  void slotPropertyChanged(KGamePropertyBase *,KGame *);
  void slotPlayerPropertyChanged(KGamePropertyBase *,KPlayer *);
  void moveDone(QCanvasItem *,int);
  void slotMessageUpdate(int,Q_UINT32,Q_UINT32);
  void slotPrepareTurn(QDataStream &stream,bool b,KGameIO *input,bool *eatevent);
  void slotClientConnected(Q_UINT32,KGame *);
  void slotProcessQuery(QDataStream &,KGameProcessIO *);
  void slotProcessHint(QDataStream &,KGameProcessIO *);
  void slotGameOver(int status, KPlayer * p, KGame * me);
  void slotRepeatMove();
  void loadSettings();

signals:
  /**
  * emmitted if the game status changes to run
  */
  void signalGameRun();
  /**
  * Emmitted if the chat origin changes
  */
  void signalChatChanged(Kwin4Player *);
  /**
  * emmitted after a sprite move ends
  **/
  void signalMoveDone(int,int);
  void signalNextPlayer();
  /**
  * emmitted if the game ends
  **/
  void GameOver(int,KPlayer *,KGame *);

private:
  Kwin4View *pView;

  KGamePropertyInt mLastColumn;   // last x moved
  KGamePropertyInt mLastColour; // last colour moved

  KGamePropertyInt mHistoryCnt;
  KGamePropertyArray<int> mField; // 42 pieces
  Kwin4Player *yellowPlayer;
  KGamePropertyInt mStartPlayer;  // Player started game
  KGamePropertyInt mAmzug;        // Player's to move
  KGamePropertyInt mMaxMove;      // maximal move made in a game before undo
  KGamePropertyInt mCurrentMove;  // current move in the game
  KGamePropertyArray<int> mFieldFilled; // to what height is the column filled
  KGamePropertyInt mLastHint;
  KGamePropertyInt mScore;          // Computer score
  KGamePropertyArray<int> mHistory; // to what height is the column filled

  KGameIO::IOMode mPlayedBy[NOOFPLAYER];
  KGameProcessIO *mHintProcess;

};

#endif // KWIN4DOC_H

