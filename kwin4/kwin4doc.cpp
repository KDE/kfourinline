/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Sun Mar 26 12:50:12 CEST 2000
    copyright            : (C) |1995-2000 by Martin Heni
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

#include <stdlib.h>
#include <time.h>

// include files for Qt
#include <qdir.h>
#include <qfileinfo.h>
#include <qwidget.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kstddirs.h>

// application specific includes
#include "kwin4doc.h"
#include "kwin4.h"
#include "kwin4view.h"
#include "geom.h"

QList<Kwin4View> *Kwin4Doc::pViewList = 0L;

Kwin4Doc::Kwin4Doc(QWidget *parent, const char *name) : QObject(parent, name)
{
  if(!pViewList)
  {
    pViewList = new QList<Kwin4View>();
  }

  pViewList->setAutoDelete(true);

  // Game initialization
  fields=new FARBE[42];
  field_filled=new int[7];
  history=new int[43];

  ResetGame();

  // game startup parameter
  Colour[0]=Gelb;
  Colour[1]=Rot;
  amzug=Gelb;

  player[Gelb]=Men;
  player[Rot]=Computer;

  // calculation
  maxreklev=3;

  ResetStat();
  // default names
  Names[Gelb]="1";
  Names[Rot]="2";

  intro=1;

  time_t tim;
  tim=time(0);
  srand((unsigned int)tim);
  docid=rand();

  is_server=true;

  delpath=1;
  picpath="/tmp";
  procfile="kproc4";

}

Kwin4Doc::~Kwin4Doc()
{
  delete fields;
  delete field_filled;
  delete history;
}

void Kwin4Doc::addView(Kwin4View *view)
{
  pViewList->append(view);
}

void Kwin4Doc::removeView(Kwin4View *view)
{
  pViewList->remove(view);
}

void Kwin4Doc::setAbsFilePath(const QString &filename)
{
  absFilePath=filename;
}

const QString &Kwin4Doc::getAbsFilePath() const
{
  return absFilePath;
}

void Kwin4Doc::setTitle(const QString &_t)
{

//  title=_t;
  title="";
}

const QString &Kwin4Doc::getTitle() const
{
  return title;
}

void Kwin4Doc::slotUpdateAllViews(Kwin4View *sender)
{
  Kwin4View *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender)
        w->repaint();
    }
  }

}

bool Kwin4Doc::saveModified()
{
/*
  if(!intro)
  {
    Kwin4App *win=(Kwin4App *) parent();
    int want_save = KMessageBox::warningYesNo(win,
                                i18n("Do you really want to quit?"),
                                TITLE);

    if (want_save==KMessageBox::No) return false;
  }
  */
  return true;
}

void Kwin4Doc::closeDocument()
{
  deleteContents();
}

bool Kwin4Doc::newDocument(KConfig *config,QString path)
{
  int res;
  modified=false;
  absFilePath=QDir::homeDirPath();
//  title=i18n("Untitled");
  if (global_debug>1) kdDebug() << "path=" << path << endl;
  res=LoadBitmaps(path);
  if (res==0) return false;
  return true;
}

bool Kwin4Doc::openDocument(const QString &filename, const char *format /*=0*/)
{
  QFileInfo fileInfo(filename);
  title=fileInfo.fileName();
  absFilePath=fileInfo.absFilePath();	
  /////////////////////////////////////////////////
  // TODO: Add your document opening code here
  /////////////////////////////////////////////////
	
  modified=false;
  return true;
}

bool Kwin4Doc::saveDocument(const QString &filename, const char *format /*=0*/)
{
  /////////////////////////////////////////////////
  // TODO: Add your document saving code here
  /////////////////////////////////////////////////

  modified=false;
  return true;
}

void Kwin4Doc::deleteContents()
{
  /////////////////////////////////////////////////
  // TODO: Add implementation to delete the document contents
  /////////////////////////////////////////////////

}



/** Returns colour */
FARBE Kwin4Doc::QueryColour(int x,int y){
  return fields[x+y*geom.field_mx];
}
/** Set the colour */
void Kwin4Doc::SetColour(int x,int y,FARBE c){
  if (x<0 || x>=geom.field_mx || y<0 || y>=geom.field_my)
  {
    kdDebug() << "ERROR: SetColour auf falsche Poition " << x << " " << y << endl;
    return ;
  }
  fields[x+y*geom.field_mx]=c;
}

