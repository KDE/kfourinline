/***************************************************************************
                          kproc4.h  -  description
                             -------------------
    begin                : Sun Apr 9 2000
    copyright            : (C) 2000 by Martin Heni
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

#ifndef KPROC4_H
#define KPROC4_H
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "KInputChildProcess.h"

/**
  *@author Martin Heni
  */

typedef enum e_farbe {Niemand=0,Gelb=1,Rot=2,Tip=3,Rand=4} Farbe;   /* Spielerfarbe */
typedef char FARBE;
typedef enum e_player {Men=0,Computer=1} PLAYER;


#define SIZE_Y_ALL 36
#define SIZE_X 6
#define SIZE_Y 5
#define NOOFPLAYER 2/* Zwei Spieler */
#define MAXANZAHL 6 /* Maximal 6 Steine pro Reihe */
#define WIN4 4      /* 4er Reihe hat gewonnen */
#define MAXZUG 42   /* Soviele Zuege moeglich */
#define FELD_OFF 10
#define LOWERT 	-999999999L
#define SIEG_WERT 9999999L


#define START_REK 1    // (0) 1:Nur Stellungsbewertung bei Level 1
					   //     0:Level 1 schon eine Rekursion

class KProc4 : public KInputChildProcess   {

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

public:
	KProc4();
	~KProc4();
	
  virtual bool ReceiveMsg(KEMessage *msg,int id);


  Farbe SwitchPlayer(Farbe amZug=Niemand);
  Farbe GameOver(FARBE feld[][SIZE_X+1],char anzahl[]);
  void DoMove(char move,Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[]);
  int GetCompMove();
  long Wertung(Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[],int reklev,int zug);
  long Bewertung(Farbe farbe,FARBE feld[][SIZE_X+1]);
  long random(long max);
  void initrandom();
  void InitField();
};

#endif
