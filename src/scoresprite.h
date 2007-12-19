#ifndef SCORE_SPRITE_H
#define SCORE_SPRITE_H
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
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>


// Local includes
#include "thememanager.h"
#include "pixmapsprite.h"


/** The sprite for a score board on the canvas.
 */
class ScoreSprite : public PixmapSprite
{

  public:
    /** Constructor for the score sprite.
      * @param id              The theme id
      * @param theme           The theme manager
      * @param no              A used defined number (unused)
      * @param scene           The graphics scene
      */
    ScoreSprite(const QString &id, ThemeManager* theme, int no, QGraphicsScene* scene);
    
    /** Destructor 
      */
    ~ScoreSprite();

    /** Standard QGI advance function.
     *  @param phase The advance phase
     */
    virtual void advance(int phase);

    /** Retrieve the type of QGI. This item is UserType+10
     *  @return The type of item.
     */
    virtual int type() const {return QGraphicsItem::UserType+10;}

    /** Main theme change function. On call of this the item needs to redraw and
      * resize.
      */
    virtual void changeTheme();

    /** Store the level of the AI.
      * @param level  The level
      * @param no     The player number [0,1]
      */ 
    void setLevel(int level, int no);
    
    /** Store the player name.
      * @param s  The name
      * @param no The player number [0,1]
      */ 
    void setPlayerName(const QString &s,int no);
    
    /** Store the amounts of wins for a player.
      * @param s  The amount of wins
      * @param no The player number [0,1]
      */ 
    void setWon(const QString &s,int no);

    /** Store the amounts of draws for a player.
      * @param s  The amount of draws
      * @param no The player number [0,1]
      */ 
    void setDraw(const QString &s,int no);
    
    /** Store the amounts of losses for a player.
      * @param s  The amount of losses
      * @param no The player number [0,1]
      */ 
    void setLoss(const QString &s,int no);

    /** Store the amounts of aborted games for a player.
      * @param s  The amount of aborted games
      * @param no The player number [0,1]
      */ 
    void setBreak(const QString &s,int no);
    
     /** Define who's turn it is next
      * @param no The next player number [0,1]
      */    
    void setTurn(int no);
    
    /** Store input device for a player.
      * @param device  The device number [0-3]
      * @param no      The player number [0,1]
      */ 
    void setInput(int device, int no);

  private:
    // Text for won
    QGraphicsTextItem* mWon[2];

    // Text for draw
    QGraphicsTextItem* mDraw[2];

    // Text for loss
    QGraphicsTextItem* mLoss[2];

    // Text for aborted games
    QGraphicsTextItem* mBreak[2];

    // Text for name of player
    QGraphicsTextItem* mName[2];

    // Sprite for the input device
    PixmapSprite* mInput[2];

    // Frame  number of the input device sprite
    int mInputFrame[2];

    // Whose' turn or -1 for nobody
    int mTurn;

};

#endif