/** Reset the whole game */
void Kwin4Doc::ResetGame(){
  int x,y;

  for (x=0;x<geom.field_mx;x++)
  {
    for (y=geom.field_my-1;y>=0;y--)
    {
      SetColour(x,y,Niemand);
    }
    field_filled[x]=0;
  }
  history_cnt=0;
  currentmove=0;
  maxmove=0;
  lastcolumn=-1;
  lastcolour=Niemand;
  aktwert=0;
  running=0;
  amzug=Colour[0];
  lasthint=-1;
}

/** Start a new game */
void Kwin4Doc::StartGame(){
  running=1;
  intro=0;
}

/** End a game */
void Kwin4Doc::EndGame(TABLE mode){
  running=0;

  switch(mode)
  {
    case TWin:  tab[Gelb].win++;
                tab[Rot].lost++;
    break;
    case TLost: tab[Gelb].lost++;
                tab[Rot].win++;
    break;
    case TRemis: tab[Gelb].remis++;
                 tab[Rot].remis++;
    break;
    default:
       // Only break if moves have been made
       if (maxmove>0)
       {
          tab[Gelb].brk++;
          tab[Rot].brk++;
       }
    break;
  }

  // switch start player
}

/** Is a game running */
bool Kwin4Doc::IsRunning(){
  return running!=0;
}
/** Is the introduction on */
bool Kwin4Doc::IsIntro(){
  return intro!=0;
}
/** Make a game move */
// Play a move
// mode=0 normal move, =1: redo move
MOVESTATUS Kwin4Doc::MakeMove(int x,int mode){
  int y,res;

  if (x<0 || x>=geom.field_mx)
  {
    kdDebug() << "ERROR: MakeMove auf falsche Position " << x << endl;
    return GNotAllowed;
  }

  y=field_filled[x];

  if (y>=geom.field_my)
  {
    return GIllMove;  // no space left in column
  }

  if (lasthint>=0)
  {
    int hy;
    hy=field_filled[lasthint];
    SetColour(lasthint,hy,Niemand);
    lasthint=-1;
  }
  if (mode==Tip)
  {
    lasthint=x;
    SetColour(x,y,Tip);
    return GTip ;  // no real move
  }

  field_filled[x]++;
  SetColour(x,y,amzug);
  history[history_cnt]=x;
  history_cnt++;

  lastcolour=amzug;
  if (amzug==Gelb) amzug=Rot;
  else amzug=Gelb;

  currentmove++;
  // only if a real move isdone the maxmove is raised
  if (mode==0) maxmove=currentmove;
  lastcolumn=x;

  res=CheckGameOver(x,lastcolour);
  if (res==1)
  {
    if (lastcolour==Gelb) return GYellowWin;
    else return GRedWin;
  }
  else if (res==-1) return GRemis;
  return GNormal;
}


/** Undo a move */
int Kwin4Doc::UndoMove(){
  int x,y;
  if (history_cnt<1) return 0;
  if (lasthint>=0)
  {
    int hy;
    hy=field_filled[lasthint];
    SetColour(lasthint,hy,Niemand);
    lasthint=-1;
  }
  history_cnt--;
  x=history[history_cnt];
  field_filled[x]--;
  y=field_filled[x];
  SetColour(x,y,Niemand);
  lastcolour=amzug;
  if (amzug==Gelb) amzug=Rot;
  else amzug=Gelb;
  currentmove--;

  if (history_cnt>0) lastcolumn=history[history_cnt-1];
  else lastcolumn=-1;

  aktwert=0;

  return 1;
}

