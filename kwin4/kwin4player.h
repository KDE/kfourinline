/***************************************************************************
                          Kwin4Player  
                          -------------------
    begin                : August 2001
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

#ifndef KWIN4PLAYER_H
#define KWIN4PLAYER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kplayer.h>

class StatusWidget;
class KConfig;

class Kwin4Player : public KPlayer
{
  Q_OBJECT

  signals:
	  
  public:
    Kwin4Player();
    int rtti() const {return 1;}
    void setWidget(StatusWidget *w) {sWidget=w;}
    void readConfig(KConfig *config);
    void writeConfig(KConfig *config);
    void incWin();
    void incRemis();
    void incLost();
    void incBrk();
    int win() {return mAllWin.value();}
    int lost() {return mAllLost.value();}
    int brk() {return mAllBrk.value();}
    int remis() {return mAllRemis.value();}
    void resetStats(bool all=true);

   protected slots:
    void slotPlayerPropertyChanged(KGamePropertyBase *prop,KPlayer *player);

   private:
    // One session
    KGamePropertyInt mWin;
    KGamePropertyInt mRemis;
    KGamePropertyInt mLost;
    KGamePropertyInt mBrk;

    // all time
    KGamePropertyInt mAllWin;
    KGamePropertyInt mAllRemis;
    KGamePropertyInt mAllLost;
    KGamePropertyInt mAllBrk;

    StatusWidget *sWidget;
};

#endif // KWIN4PLAYER_H

