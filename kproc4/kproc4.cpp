/***************************************************************************
                          kproc4.cpp  -  description
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
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kproc4.h"

#define MIN_TIME 1  // sec

KProc4::KProc4()
  : KInputChildProcess(4096)
{
size_t i;

    initrandom();
    InitField();

    const char *s1="7777776666666123456654321123456654321";
    const char *s2="0000000000000000000123456000000123456";

    for (i=0;i<strlen(s1);i++)
      lenofrow[i]=s1[i]-'0';
    for (i=0;i<strlen(s2);i++)
      startofrow[i]=s2[i]-'0';

}

KProc4::~KProc4(){
}

bool KProc4::ReceiveMsg(KEMessage* msg,int id)
{
//  float x;

  // end of process
  if (msg->HasKey(QCString("Terminate")))
  {
    Terminate();
  }
  // Init of process
  if (msg->HasKey(QCString("Init")))
  {
    // No init necessary
  }
  if (msg->HasKey(QCString("Move")))
  {

    InitField();
    short sh1;
    short hint;
    short oldm;
    msg->GetData(QCString("Aktzug"),sh1);
    aktzug=(int)sh1;
    msg->GetData(QCString("Amzug"),sh1);
    amZug=(Farbe)sh1;
    msg->GetData(QCString("Beginner"),sh1);
    beginner=(Farbe)sh1;
    msg->GetData(QCString("Second"),sh1);
    second=(Farbe)sh1;
    msg->GetData(QCString("Level"),sh1);
    mymaxreklev=(int)sh1;
    msg->GetData(QCString("Hint"),hint);
    msg->GetData(QCString("Hintmove"),oldm);

    QString s;
    char *fl;
    int i,j,size;

    for (i=0;i<=SIZE_Y;i++)
    {
      s.sprintf("Row%d",i);
      /** Coulour's of row i */
      msg->GetData(s,fl,size);
      // if size!=7 we are fucked up...


      for (j=0;j<=SIZE_X;j++)
      {
         if (fl[j]!=0)
         {
            DoMove(j,(Farbe)fl[j],feldmatrix,anzahlmatrix);
         }
      }
    }

    msg->RemoveAll();

    time_t timea,timee;
    timea=time(0);

    int mymove;
    mymove= GetCompMove();

    timee=time(0);
    // Sleep a minimum amount to slow down moves
    if (timee-timea < MIN_TIME) sleep((MIN_TIME-(timee-timea)));


    if (hint) msg->AddData(QCString("Hint"),(short)mymove);
    else msg->AddData(QCString("Move"),(short)mymove);
    msg->AddData(QCString("Hintmove"),oldm);
    msg->AddData(QCString("Score"),aktwert);
    s.sprintf("Move:%d Score=%ld\n",mymove,aktwert);
//    msg->AddData("Message",(char *)s,s.length());
//    msg->AddData("KLogSendMsg","kproc4.log");
    if (!IsTerminated()) SendMsg(msg);
  }

  return true;
}


/* --------------------------------------------------------------------------- */
/* Computer Routinen														   */
/* --------------------------------------------------------------------------- */
int KProc4::GetCompMove()
{
int cmove;
long cmax,wert;
int x;
FARBE lfeld[SIZE_Y_ALL+1][SIZE_X+1];
char lanzahl[SIZE_Y_ALL+1];
Farbe farbe;


  farbe=amZug;
  cmove=-1;		/* Kein Zug */
  cmax=LOWERT;
  for (x=0;x<=SIZE_X;x++)
  {
	 if (anzahlmatrix[6+x]>=MAXANZAHL) continue;
	 memcpy(lanzahl,anzahlmatrix,sizeof(lanzahl));
	 memcpy(lfeld,feldmatrix,sizeof(lfeld));

	 DoMove(x,farbe,lfeld,lanzahl);
	 wert=Wertung(farbe,lfeld,lanzahl,START_REK,aktzug+1);
	 if (wert>=cmax)
	 {
		cmax=wert;
		cmove=x;
    if (cmax>=SIEG_WERT) break;
	 }
  }/*next x*/
  aktwert=cmax;
  amZug=farbe;	// Wertung changes amZug !!!
return cmove;
}/* end getCompMove */