/** Redo a move */
int Kwin4Doc::RedoMove(){
  int x;
  if (history_cnt>=maxmove) return 0;
  x=history[history_cnt];
  MakeMove(x,1);
  aktwert=0;
  return 1;
}
/** Is colour player by a computer */
bool Kwin4Doc::IsComputer(FARBE col){
  if  (player[col]==Computer) return TRUE;
  return FALSE;
}
/** Is col played by the interactive player */
bool Kwin4Doc::IsUser(FARBE col){
  if  (player[col]==Men) return TRUE;
  return FALSE;
}
/** Is col played remotely */
bool Kwin4Doc::IsRemote(FARBE col){
  if  (player[col]==Remote) return TRUE;
  return FALSE;
}
/** col will be played by the computer */
void Kwin4Doc::SetComputer(FARBE col){
  player[col]=Computer;
}
/** col is played by the interactive user */
void Kwin4Doc::SetUser(FARBE col){
  player[col]=Men;
}
/** col is played remotely */
void Kwin4Doc::SetRemote(FARBE col){
  player[col]=Remote;
}
/** Set the name of col */
void Kwin4Doc::SetName(FARBE i,QString n){
  Names[i]=n;
}
/** Query the name of i */
QString Kwin4Doc::QueryName(FARBE i){
  return Names[i];
}
/** Query the table */
int Kwin4Doc::QueryTable(FARBE i, TABLE mode){
  int r;

  r=0;
  switch(mode)
  {
    case TWin: r=tab[i].win;
    break;
    case TRemis: r=tab[i].remis;
    break;
    case TLost: r=tab[i].lost;
    break;
    case TBrk: r=tab[i].brk;
    break;
    case TSum:  r=tab[i].win+tab[i].remis+tab[i].lost;
    default:
      break;
  }
  return r;
}
/**  */
int Kwin4Doc::QueryStat(FARBE i,TABLE mode){
  int r;

  r=0;
  switch(mode)
  {
    case TWin: r=alltab[i].win;
    break;
    case TRemis: r=alltab[i].remis;
    break;
    case TLost: r=alltab[i].lost;
    break;
    case TBrk: r=alltab[i].brk;
    break;
    case TSum:  r=alltab[i].win+alltab[i].remis+alltab[i].lost;
    default:
      break;
  }
  return r+QueryTable(i,mode);
}
/**  */
void Kwin4Doc::SetStat(FARBE i,int value,TABLE mode){
    switch(mode)
    {
      case TWin: alltab[i].win=value;
      break;
      case TRemis: alltab[i].remis=value;
      break;
      case TLost: alltab[i].lost=value;
      break;
      case TBrk: alltab[i].brk=value;
      break;
      default:
      break;
    }
}
/**  */
void Kwin4Doc::SetTable(FARBE i,int value,TABLE mode){
    switch(mode)
    {
      case TWin: tab[i].win=value;
      break;
      case TRemis: tab[i].remis=value;
      break;
      case TLost: tab[i].lost=value;
      break;
      case TBrk: tab[i].brk=value;
      break;
      default:
      break;
    }
}

/** Query the colour of player i */
FARBE Kwin4Doc::QueryPlayerColour(int player){
  return Colour[player];
}

/** Set the colour of player i */
void Kwin4Doc::SetPlayerColour(int player,FARBE col){
  Colour[player]=col;
}

/**  */
int Kwin4Doc::CheckGameOver(int x, FARBE col){
  int y,xx,i;
  int flag;
  FARBE c;

  // Check dy
  flag=0;
  for (i=0;i<4;i++)
  {
    y=field_filled[x]-1-i;
    if (y>=0)
    {
       c=QueryColour(x,y);
       if (c==col) flag++;
    }
  }
  if (flag>=4) return 1;

  // Check dx
  y=field_filled[x]-1;
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
     xx=x+i;
     if (xx>=0 && xx<geom.field_mx)
     {
       c=QueryColour(xx,y);
       if (c==col) flag++;
       else flag=0;
     }
  }
  if (flag>=4) return 1;


  // Check dy+
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
    xx=x+i;
    if (xx>=0 && xx<geom.field_mx)
    {
      y=field_filled[x]-1-i;
      if (y>=0 && y<geom.field_my)
      {
        c=QueryColour(xx,y);
        if (c==col) flag++;
        else flag=0;
      }
    }
  }
  if (flag>=4) return 1;


  // Check dy-
  flag=0;
  for (i=-3;i<=3 && flag<4;i++)
  {
    xx=x+i;
    if (xx>=0 && xx<geom.field_mx)
    {
      y=field_filled[x]-1+i;
      if (y>=0 && y<geom.field_my)
      {
        c=QueryColour(xx,y);
        if (c==col) flag++;
        else flag=0;
      }
    }
  }
  if (flag>=4) return 1;

  if (currentmove>=42) return -1;

  return 0;
}
/** Reset the stats */
void Kwin4Doc::ResetStat(){
  // Reset data
  tab[Gelb].win=0;
  tab[Gelb].remis=0;
  tab[Gelb].lost=0;
  tab[Gelb].brk=0;
  tab[Rot].win=0;
  tab[Rot].remis=0;
  tab[Rot].lost=0;
  tab[Rot].brk=0;

  alltab[Gelb].win=0;
  alltab[Gelb].remis=0;
  alltab[Gelb].lost=0;
  alltab[Gelb].brk=0;
  alltab[Rot].win=0;
  alltab[Rot].remis=0;
  alltab[Rot].lost=0;
  alltab[Rot].brk=0;
}

