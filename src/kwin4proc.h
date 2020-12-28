/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 1995-2007 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KWIN4PROC_H_
#define _KWIN4PROC_H_

// own
#include "kwin4global.h"
#include "aiboard.h"
#include "kgameprocess.h"
// Qt
#include <QHash>
// St
#include <cmath>

// Save storage for color arrays
typedef char FARBE;

#define SIZE_Y_ALL 36
#define SIZE_X 6
#define SIZE_Y 5

/** The computer (AI) player. 
  * It is used via the KGameIO framework and serves as abstract input device
  * to the game.
  */
class KComputer : public QObject
{

  Q_OBJECT

  public:
    /** Constructor
      */
    KComputer();

    /** The KGameProcess is the main program and event loop.
      */
    KGameProcess proc;

  public Q_SLOTS:
    /** KGameIO slot to receive a command from the main program.
      * @param in       The data stream
      * @param msgid    The message id
      * @param received The received (we)
      * @param sender   The sender (the game)
      */
    void slotCommand(QDataStream& in, int msgid,int receiver,int sender);

    /** KGameIO slot to init a KGameIO device.
      * @param in  The data stream
      * @param id  The message id
      */
    void slotInit(QDataStream& in, int id);

    /** KGameIO slot to receive the setTurn() command, that is a turn is
      * started or aborted.
      * @param in    The data stream
      * @param turn  True if a turn is started
      */
    void slotTurn(QDataStream& in, bool turn);

  protected:
    /** Move result structure. Combines move position and value.
      */
    typedef struct 
    {
      // Move value
      long value;
      // Move x-position
      int move;
    }MoveResult;

    /** Sends the game value/rating back to the main program.
      * @param value  The rating
      * @param moveNo Which move
      */
    void sendValue(long value, int moveNo );

    /** Generate a random number from 0..max
      * @param max The range [0..max[
      */
    int random(int max);

    /** Start thinking about a new move. Will read the input stream and
      * then start the MinMax AI algorithm.
      * @param in    The data stream
      * @param out   The outgoing data stream
      * @param hint  True if it is a hint move (unused)
      */
    MoveResult think(QDataStream &in,QDataStream &out,bool hint);
    
    /** Check whether the current board is a game over situation.
      * @param field   The board
      * @param numbers The number matrix
      * @return The winner color or Nobody.
      */
    COLOUR isGameOver(FARBE field[][SIZE_X+1],char numbers[]);

    /** Execute move in given color to the given position on the given game board.
      * @param move    The move x coordinate
      * @param color   The move color
      * @param field   The game board
      * @param numbers The number board
      */
    void DoMove(int move, COLOUR color, FARBE field[][SIZE_X+1], char numbers[]);

    /** Execute the AI MinMax code. The best move and its (negative) value are returned.
      * @param color    The current player's color.
      * @param field    The game board
      * @param numbers  The number board
      * @param reklev   The recursion level
      * @param moveNo   The current move number
      * @param first    First recursion
      * @return The move result (best move and its value).
      */
    MoveResult MinMax(COLOUR color,FARBE field[][SIZE_X+1],char numbers[],int reklev,int moveNo, bool first);

    /** Evaluate a position at the end of the recursion.
      * @param curColor The current players' color
      * @param field The game board
      * @return The position value.
      */
    long PositionEvaluation(COLOUR curColor, FARBE field[][SIZE_X+1]);

    /** Load the computer brain from disk.
      */
    void loadBrain();

    /** Save the computer brain to disk.
      */
    void saveBrain();

  private:
    // Move number in game 0..41
    int mCurMoveNo;          
    
    // Maximum recursion of MinMax search (level)
    int mLevel;    

    // Amount of positions evaluated (Statistic)
    int mPosEvaluations; 
    
    // How many pos calculated per [ms] (Statistic)
    float mCalcPosPerMS; 

    // Speed up matrix access
    // rows: 0-5  =6 : horiz(i:0-6) 
    //       6-12 =7 : vert(i:0-5)  
    //      13-24 =12: diag-45(i:...)
    //      25-36 =12: diag45(i:...)
    // Length of the rows (number meaning above)
    char mRowLengths[38];
    // Start of the rows (number meaning above)
    char mStartOfRows[38];


    // Learning of AI:  
    // Store learned boards
    QHash<AIBoard,AIValue> mBrain;

    // Best board in move
    AIBoard mMaxAIBoard;  

    // Learning on or off
    bool mIsLearning;  

    // Is the brain already loaded
    bool mBrainLoaded; 
    
    // Path to the brain file
    QString mBrainDir;  
    
    // Statistics on brain usage
    long mBrainUsed;    
};

#endif // _KWIN4PROC_H_