/* ---------------------------------------------------------------- */
long KProc4::Wertung(Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[],int reklev,int zug)
{
static long gaus[]={10,50,300,500,300,50,10};
FARBE lfeld[SIZE_Y_ALL+1][SIZE_X+1];
char lanzahl[SIZE_Y_ALL+1];
long max,wert;
int x;
Farbe winner;
	


   winner=GameOver(feld,anzahl);
   if (winner!=Niemand)
   {
		if (winner==farbe) return(SIEG_WERT);
        else return(-SIEG_WERT);
   }
   if (zug>=MAXZUG) return(0); /* Remis */
   if (reklev>=mymaxreklev)  return Bewertung(farbe,feld);


   farbe=SwitchPlayer(farbe);
   max=LOWERT;
   for (x=0;x<=SIZE_X;x++)
   {
	  if (anzahl[6+x]>=MAXANZAHL) continue;
	  memcpy(lfeld,feld,sizeof(lfeld));
    memcpy(lanzahl,anzahl,sizeof(lanzahl));
	  DoMove(x,farbe,lfeld,lanzahl);
	  wert=Wertung(farbe,lfeld,lanzahl,reklev+1,zug+1)+gaus[x];
	  if (wert>=max)
	  {
		max=wert;
		if (max>=SIEG_WERT) break;
	  }
    }/*next x*/
    return(-max);
}/*end wertung*/
/* ---------------------------------------------------------------- */
long KProc4::Bewertung(Farbe farbe,FARBE feld[][SIZE_X+1])
{
/* Abstand:              0    1    2    3    4    5    */
static long myWERT[]={2200,600, 300,  75,  20,   0};
//static long myWERT[]={0,0,0,0,0,0};
/* Wieviele von Farbe:       0     1     2     3    4 */
static long steinWERT[4][5]=
{
	{     0,  500L, 40000L,200000L,SIEG_WERT}, // Leerfelder=0
	{     0,  500L,  8000L, 40000L,SIEG_WERT}, //           =1
	{     0,   00L,  4000L, 25000L,SIEG_WERT}, //			=2
	{     0,   00L,  2000L, 12500L,SIEG_WERT}, //           =3
};
long gelb_wert,rot_wert,wert;
int cntcol,cnt;
Farbe color;
FARBE field;
int y,i,j;
	gelb_wert=random(2500);
	rot_wert=random(2500);
	for (y=0;y<=SIZE_Y_ALL;y++)
	{
		if (lenofrow[y]<WIN4) continue;
		for (i=0;i<=(lenofrow[y]-WIN4);i++)
		{
			color=Niemand;
            wert=0;
			cntcol=0;
			cnt=0;
			for (j=0;j<WIN4;j++)
			{
				field=feld[y][i+j+startofrow[y]];
				if ((Farbe)field==Rot)
				{
					if (color==Gelb) {color=Niemand;break;}
					cntcol++;
          color=Rot;
				}
				else if ((Farbe)field==Gelb)
				{
					if (color==Rot) {color=Niemand;break;}
					cntcol++;
          color=Gelb;
				}
				else
				{
					cnt+=field-FELD_OFF;
					wert+=myWERT[field-FELD_OFF];
				}
			}/*next j */
      if (cnt>3) cnt=3;
			if (color==Rot) rot_wert+=(wert+steinWERT[cnt][cntcol]);
			else if (color==Gelb) gelb_wert+=(wert+steinWERT[cnt][cntcol]);
    }/*next i*/
	}/*next y*/
	if (farbe==Rot) wert=rot_wert-gelb_wert;
	else wert=gelb_wert-rot_wert;
return(wert);
}/* end bewertung */
/* ---------------------------------------------------------------- */

long KProc4::random(long max)
{
double value;
int r;
  r=rand();
  value=(double)((double)r*(double)max)/(double)RAND_MAX;
  return (long)value;
}

void KProc4::initrandom()
{
	srand( (unsigned)time( NULL ) ); // randomize
}




Farbe KProc4::GameOver(FARBE feld[][SIZE_X+1],char anzahl[])
{
Farbe thiscolor,field;
int x,y,cnt;
  for (y=0;y<=SIZE_Y_ALL;y++)
  {
	 if (anzahl[y]<WIN4) continue;
	 if ( lenofrow[y]<WIN4 ) continue;
	 cnt=0;
	 thiscolor=Niemand;
	 for (x=0;x<lenofrow[y];x++)
	 {
	   field=(Farbe)feld[y][x+startofrow[y]];
       if (field==thiscolor) cnt++;
       else {cnt=1;thiscolor=field;}
	   if ( (cnt>=WIN4)&&( (thiscolor==Gelb)||(thiscolor==Rot) ) ) return(thiscolor);
     }/*next x */
  }/*next y*/
return(Niemand);
}/* end gameOver */

Farbe KProc4::SwitchPlayer(Farbe m_amZug)
{
	if (m_amZug==Niemand) m_amZug=amZug;
	if (m_amZug==Rot) amZug=Gelb;
	else if (m_amZug==Gelb) amZug=Rot;
    else amZug=beginner;
	return amZug;
}

void KProc4::DoMove(char move,Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[])
{
int x,i,y;

  x=move;
  y=anzahl[6+move];
  feld[y][x]=farbe;

  if (farbe==Tip || farbe==Niemand) return ;  // no real move

  feld[6+x][y]=farbe;
  feld[13+x+y][x]=farbe;
  feld[30+x-y][x]=farbe;
  anzahl[y]++;
  anzahl[6+x]++;
  anzahl[13+x+y]++;
  anzahl[30+x-y]++;
  for (i=y+1;i<=SIZE_Y;i++)
  {
	   feld[i][x]--;
	   feld[6+x][i]--;
	   feld[13+x+i][x]--;
	   feld[30+x-i][x]--;
  }
}/* end doMove */

void KProc4::InitField()
{
int x,y;
	for (y=0;y<=SIZE_Y_ALL;y++)
		anzahlmatrix[y]=0;
	for (y=0;y<=SIZE_Y;y++)
    {
		for (x=0;x<=SIZE_X;x++)
		{
			feldmatrix[y][x]=(FARBE)(y+FELD_OFF);
			feldmatrix[6+x][y]=(FARBE)(y+FELD_OFF);
			feldmatrix[13+x+y][x]=(FARBE)(y+FELD_OFF);
			feldmatrix[30+x-y][x]=(FARBE)(y+FELD_OFF);
		}
    }/* next y */

}
