/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWIN4DOC_H
#define KWIN4DOC_H

// own
#include "kwin4global.h"
#include "kwin4player.h"
#include "aiboard.h"
#include "kgamepropertyarray.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgame.h>
#include <libkdegamesprivate/kgame/kgameio.h>
// KF
#include <KConfig>
// Qt
#include <QList>
#include <QVector>

class KWin4View;
class Score;


/**
 * The game document or game engine. It is derived from the KGame framework.
 */
class KWin4Doc : public KGame
{
  Q_OBJECT

  public:
    /** Constructor.
      * @param parent The parent widget
      */
    explicit KWin4Doc(QWidget *parent);

    /** The destructor.
      */
    ~KWin4Doc() override;

    /** Adds a view to the document which displays the document contents.
      * @param view The view to add
      */
    void setView(KWin4View *view);

    /** Initializes the KGame derived players.
      */
    void initPlayers();

    /** Save the document in the datastrem. This is a KGame function.
      * @param stream  The data stream to use
      * @param network Is the saving via the network (a network game)
      * @param reset   Reset parameter forward to KGame
      * @return True on success, false otherwise.
      */	
    bool loadgame(QDataStream &stream, bool network, bool reset) override;

    /** Read the game config from the config file.
      * @param config The config 
      */
    void readConfig(KConfig* config);

    /** Write the game config to the config file.
      * @param config The config 
      */
    void writeConfig(KConfig* config);


    /** End a game. Update statistic and forward end game to view.
      * @param mode Indicate how the game ended for the current player [TWin, TLost, TRemis, TBrk]
      */
    void endGame(TABLE mode);

    /** Reset all the player stats.
      */
    void resetStatistic();

    /** Redoes a move if possible.
      * @return True on success.
      */
    bool redoMove();

    /** Undoes a move if possible.
      * @return True on success.
      */
    bool undoMove();

    /** Generate a computer AI move and show it to the player as hint.
      */
    void calculateHint();

    /** Returns the all time statistics for player of given color
      * The mode determines what statistics to access.
      * @param col  The player color 
      * @param mode The type of data to retrieve [TWin, TRemis, TLost, TBrk, TSum]
      * @return The amount of the queried category.
      */
    int getStatistic(COLOUR col, TABLE mode);

    /** Retrieve the name of the player of the given color.
      * @param col The color
      * @return The name.
      */
    QString getName(COLOUR col);

    /** Set the name of the player of the given color.
      * @param col The color
      * @param n   The new name
      */
    void setName(COLOUR col, const QString& n);

    /** Query the IO mode of player og the given color.
      * @param col The color
      * @return The input device mode.
      */
    KGameIO::IOMode playedBy(int col);

    /** Sets the input device mode for the given player color.
      * @param col  The color
      * @param mode The input device code (Mouse, Key, ...)
      */
    void setPlayedBy(int col,KGameIO::IOMode mode);

    /** Retrieve the player object for the given player colour.
      * @param col The player color
      * @return The player object.
      */
    KWin4Player* getPlayer(COLOUR col);

    /** Swap the start player so that the game is started alternatingly.
      * @return The new start player color.
      */
    COLOUR switchStartPlayer();

    /** Sets the current player.
      * @param no The current player
      */
    void setCurrentPlayer(COLOUR no);

    /** Retrieve the player whose turn it is next.
      * @return The current player.
      */
    COLOUR getCurrentPlayer();

    /** Retrieve the current move number.
      * @return The amount [0..42]
      */
    int getCurrentMove();  

    /** Retrieve the maximum move which has been made before undos.
      * @return The amount [0..42]
      */
    int getMaxMove();     

    /** Retrieve the amount of moves in the undo/redo history.
      * @return The amount [0..42]
      */
    int getHistoryCnt(); 

    /** Find the name of the AI process executable file.
      * @return The filename
      */
    QString findProcessName();


  protected:
    /** Create and add an KGameIO device to an given player. 
      * The old ones have to be removed manually before.
      * @param player The player to modify
      * @param io     The IO mode (Mouse, AI, Keyboard, ...)
      */
    void createIO(KPlayer* player, KGameIO::IOMode io);

    /** Create a player of a given type (here only one type possible)
      * and equip it with a given KGameIO device. Virtual players
      * are remote network players.
      * @param rtti  Unused
      * @param io    The IO mode 
      * @param isvirtual True for network players (without physical IO)
      */
    KPlayer* createPlayer(int rtti, int io, bool isvirtual) override;

    /** KGame function to determine the next player. In KWin4 players alternate.
      * @param last      The last player to move
      * @param exclusive unused
      */
    KPlayer* nextPlayer(KPlayer* last, bool exclusive=true) override;

    /** This is also an overwritten function of KGame. It is
      * called in the game negotiation upon connect. Here
      * the games have to determine what player is remote and
      * what is local.
      * @param list     Unused
      * @param newList  List of new players
      * @param inactive List of inactive players
      */
    void newPlayersJoin(KGamePlayerList* list, KGamePlayerList* newList, QList<int>& inactive) override;

    /** Reset the whole game to the beginning (clear board, ...)
      * @param initview  If true also reset the view
      */
    void resetGame(bool initview);

    /** Make a game move to the given position and return a status. 
      * Also displays it in the view.
      * @param x    The position to move to
      * @param mode The mode of the move (0: normal move: 1: redo move)
      * @return The movement status (allowed, normal, ...) 
      */
    MOVESTATUS makeMove(int x,int mode);

    /** Perform a game move. Calls makeMove().
      *  @param x  The position to move to
      *  @param id The player id
      *  @return True if the move was successful.
      */
    bool doMove(int x, int id);

