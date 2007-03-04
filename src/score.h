#ifndef SCORE_H
#define SCORE_H
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


// Qt includes
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
    Score(QObject* parent = 0);
    
    /** Set and update the level of the AI.
      * @param level  The new level
      * @param no     The player number [0,1]
      */ 
    void setLevel(int level, int no) {mLevel[no] = level; update();}

    /** Set and update the player name.
      * @param name   The new name
      * @param no     The player number [0,1]
      */ 
    void setPlayerName(QString name,int no) {mName[no] = name;update();}

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
    
    // The amount of aboerted games
    int mBrk[2];
    
    // The input device
    int mInputDevice[2];
};

#endif 

