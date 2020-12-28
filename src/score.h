/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SCORE_H
#define SCORE_H

// Qt
#include <QObject>

class ScoreSprite;


/**
 * The score object is a compatibility object to transfer the score between
 * the various players and status information entities and the score widget.
 */
class Score : public QObject
{ 
    Q_OBJECT

public:
    /** Construct a score object.
      * @param parent The parent object
      */
    explicit Score(QObject* parent = nullptr);
    
    /** Set and update the level of the AI.
      * @param level  The new level
      * @param no     The player number [0,1]
      */ 
    void setLevel(int level, int no) {mLevel[no] = level; update();}

    /** Set and update the player name.
      * @param name   The new name
      * @param no     The player number [0,1]
      */ 
    void setPlayerName(const QString &name,int no) {mName[no] = name;update();}

    /** Set and update whose turn it is (which player goes next).
      * @param no     The player number [0,1]
      */ 
    void setTurn(int no) {mTurn = no;update();}  

    /** Set and update the amount of wins of a player.
      * @param amount The new amount
      * @param no     The player number [0,1]
      */ 
    void setWins(int amount, int no) {mWin[no] = amount; update();}

    /** Set and update the amount of losses of a player.
      * @param amount The new amount
      * @param no     The player number [0,1]
      */ 
    void setLosses(int amount, int no) {mLoss[no] = amount; update();}

    /** Set and update the amount of draws of a player.
      * @param amount The new amount
      * @param no     The player number [0,1]
      */ 
    void setRemis(int amount, int no) {mRemis[no] = amount; update();}

    /** Set and update the amount of aborted games of a player.
      * @param amount The new amount
      * @param no     The player number [0,1]
      */ 
    void setBreaks(int amount, int no) {mBrk[no] = amount; update();}

    /** Set and update the input device of a player.
      * @param type   The new input device (KGameIO) [1,2,4,8], 8 is AI 
      * @param no     The player number [0,1]
      */ 
    void setPlayedBy(int type, int no) {mInputDevice[no] = type; update();}

    /** Connect the score to a display sprite. This sprite will 
      * be used for the update of the data.
      * @param s The display sprite
      */
    void setDisplay(ScoreSprite* s);

protected:
    /** Push all data into the display sprite.
      */
    void update();

private:
    // The display sprite
    ScoreSprite* mDisplay;

    // The name of the players
    QString mName[2];
    
    // The level of the AI(s)
    int mLevel[2];
    
    // Whose turn is it
    int mTurn;
    
    // The amount of wins
    int mWin[2];
    
    // The amount of draws
    int mRemis[2];
    
    // The amount of losses
    int mLoss[2];
    
    // The amount of aborted games
    int mBrk[2];
    
    // The input device
    int mInputDevice[2];
};

#endif 

