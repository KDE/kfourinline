/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 1995-2007 Martin Heni <kde@heni-online.de>

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

#ifndef KWIN4_PLAYER_H
#define KWIN4_PLAYER_H

// KDE includes
#include <kconfiggroup.h>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kplayer.h>
#include <libkdegamesprivate/kgame/kgameproperty.h>

// Local includes
#include "score.h"


/** A player for the KWin4 game. A player holds all important player data,
  * in particular the long time status and score information. Additionally,
  * via the KGame framework it supports various methods of input like mouse,
  * keyboard, AI or even network.
  */
class KWin4Player : public KPlayer
{
  Q_OBJECT
	  
  public:
    /** Construct a player object,.
      */
    KWin4Player();
    ~KWin4Player();
    
    /** Runtime identification for the player type (KGame). Only one player
      * _type_ for kwin4.
      * @return The player type = 1.
      */
    int rtti() const {return 1;}
    
    /** Assign the score object which allows to display player properties in 
      * the GUI.
      * @param s The score object
      */
    void setStatus(Score *s) {mStatus=s;}
    
    /** Retrieve the score object from the player.
      * @return The score object.
      */
    Score* status(){return mStatus;}
    
    /** Read the player data (status, score) from the configuration file.
      * @param config The configuration group
      */
    void readConfig(KConfigGroup& config);
    
    /** Write the player data (status, score) to the configuration file.
      * @param config The configuration group
      */
    void writeConfig(KConfigGroup& config);
    
    /** Increase the number of wins for this player.
      */
    void incWin();
    
    /** Increase the number of draws for this player.
      */
    void incRemis();
    
    /** Increase the number of losses for this player.
      */
    void incLost();
    
    /** Increase the number of aborted games for this player.
      */
    void incBrk();
    
    /** Retrieve the number of wins.
      */
    int win() {return mAllWin.value();}
    
    /** Retrieve the number of losses.
      */
    int lost() {return mAllLost.value();}
    
    /** Retrieve the number of aborted games.
      */
    int brk() {return mAllBrk.value();}
    
    /** Retriev the number of drawn games.
      */
    int remis() {return mAllRemis.value();}
    
    /** Clear the score of the player. 
      * @param all True: Clear also long time score. False: Clear only session score.
      */
    void resetStats(bool all=true);

  protected slots:
    /** KGame method which is called when any KGamePropery changes for this
      * player.
      * @param prop    The property
      * @param player  This player
      */   
    void slotPlayerPropertyChanged(KGamePropertyBase *prop,KPlayer *player);

  private:
    // One session wins
    KGamePropertyInt mWin;

    // One session draws
    KGamePropertyInt mRemis;

    // One session losses
    KGamePropertyInt mLost;

    // One session aborted games
    KGamePropertyInt mBrk;

    // All time wins
    KGamePropertyInt mAllWin;

    // All time draws
    KGamePropertyInt mAllRemis;

    // All time losses
    KGamePropertyInt mAllLost;

    // All time aborted games
    KGamePropertyInt mAllBrk;

    // Status object: Connects player to GUI widget
    Score *mStatus;
};

#endif // KWIN4_PLAYER_H

