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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// KGame includes
#include <kgame.h>
#include <kgameio.h>
#include <kgamepropertyarray.h>

#include "kwin4.h"
#include "kwin4player.h"

#define WIN_VALUE 9999000L
#define NOOFPLAYER  2

// forward declaration of the Kwin4 classes
class Kwin4View;
class QCanvasItem;

/**	Kwin4Doc provides a document object for a document-view model.
  *
  * The Kwin4Doc class provides a document object that can be used in conjunction with the classes Kwin4App and Kwin4View
  * to create a document-view model for standard KDE applications based on KApplication and KTMainWindow. Thereby, the document object
  * is created by the Kwin4App instance and contains the document structure with the according methods for manipulation of the document
  * data by Kwin4View objects. Also, Kwin4Doc contains the methods for serialization of the document data from and to files.
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team. 	
  * @version KDevelop version 0.4 code generation
  */
class Kwin4Doc : public KGame
{
  Q_OBJECT

  public:
    /** Constructor for the fileclass of the application */
    Kwin4Doc(QWidget *parent, const char *name=0);
    /** Destructor for the fileclass of the application */
    ~Kwin4Doc();

    /** adds a view to the document which represents the document contents. Usually this is your main view. */
    void setView(Kwin4View *view);
    /** initializes the document generally */
    bool newDocument(KConfig *config,QString file);
    /** initializes the players */
    void initPlayers();
    /** loads the document by filename and format and emits the updateViews() signal */
    bool openDocument(const QString &filename, const char *format=0);
    /** saves the document under filename and format.*/	
    bool saveDocument(const QString &filename, const char *format=0);
    /** sets the path to the file connected with the document */
    void setAbsFilePath(const QString &filename);
    /** returns the pathname of the current document file*/
    const QString &getAbsFilePath() const;
    /** sets the filename of the document */
    void setTitle(const QString &_t);
    /** returns the title of the document */
    const QString &getTitle() const;

    bool loadgame(QDataStream &stream,bool network,bool reset);



  /**  */
  int QueryLastHint();
  /**  */
  int QueryHeight(int x);
  /**  */
  long QueryScore();
  /**  */
  void SetScore(long i);
  /** Reset the stats */
  void ResetStat();
  /**  */
  int CheckGameOver(int x, FARBE col);
  /** What colour has player i  */
  FARBE QueryPlayerColour(int player);
  /**
    Returns the all time statistics for player i
  */
  int QueryStat(FARBE i,TABLE mode);
  /** Query the name of i */
  QString QueryName(FARBE i);
  /** Set the name of col */
  void SetName(FARBE i,QString n);

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

  /**
  * Find the player whose colour is given
  */
  Kwin4Player *getPlayer(FARBE col);
  
  /** Redo a move */
  bool RedoMove();
  /** Undo a move */
  bool UndoMove();
  /** Make a game move */
  MOVESTATUS MakeMove(int x,int mode);
  /** Is the introduction on */
  bool IsIntro();
  /** Is a game running */
  bool IsRunning();
  /** End a game */
  void EndGame(TABLE mode);
  /** Reset the whole game */
  void ResetGame(bool initview);
  /** Set the colour */
  void SetColour(int x,int y,FARBE c);
  /** Returns colour */
  FARBE QueryColour(int x,int y);

  /** Read game config */
  void ReadConfig(KConfig *config);
  /** Write game config */
  void WriteConfig(KConfig *config);

  /** Returns the current player, resp amzug */
  FARBE QueryCurrentPlayer();
  void SetCurrentPlayer(FARBE i);

  /** Swtich the starting player and return the new started */
  FARBE SwitchStartPlayer();

  int QueryLastcolumn();   // last x moved
  FARBE QueryLastcolour(); // last colour moved
  int QueryCurrentMove();  // 0..42
  void SetCurrentMove(int );  // 0..42
  int QueryMaxMove();      // 0..42
  int QueryHistoryCnt();  // 0..42
  /**  */
  /** Random number between 0..max-1 */
  int Random(int max);
  QString QueryProcessName();

  /** Create a kwin4player */
  KPlayer *createPlayer(int rtti,int io,bool isvirtual);
  /** Calcualte the next players turn */
  KPlayer * nextPlayer(KPlayer *last,bool exclusive=true);

  /** KGame function to deterimine what players are in the game */
  void newPlayersJoin(KGamePlayerList *,KGamePlayerList *,QValueList<int> &);

  protected:

  /** Performs a game move */
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
    /**
    * Ask the computer to calculate a hint
    */
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

  public:	
    /** the list of the views currently connected to the document */
    Kwin4View *pView;	

  private:

  QString title;
  QString absFilePath;

  KGamePropertyInt mLastColumn;   // last x moved
  //FARBE lastcolour; // last colour moved
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

  // TODO Save manually in slotSave
  KGameIO::IOMode mPlayedBy[NOOFPLAYER];
  KGameProcessIO *mHintProcess;
};

#endif // KWIN4DOC_H

