/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2007 by Martin Heni
    email                : kde@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _KWIN4PROC_H_
#define _KWIN4PROC_H_

// KDE includes
#include <kgameprocess.h>

// Local includes
#include "kwin4global.h"

// Save storage for color arrays
typedef char FARBE;

#define SIZE_Y_ALL 36
#define SIZE_X 6
#define SIZE_Y 5

class KComputer : public QObject
{

Q_OBJECT

public:
  KComputer();
  // The KGameProcess is the main program and event loop 
  KGameProcess proc;

public slots:
  void slotCommand(QDataStream &, int msgid,int receiver,int sender);
  void slotInit(QDataStream &, int id);
  void slotTurn(QDataStream &, bool turn);

protected:
  void sendValue(long value);
  long random(long max);
  long think(QDataStream &in,QDataStream &out,bool hint);
  
  // Old computer stuff
  COLOUR SwitchPlayer(COLOUR amZug=Nobody);
  COLOUR IsGameOver(FARBE feld[][SIZE_X+1],char anzahl[]);
  void DoMove(char move,COLOUR farbe,FARBE feld[][SIZE_X+1],char anzahl[]);
  int GetCompMove(int maxRecursion);
  long MinMax(COLOUR farbe,FARBE feld[][SIZE_X+1],char anzahl[],int reklev,int zug);
  long PositionEvaluation(COLOUR farbe,FARBE feld[][SIZE_X+1]);
  void InitField();

private:
  /* rows: 0-5  =6 : horiz(i:0-6) */
  /*       6-12 =7 : vert(i:0-5)  */
  /*      13-24 =12: diag-45(i:...) */
  /*      25-36 =12: diag45(i:...) */

  char lenofrow[38];
  char startofrow[38];

  COLOUR beginner,second;  // Welche COLOUR faengt an und zieht nach

  COLOUR amZug;    // wer ist am Zug
  COLOUR winner;   // who won thee game
  FARBE feldmatrix[SIZE_Y_ALL+1][SIZE_X+1];
  char anzahlmatrix[SIZE_Y_ALL+1];

  int aktzug;           // welcher Zug ist getade gemacht 0..42
  int mymaxreklev;      // maximale Rekursion
  long aktwert;         // Stellungsbewertung
  int pos_evaluations;  // Amount of positions evaluated
  float calcPosPerMS;   // How many pos calculated per [ms]

};

#endif // _KWIN4PROC_H_

