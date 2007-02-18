#ifndef _SCORE_H
#define _SCORE_H
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
    Score(QObject* parent = 0);
    void setLevel(int i, int no) {mLevel[no] = i; update();}
    void setChance(int i);
    void setPlayerName(QString s,int no) {mName[no] = s;update();}
    void setTurn(int i) {mTurn = i;update();}  
    void setDisplay(ScoreSprite* s);
    void setWins(int i, int no) {mWin[no] = i; update();}
    void setSum(int i, int no) {mSum[no] = i; update();}
    void setLosses(int i, int no) {mLoss[no] = i; update();}
    void setRemis(int i, int no) {mRemis[no] = i; update();}
    void setBreaks(int i, int no) {mBrk[no] = i; update();}
    void setPlayedBy(int i, int no) {mAI[no] = i; update();}

protected:
    void update();

private:
    ScoreSprite* mDisplay;

    QString mName[2];
    int mLevel[2];
    int mTurn;
    int mWin[2];
    int mRemis[2];
    int mLoss[2];
    int mBrk[2];
    int mSum[2];
    int mAI[2];
};

#endif 

