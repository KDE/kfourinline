/***************************************************************************
                          kproc4.cpp  -  description
                             -------------------
    begin                : Sun Apr 9 2000
    copyright            : (C) 2000 by Martin Heni
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <QDataStream>
#include <QByteArray>
#include <QBuffer>
#include <QTime>
#include <QMutex>
#include <QWaitCondition>

#include <kgamemessage.h>
#include <kdebug.h>

#include "kwin4proc.h"

#define MIN_TIME        1000  // min time in milli sec for move
#define MAXANZAHL          6 /* Max 6 pieces per column */
#define WIN4               4      /* 4er Reihe hat gewonnen */
#define MAXZUG            42   /* Soviele Zuege moeglich */
#define FELD_OFF          10
#define LOWERT    -999999999L  // Init variables
#define SIEG_WERT    9999999L  // Win or loss value


// Constructor
KComputer::KComputer()
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
  calcPosPerMS = -1.0; // Unknown yet
}


// Recevied init command
void KComputer::slotInit(QDataStream & /*in */,int /*id*/)
{
//  fprintf(stderr,"----------------->\nKComputer::slotInit\nid:%d\n",id);
  /*
  QByteArray buffer;
  QDataStream out(buffer,QIODevice::WriteOnly);
  int msgid=KGameMessage::IdProcessQuery;
  out << (int)1;
  proc.sendSystemMessage(out,msgid,0);
  */
}


// Received turn command
void KComputer::slotTurn(QDataStream &in,bool turn)
{
  QByteArray buffer;
  QDataStream out(&buffer,QIODevice::WriteOnly);
  fprintf(stderr,"----------------->\nKComputer::slotTurn\nturn:%d\n",turn);
  if (turn)
  {
    // Create a move
    qint32 value = think(in,out,false);
    int id       = KGameMessage::IdPlayerInput;
    proc.sendSystemMessage(out,id,0);
    sendValue(value);
    fprintf(stderr,"  KComputer::sendValue in turn:%d\n",value);
  }
  fflush(stderr);
}


// Send position value back to main program
void KComputer::sendValue(long value)
{
  qint8 cid = 1; // notifies our KGameIO that this is a value message
  int id    = KGameMessage::IdProcessQuery;
  QByteArray buffer;
  QDataStream out(&buffer,QIODevice::WriteOnly);
  out << cid << ( qint32 )value;
  proc.sendSystemMessage(out,id,0);
  fprintf(stderr,"  KComputer::sendValue in sendValue:%ld\n",(long)value);
  fflush(stderr);
}


// Received a command from the main program
void KComputer::slotCommand(QDataStream &in,int msgid,int /*receiver*/,int /*sender*/)
{
  fprintf(stderr,"----------------->\nKComputer::slotCommand\nMsgid:%d\n",msgid);
  QByteArray buffer;
  QDataStream out(&buffer,QIODevice::WriteOnly);
  switch(msgid)
  {
    case 2:  // hint
    {
      qint8 cid   = 2;
      qint32 recv = 0;
      out << cid;
      long value  = think(in,out,true);
      out << ( qint32 )value;
      int id = KGameMessage::IdProcessQuery;
      proc.sendSystemMessage(out, id, recv);
    }
    break;
    default:
      fprintf(stderr,"KComputer:: unknown command Msgid:%d\n",msgid);
  }
}