    /** Check whether the field has a game over situation. KGame standard
      * function.
      * @param player The current player
      * @return -1: draw, 1: won, 0: continue game
      */
    int checkGameOver(KPlayer *player) override;

    /** Check whether the field has a game over situation. Called by
      * above standard KGame function but with more suitable parameters.
      * @param x   The position of the last move
      * @param col The color of the last move
      */
    int checkGameOver(int x, COLOUR col);

    /** Pack the current game into a data stream so that it can be
      * send to the computer AI.
      * @param stream The data stream to write to
      * @param pl     The player id
      */
    void prepareGameMessage(QDataStream& stream, qint32 pl);

    /** Main function to handle player input. This function is 
      * the central input for all player inputs. Mouse, Keyboard
      * AI or network end here in the same format. A move is
      * initiated here.
      * @param msg    The game move message 
      * @param player The sender player
      */
    bool playerInput(QDataStream& msg, KPlayer* player) override;

    /** Set the IO devices new.
      */
    void recalcIO();

    /** Set the turn of the current player to true so that
      * he can move.
      */
    void activateCurrentPlayer();

    /** Set the score value of the AI.
      * @param value The score value.
      */
    void setScore(long value);

    /** Set the colour of a position on the game board.
      * @param x The x position [0-6]
      * @param y The y position [0-5]
      * @param c The color [Red, Yellow, Nobody]
      */
    void setColour(int x,int y,COLOUR c);

    /** Retrieve the colour of a position on the game board.
      * @param x The x position [0-6]
      * @param y The y position [0-5]
      * @return The color [Red, Yellow, Nobody]
      */
    COLOUR getColour(int x,int y);

    /** Retrieve the color of the i-th player. Player 0 is the start
      * player and player 1 the follow up player.
      * @param player The player number [0,1]
      * @return The color of the player.
      */
    COLOUR getPlayerColour(int player);

  public Q_SLOTS:
    /** Indication from the view that a move has been displayed. Now
      * The next player can be switched.
      * @param mode A user defined (unused) mode
      */
    void moveDone(int mode);

    /** Load the game properties from the settings. Either the config file
      * or the config dialog call this.
      */
    void loadSettings();

  protected Q_SLOTS:  
    /** Initiate a repeat of the move. This happens if somehow the player
      * input created an invalid move. The same player has to input again.
      */
    void repeatMove();

    /** An AI command was received from the computer AI _hint_ process. Process it.
      * Currently this is only the hint move.
      * @param in The input stream from the process
      * @param io The io device
      */
    void processAIHintCommand(QDataStream& in, KGameProcessIO *io);

    /** An AI command was received from the computer AI _input device_ process. Process it.
      * Currently this is only the move score value.
      * @param in The input stream from the process
      * @param io The io device
      */
    void processAICommand(QDataStream& in, KGameProcessIO* io);

    /** This slot is called by the signal of KGame to indicated
     * that the network connection is done and a new client is
     * connected
     * @param cid Is the id of the client connected. If this is equal gameId() WE are the client.
     * @param me  The game
     */
    void clientConnected(quint32 cid, KGame* me);

    /** This slot is called by the KGame input device when we should prepare a message
      * to the AI process.
      * @param stream   The message stream
      * @param b        True if it is our turn
      * @param input    The input device
      * @param eatevent Set to true if a message has been send
      */
    void prepareAITurn(QDataStream &stream,bool b,KGameIO *input,bool *eatevent);

    /** Debug: Listen to network messages.
      * @param id       The message id
      * @param sender   The sender
      * @param receiver The receiver
      */
    void networkMessageUpdate(int id, quint32 sender, quint32 receiver);

    /** Called by KGame when a player property has changed. 
      * We check whether the name changed and then update the score widget.
      * @param prop   The property
      * @param player The affected player
      */
    void playerPropertyChanged(KGamePropertyBase* prop, KPlayer* player);

    /** Called by KGame when a game property has changed. We update the game
      * status etc.
      * @param prop  The property
      * @param me    The game
      */
    void gamePropertyChanged(KGamePropertyBase* prop, KGame* me);

    /** Received a debug message from the AI (debug only)
      * @param s The message
      */
    void receivedStderr(const QString &s);

  Q_SIGNALS:
    /** Emitted if the game status changes to run.
      */
    void signalGameRun();

    /** Emitted if the chat origin changes.
      * @param player The affected player
      */
    void signalChatChanged(KWin4Player* player);

    /** Emitted when the next players move is due.
      * @param playerNumber The number of the player
      */
    void signalNextPlayer(int playerNumber);

  private:
    // The view
    KWin4View *pView;

    // Last x position moved to
    KGamePropertyInt mLastColumn;  

    // Colour of last move
    KGamePropertyInt mLastColour;  
    
    // Amount of info in history
    KGamePropertyInt mHistoryCnt; 

    // 42 pieces construct the game board
    KGamePropertyArray<int> mField;

    // Player who started game
    KGamePropertyInt mStartPlayer;  

    // Player's to move
    KGamePropertyInt mAmzug; 

    // Maximal move made in a game before undo
    KGamePropertyInt mMaxMove; 

    // Current move number in the game
    KGamePropertyInt mCurrentMove; 

    // To what height is a column filled
    KGamePropertyArray<int> mFieldFilled; 

    // Position of last hint given
    KGamePropertyInt mLastHint;

    // Computer score value (position estimation)
    KGamePropertyInt mScore;  

    // History of all moves (x positions)
    KGamePropertyArray<int> mHistory; 

    // Input device of players
    KGameIO::IOMode mPlayedBy[2];

    // Process AI for hints
    KGameProcessIO *mHintProcess;

    // Score and status storage to communicate with view
    Score* mStatus; 

    // Keep AI move values
    QVector<long> mAIValues;
};

#endif // KWIN4DOC_H

