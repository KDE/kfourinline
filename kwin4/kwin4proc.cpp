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

#include "kwin4proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <kgamemessage.h>
#include <kdebug.h>

#define MIN_TIME 1  // sec

#define NOOFPLAYER 2/* Zwei Spieler */
#define MAXANZAHL 6 /* Maximal 6 Steine pro Reihe */
#define WIN4 4      /* 4er Reihe hat gewonnen */
#define MAXZUG 42   /* Soviele Zuege moeglich */
#define FELD_OFF 10
#define LOWERT -999999999L
#define SIEG_WERT 9999999L


#define START_REK 1    // (0) 1:Nur Stellungsbewertung bei Level 1
                       //     0:Level 1 schon eine Rekursion

KComputer::KComputer() : QObject(0,0)
{
    InitField();

    const char *s1="7777776666666123456654321123456654321";
    const char *s2="0000000000000000000123456000000123456";

    unsigned int i;
    for (i=0;i<strlen(s1);i++)
      lenofrow[i]=s1[i]-'0';
    for (i=0;i<strlen(s2);i++)
      startofrow[i]=s2[i]-'0';

  connect(&proc,SIGNAL(signalCommand(QDataStream &,int ,int ,int )),
                   this,SLOT(slotCommand(QDataStream & ,int ,int ,int )));
  connect(&proc,SIGNAL(signalInit(QDataStream &,int)),
                   this,SLOT(slotInit(QDataStream & ,int )));
  connect(&proc,SIGNAL(signalTurn(QDataStream &,bool )),
                   this,SLOT(slotTurn(QDataStream & ,bool )));
  fprintf(stderr, "----------------->\nKComputer::Computer\n");
}

void KComputer::slotInit(QDataStream &in,int id)
{
  fprintf(stderr,"----------------->\nKComputer::slotInit\nid:%d\n",id);
  /*
  QByteArray buffer;
  QDataStream out(buffer,IO_WriteOnly);
  int msgid=KGameMessage::IdProcessQuery;
  out << (int)1;
  proc.sendSystemMessage(out,msgid,0);
  */
}

void KComputer::slotTurn(QDataStream &in,bool turn)
{
  QByteArray buffer;
  QDataStream out(buffer,IO_WriteOnly);
  fprintf(stderr,"----------------->\nKComputer::slotTurn\nturn:%d\n",turn);
  if (turn)
  {
    // Create a move
    long value=think(in,out,false);
    int id=KGameMessage::IdPlayerInput;
    proc.sendSystemMessage(out,id,0);
    sendValue(value);
  }
}

void KComputer::sendValue(long value)
{
  Q_INT8 cid=1; // notifies our KGameIO that this is a value message
  int id=KGameMessage::IdProcessQuery;
  QByteArray buffer;
  QDataStream out(buffer,IO_WriteOnly);
  out << cid << value;
  proc.sendSystemMessage(out,id,0);
}