// Think up a move (plus reading data from stream)
long KComputer::think(QDataStream& in, QDataStream& out, bool /*hint*/)
{
  qint32 pl;
  qint32 move;
  qint32 tmp;
  in >> pl ;
  in >> tmp;
  aktzug=tmp;
  in >> tmp;
  // We need all the +1 because the main program has different defines
  // for the colors. And changing it here seems not to work....
  amZug=(COLOUR)(tmp);
  in >> tmp;
  beginner=(COLOUR)(tmp);
  in >> tmp;
  second=(COLOUR)(tmp);
  in >> tmp;
  mymaxreklev=tmp;
  fprintf(stderr,"think: pl=%d, aktzug=%d amzug=%d begin=%d second=%d level=%d\n",
                 pl,aktzug,amZug,beginner,second,mymaxreklev);

  InitField();

  // Field as 42 qint8's
  int i,j;
  for (i=0;i<=SIZE_Y;i++)
  {
    for (j=0;j<=SIZE_X;j++)
    {
      qint8 col;
      in >> col;
      DoMove(j, (COLOUR)col, feldmatrix, anzahlmatrix);
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

  // Estimated number of positions to evaluate (MAX)
  int estimated = 0;
  for (int i=1; i<= mymaxreklev; i++)
  {
    estimated += int(pow(7.,i));
  }

  // Measure time of move and positions evaluated
  QTime timer;
  timer.start();
  pos_evaluations = 0;

  // Get move
  int mymove;
  int gameOver = 0;
  int extraRecurstion = 0;

  // Loop movement if not many positions are evaulated (end game)
  // to look ahead a bit more
  do
  {
    // Actually calculate move
    mymove = GetCompMove(mymaxreklev + extraRecurstion);

    // Do not recalcuate for (nearly finished) games
    if (aktwert >= SIEG_WERT*0.95 || aktwert <= -SIEG_WERT *0.95) gameOver = 1;
    extraRecurstion++;
  }while(4*pos_evaluations <= estimated && !gameOver);

  // Measure elapsed time
  int elapsed = timer.elapsed();
  if (calcPosPerMS <= 0.0) calcPosPerMS = (float)pos_evaluations/(float)elapsed;

  // Debug
  fprintf(stderr,"Computer sends move; to %d value=%ld\n",mymove,aktwert);
  fprintf(stderr,"AI MOVE level %d took time= %d ms and evaluations=%d estimated=%d pos/ms=%f\n",
             mymaxreklev, elapsed, pos_evaluations,estimated,calcPosPerMS);

  // Sleep a minimum amount to slow down moves
  if (elapsed < MIN_TIME) 
  {
    // usleep(1000*(MIN_TIME-elapsed));
    QMutex mutex;
    QWaitCondition cond;
    mutex.lock();
    cond.wait(&mutex, MIN_TIME-elapsed);
    
    elapsed = timer.elapsed();
    fprintf(stderr,"AI move after sleep %d\n",elapsed);
  }


  // Send out move
  move = mymove;
  out << pl << move;
  return aktwert;
}


// Computer AI algorithms
int KComputer::GetCompMove(int maxRecursion)
{
  int cmove;
  long cmax,wert;
  int x;
  FARBE lfeld[SIZE_Y_ALL+1][SIZE_X+1];
  char lanzahl[SIZE_Y_ALL+1];
  COLOUR farbe;


  farbe = amZug;
  cmove = -1; /* Kein Zug */
  cmax  = LOWERT;
  for (x=0;x<=SIZE_X;x++)
  {
    if (anzahlmatrix[6+x]>=MAXANZAHL) continue;
    memcpy(lanzahl,anzahlmatrix,sizeof(lanzahl));
    memcpy(lfeld,feldmatrix,sizeof(lfeld));

    DoMove(x,farbe,lfeld,lanzahl);
    wert=MinMax(farbe,lfeld,lanzahl,maxRecursion,aktzug+1);

    if (wert>=cmax)
    {
     cmax  = wert;
     cmove = x;
     if (cmax>=SIEG_WERT) break;
    }
  }/*next x*/
  aktwert = cmax;
  amZug   = farbe; // Wertung changes amZug!
  return cmove;
}


// Min-Max AI algorithm
long KComputer::MinMax(COLOUR farbe,FARBE feld[][SIZE_X+1], char anzahl[], int reklev, int zug)
{
  static long gaus[]={10,50,300,500,300,50,10};
  FARBE lfeld[SIZE_Y_ALL+1][SIZE_X+1];
  char lanzahl[SIZE_Y_ALL+1];
  long max,wert;
  int x;
  COLOUR winner;

  pos_evaluations++;

  winner=IsGameOver(feld,anzahl);
  if (winner!=Nobody)
  {
     if (winner==farbe) return(SIEG_WERT);
     else return(-SIEG_WERT);
  }
  if (zug>=MAXZUG) return(0); /* Remis */
  if (reklev <= 1) return PositionEvaluation(farbe,feld);


  farbe = SwitchPlayer(farbe);
  max   = LOWERT;
  for (x=0;x<=SIZE_X;x++)
  {
    if (anzahl[6+x]>=MAXANZAHL) continue;
    memcpy(lfeld,feld,sizeof(lfeld));
    memcpy(lanzahl,anzahl,sizeof(lanzahl));
    DoMove(x,farbe,lfeld,lanzahl);
    wert=MinMax(farbe,lfeld,lanzahl,reklev-1,zug+1)+gaus[x];
    if (wert>=max)
    {
      max=wert;
      if (max>=SIEG_WERT) break;
    }
   }//next x
   return(-max);
}// end MinMax


// Position evaluation
long KComputer::PositionEvaluation(COLOUR farbe, FARBE feld[][SIZE_X+1])
{
/* Abstand:              0    1    2    3    4    5    */
static long myWERT[]={2200,600, 300,  75,  20,   0};
//static long myWERT[]={0,0,0,0,0,0};
/* Wieviele von COLOUR:       0     1     2     3    4 */
static long steinWERT[4][5]=
{
        {     0,  500L, 40000L,200000L,SIEG_WERT}, // Leerfelder=0
        {     0,  500L,  8000L, 40000L,SIEG_WERT}, //           =1
        {     0,   00L,  4000L, 25000L,SIEG_WERT}, //           =2
        {     0,   00L,  2000L, 12500L,SIEG_WERT}, //           =3
};
  long gelb_wert,rot_wert,wert;
  int cntcol,cnt;
  COLOUR color;
  FARBE field;
  int y,i,j;

  gelb_wert = random(2500);
  rot_wert  = random(2500);
  for (y=0;y<=SIZE_Y_ALL;y++)
  {
    if (lenofrow[y]<WIN4) continue;
    for (i=0;i<=(lenofrow[y]-WIN4);i++)
    {
      color  = Nobody;
      wert   = 0;
      cntcol = 0;
      cnt    = 0;
      for (j=0;j<WIN4;j++)
      {
        field=feld[y][i+j+startofrow[y]];
        if ((COLOUR)field==Red)
        {
          if (color==Yellow) {color=Nobody;break;}
          cntcol++;
          color=Red;
        }
        else if ((COLOUR)field==Yellow)
        {
          if (color==Red) {color=Nobody;break;}
          cntcol++;
          color=Yellow;
        }
        else
        {
          cnt+=field-FELD_OFF;
          wert+=myWERT[field-FELD_OFF];
        }
      }/*next j */
      if (cnt>3) cnt=3;
      if (color==Red) rot_wert+=(wert+steinWERT[cnt][cntcol]);
      else if (color==Yellow) gelb_wert+=(wert+steinWERT[cnt][cntcol]);
    }/*next i*/
  }/*next y*/
  if (farbe==Red) wert=rot_wert-gelb_wert;
  else wert=gelb_wert-rot_wert;
  return(wert);
}


// Check for game over 
COLOUR KComputer::IsGameOver(FARBE feld[][SIZE_X+1],char anzahl[])
{
  COLOUR thiscolor,field;
  int x,y,cnt;
  for (y=0;y<=SIZE_Y_ALL;y++)
  {
     if (anzahl[y]<WIN4) continue;
     if (lenofrow[y]<WIN4) continue;

     cnt       = 0;
     thiscolor = Nobody;
     for (x=0;x<lenofrow[y];x++)
     {
       field = (COLOUR)feld[y][x+startofrow[y]];
       if (field == thiscolor) cnt++;
       else {cnt=1; thiscolor=field;}
       if ( (cnt>=WIN4)&&( (thiscolor==Yellow)||(thiscolor==Red) ) ) return(thiscolor);
     }// next x
  }// next y
  return(Nobody);
}


// Swap player color
COLOUR KComputer::SwitchPlayer(COLOUR m_amZug)
{
  if (m_amZug==Nobody)
    m_amZug=amZug;
  if (m_amZug==Red)
    amZug=Yellow;
  else if (m_amZug==Yellow)
     amZug=Red;
  else amZug=beginner;
  return amZug;
}


// Execute move on given board
void KComputer::DoMove(char move,COLOUR farbe,FARBE feld[][SIZE_X+1],char anzahl[])
{
  int x,i,y;

  if (farbe==Tip || farbe==Nobody) return ;  // no real move
  x          = move;
  y          = anzahl[6+move];
  feld[y][x] = farbe;

  feld[6+x][y]    = farbe;
  feld[13+x+y][x] = farbe;
  feld[30+x-y][x] = farbe;
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


// Clear board and diagonal matrix
void KComputer::InitField() 
{
  int x,y;

  for (y=0;y<=SIZE_Y_ALL;y++)
  {
    anzahlmatrix[y]=0;
  }

  for (y=0;y<=SIZE_Y;y++)
  {
    for (x=0;x<=SIZE_X;x++)
    {
      feldmatrix[y][x]      = (FARBE)(y+FELD_OFF);
      feldmatrix[6+x][y]    = (FARBE)(y+FELD_OFF);
      feldmatrix[13+x+y][x] = (FARBE)(y+FELD_OFF);
      feldmatrix[30+x-y][x] = (FARBE)(y+FELD_OFF);
    } // next x
  }// next y 
}


// Retreive random number 0..max
long KComputer::random(long max)
{
  long wert;
  wert=proc.random()->getLong(max);
  return wert;
}

// Main startup
int main(int argc ,char * argv[])
{
  // This is the computer player...it should do the calculation
  // It doesn't do much here
  fprintf(stderr,"Vor KComputer START *****************************************************\n");
  fflush(stderr);
  KComputer comp;
  fprintf(stderr,"Vor exec *************************\n");
  fflush(stderr);
  // And start the event loop
  comp.proc.exec(argc,argv);
  fprintf(stderr,"nach KComputer exec\n");
  return 1;
}
#include "kwin4proc.moc"