/** Set computer score */
void Kwin4Doc::SetScore(long i){
  aktwert=i;
}

/** Query computer score */
long Kwin4Doc::QueryScore(){
  return aktwert;
}

/** Lock input */
void Kwin4Doc::SetLock(){
  is_lock=TRUE;
}

/** Unlock input */
void Kwin4Doc::ReleaseLock(){
  is_lock=FALSE;
}

/**  Input locked? */
bool Kwin4Doc::IsLocked(){
  return is_lock;
}

/** Height of a column */
int Kwin4Doc::QueryHeight(int x){
  if (x<0 || x>=geom.field_mx)
  {
    kdDebug() << "ERROR: Query Height for wrong x " << x << endl;
    return 0;
  }
  return field_filled[x];
}

/**  */
int Kwin4Doc::QueryLastHint(){
  return lasthint;
}

/** read config file */
void Kwin4Doc::ReadConfig(KConfig *config)
{
  config->setGroup("Parameter");
  mHost=config->readEntry("host","localhost");
  mPort=(unsigned short)config->readNumEntry("port",7442);
  procfile=config->readEntry("process","kproc4");
  Names[Gelb]=config->readEntry("Name1","Player 1");
  Names[Rot]=config->readEntry("Name2","Player 2");

  maxreklev=config->readNumEntry("Level",3);
  player[Gelb]=(PLAYER)config->readNumEntry("Player1",(int)Men);
  player[Rot]=(PLAYER)config->readNumEntry("Player2",(int)Computer);

  /** Force no remote games saved */
  if (player[Gelb]==Remote) player[Gelb]=Men;
  if (player[Rot]==Remote) player[Rot]=Men;

  Colour[0]=(FARBE)config->readNumEntry("Colour1",(int)Gelb);
  Colour[1]=(FARBE)config->readNumEntry("Colour2",(int)Rot);
  SetStat(Gelb,config->readNumEntry("Stat1W",0),TWin);
  SetStat(Gelb,config->readNumEntry("Stat1R",0),TRemis);
  SetStat(Gelb,config->readNumEntry("Stat1L",0),TLost);
  SetStat(Gelb,config->readNumEntry("Stat1B",0),TBrk);
  SetStat(Rot,config->readNumEntry("Stat2W",0),TWin);
  SetStat(Rot,config->readNumEntry("Stat2R",0),TRemis);
  SetStat(Rot,config->readNumEntry("Stat2L",0),TLost);
  SetStat(Rot,config->readNumEntry("Stat2B",0),TBrk);
  SetServer(config->readNumEntry("IsServer",1));
}

/** write config file */
void Kwin4Doc::WriteConfig(KConfig *config)
{
  config->setGroup("Parameter");
  config->writeEntry("host",mHost);
  config->writeEntry("port",(int)mPort);
  config->writeEntry("process",procfile);
  config->writeEntry("tmppath",picpath);
  config->writeEntry("delpath",delpath);
  config->writeEntry("Name1",Names[Gelb]);
  config->writeEntry("Name2",Names[Rot]);
  config->writeEntry("Level",maxreklev);
  // Do not save remote !
  if (IsRemote(Gelb))
    config->writeEntry("Player1",(int)Men);
  else
    config->writeEntry("Player1",(int)player[Gelb]);
  if (IsRemote(Rot))
    config->writeEntry("Player2",(int)Men);
  else
    config->writeEntry("Player2",(int)player[Rot]);
  config->writeEntry("Colour1",(int)Colour[0]);
  config->writeEntry("Colour2",(int)Colour[1]);
  config->writeEntry("Stat1W",QueryStat(Gelb,TWin));
  config->writeEntry("Stat1R",QueryStat(Gelb,TRemis));
  config->writeEntry("Stat1L",QueryStat(Gelb,TLost));
  config->writeEntry("Stat1B",QueryStat(Gelb,TBrk));
  config->writeEntry("Stat2W",QueryStat(Rot,TWin));
  config->writeEntry("Stat2R",QueryStat(Rot,TRemis));
  config->writeEntry("Stat2L",QueryStat(Rot,TLost));
  config->writeEntry("Stat2B",QueryStat(Rot,TBrk));
  config->writeEntry("IsServer",QueryServer());

  config->sync();
}