long KComputer::think(QDataStream &in,QDataStream &out,bool hint)
{
  Q_INT32 pl;
  Q_INT32 move;
  Q_INT32 tmp;
  in >> pl ;
  in >> tmp;
  aktzug=tmp;
  in >> tmp;
  // We need all the +1 because the main programm has different defines
  // for the colours. And chaning it here seems not to work....
  amZug=(Farbe)(tmp+1);
  in >> tmp;
  beginner=(Farbe)(tmp+1);
  in >> tmp;
  second=(Farbe)(tmp+1);
  in >> tmp;
  mymaxreklev=tmp;
  fprintf(stderr,"think: pl=%d, aktzug=%d amzug=%d begin=%d second=%d level=%d\n",
                 pl,aktzug,amZug,beginner,second,mymaxreklev);

  InitField();

  // Field as 42 Q_INT8's
  int i,j;
  for (i=0;i<=SIZE_Y;i++)
  {
    for (j=0;j<=SIZE_X;j++)
    {
      Q_INT8 col;
      in >> col;
      Farbe colour;
      if (col<2) colour=(Farbe)(col+1);
      else colour=(Farbe)col;
      DoMove(j,colour,feldmatrix,anzahlmatrix);
    }
  }
  
  for (i=0;i<=SIZE_Y;i++)
  {
    char tstr[1024];
    tstr[0]=0;
    for (j=0;j<=SIZE_X;j++)
    {
      sprintf(tstr+strlen(tstr),"%02d ", feldmatrix[i][j]);
    }
    fprintf(stderr,"%s\n",tstr);
  }

  in >> tmp;
  fprintf(stderr,"CHECKSUM=%ld should be 421256\n",(long)tmp);

  time_t timea,timee;
  timea=time(0);

  int mymove;
  mymove= GetCompMove();

  fprintf(stderr,"Computermove to %d value=%ld\n",mymove,aktwert);

  timee=time(0);
  // Sleep a minimum amount to slow down moves
  if (timee-timea < MIN_TIME) sleep((MIN_TIME-(timee-timea)));


  move=mymove;
  if (hint)
  {
    out << pl << move;
  }
  else
  {
    out << pl << move;
  }
  return aktwert;
}

void KComputer::slotCommand(QDataStream &in,int msgid,int receiver,int sender)
{
  fprintf(stderr,"----------------->\nKComputer::slotCommand\nMsgid:%d\n",msgid);
  QByteArray buffer;
  QDataStream out(buffer,IO_WriteOnly);
  switch(msgid)
  {
    case 2:  // hint
    {
      Q_INT8 cid=2;
      Q_INT32 pl=0;
      Q_INT32 move=3;
      out << cid;
      long value=think(in,out,true);
      out << value;
      int id=KGameMessage::IdProcessQuery;
      proc.sendSystemMessage(out,id,0);
    }
    break;
    default: 
      fprintf(stderr,"KComputer:: unknown command Msgid:%d\n",msgid);
  }
}

/**
 * Computer Routinen
 */
int KComputer::GetCompMove()
{
  int cmove;
  long cmax,wert;
  int x;
  FARBE lfeld[SIZE_Y_ALL+1][SIZE_X+1];
  char lanzahl[SIZE_Y_ALL+1];
  Farbe farbe;


  farbe=amZug;
  cmove=-1; /* Kein Zug */
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
  amZug=farbe; // Wertung changes amZug!
return cmove;
}

long KComputer::Wertung(Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[],int reklev,int zug)
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

long KComputer::Bewertung(Farbe farbe,FARBE feld[][SIZE_X+1])
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
}

Farbe KComputer::GameOver(FARBE feld[][SIZE_X+1],char anzahl[])
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
}

Farbe KComputer::SwitchPlayer(Farbe m_amZug)
{
  if (m_amZug==Niemand)
    m_amZug=amZug;
  if (m_amZug==Rot)
    amZug=Gelb;
  else if (m_amZug==Gelb)
     amZug=Rot;
  else amZug=beginner;
  return amZug;
}

void KComputer::DoMove(char move,Farbe farbe,FARBE feld[][SIZE_X+1],char anzahl[])
{
  int x,i,y;

  if (farbe==Tip || farbe==Niemand) return ;  // no real move
  x=move;
  y=anzahl[6+move];
  feld[y][x]=farbe;

  //if (farbe==Tip || farbe==Niemand) return ;  // no real move

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
}

void KComputer::InitField() {
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

long KComputer::random(long max)
{
  long wert;
  wert=proc.random()->getLong(max);
  return wert;
}

// Main startup
int main(int argc ,char * argv[])
{
  // This is the computer player...it should do the calulation
  // It doesn't do much here
  fprintf(stderr,"Vor KComputer\n");
  fflush(stderr);
  KComputer comp;
  fprintf(stderr,"Vor exec\n");
  // And start the event loop
  comp.proc.exec(argc,argv);
  fprintf(stderr,"nach exec\n");
  return 1;
}                                                                                                                             
#include "kwin4proc.moc"
