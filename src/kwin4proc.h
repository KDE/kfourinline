/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2001 by Martin Heni
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

#ifndef _KWIN4PROC_H_
#define _KWIN4PROC_H_

#include <kgameprocess.h>

// TODO match up with what is in kwin4doc.h
typedef enum e_farbe {Niemand=0,Gelb=1,Rot=2,Tip=3,Rand=4} Farbe;
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
  Farbe SwitchPlayer(Farbe amZug=Niemand);
  Farbe GameOver(FARBE feld[][SIZE_X+1],char anzahl[]);
  void DoMove(char move,Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[]);
  int GetCompMove();
  long Wertung(Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[],int reklev,int zug);
  long Bewertung(Farbe farbe,FARBE feld[][SIZE_X+1]);
  void InitField();

private:
  /* rows: 0-5  =6 : horiz(i:0-6) */
  /*       6-12 =7 : vert(i:0-5)  */
  /*      13-24 =12: diag-45(i:...) */
  /*      25-36 =12: diag45(i:...) */

  char lenofrow[38];
  char startofrow[38];

	Farbe beginner,second;	// Welche Farbe faengt an und zieht nach

	Farbe amZug;	// wer ist am Zug
	Farbe winner;	// who won thee game
  FARBE feldmatrix[SIZE_Y_ALL+1][SIZE_X+1];
	char anzahlmatrix[SIZE_Y_ALL+1];

  int aktzug;			// welcher Zug ist getade gemacht 0..42
	int mymaxreklev;	// maximale Rekursion
	long aktwert;		// Stellungsbewertung

};

#endif // _KWIN4PROC_H_