/** Sends and update to all views.
Depending on which only party of
the view, like the table are updated */
void Kwin4Doc::UpdateViews(int which,int x,int y){
  Kwin4View *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
       if (which & UPDATE_STATUS) w->updateStatus();
       if (which & UPDATE_TABLE) w->updateTable();
       if (which & UPDATE_ARROW) w->updateArrow(x);
       if (which & UPDATE_XY) w->updateXY(x,y);
    }
  }

}

void Kwin4Doc::SetInputHandler(KEInput *i)
{
  inputHandler=i;
}
KEInput *Kwin4Doc::QueryInputHandler()
{
  return inputHandler;
}/** Returns the current player, resp
amzug */
FARBE Kwin4Doc::QueryCurrentPlayer(){
  return amzug;
}
void Kwin4Doc::SetCurrentPlayer(FARBE i)
{
  amzug=i;
}
/** Query Computer Level */
int Kwin4Doc::QueryLevel(){
  return maxreklev;
}
/** Set Computer level */
void Kwin4Doc::SetLevel(int i){
  maxreklev=i;
}

  /** Swtich the starting player and return the new started */
FARBE Kwin4Doc::SwitchStartPlayer()
{
  FARBE col;
  col=Colour[0];
  Colour[0]=Colour[1];
  Colour[1]=col;
  return Colour[0];
}
int Kwin4Doc::QueryLastcolumn()
{
  return lastcolumn;
}
FARBE Kwin4Doc::QueryLastcolour()
{
  return lastcolour;
}

int Kwin4Doc::QueryCurrentMove()
{
  return currentmove;
}
void Kwin4Doc::SetCurrentMove(int i)
{
  currentmove=i;
}

int Kwin4Doc::QueryMaxMove()
{
  return maxmove;
}

int Kwin4Doc::QueryHistoryCnt()
{
  return history_cnt;
}
int Kwin4Doc::QueryID()
{
  return docid;
}/** Do we want to be a remote server */
bool Kwin4Doc::QueryServer(){
  return is_server;
}
/**  */
void Kwin4Doc::SetServer(bool s){
  is_server=s;
}
/** Random number between 0..max-1 */
int Kwin4Doc::Random(int max){
int mult;
  mult=(int)((double)rand()/(double)RAND_MAX*(double)max);
  return mult;

}

/** Return the name of the computer player process */
QString Kwin4Doc::QueryProcessName()
{
  return procfile;
}

/** Preload all Bitmaps */
int Kwin4Doc::LoadBitmaps(const QString path){
  QString file;

  file=path+QString("background.png");
  m_PixBackground.load(file);
  file=(path)+QString("board.png");
  m_PixBoard.load(file);
  file=(path)+QString("aboute.png");
  m_PixAbout.load(file);
  file=(path)+QString("arrow.png");
  m_PixArrow.load(file);
  file=(path)+QString("arrow_red.png");
  m_PixArrowRed.load(file);
  file=(path)+QString("arrow_yellow.png");
  m_PixArrowYellow.load(file);
  file=(path)+QString("red.png");
  m_PixFieldRed.load(file);
  file=(path)+QString("yellow.png");
  m_PixFieldYellow.load(file);
  file=(path)+QString("hint.png");
  m_PixFieldHint.load(file);
  file=(path)+QString("empty.png");
  m_PixFieldEmpty.load(file);
  file=(path)+QString("win4about.png");
  m_PixWin4About.load(file);
  return 1;
}

unsigned short Kwin4Doc::QueryPort()
{
  return mPort;
}
QString Kwin4Doc::QueryHost()
{
  return mHost;
}
void Kwin4Doc::SetPort(unsigned short port)
{
  mPort=port;
}

void Kwin4Doc::SetHost(QString host)
{
  mHost=host;
}
#include "kwin4doc.moc"
