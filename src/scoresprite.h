/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SCORE_SPRITE_H
#define SCORE_SPRITE_H

// own
#include "thememanager.h"
#include "pixmapsprite.h"
// Qt
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>


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
    void advance(int phase) override;

    /** Retrieve the type of QGI. This item is UserType+10
     *  @return The type of item.
     */
    int type() const override {return QGraphicsItem::UserType+10;}

    /** Main theme change function. On call of this the item needs to redraw and
      * resize.
      */
    void changeTheme() override;

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
