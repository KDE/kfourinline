/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2000 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kwin4proc.h"

// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamemessage.h>
// Qt
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QMutex>
#include <QElapsedTimer>
#include <QWaitCondition>
#include <QRandomGenerator>
// Std
#include <cstdio>
#include <cstdlib>
#include <string>

// Algorithm defines
#define MIN_TIME        1000       // min time in milli sec for move
//#define MIN_TIME        10           // min time in milli sec for move

// Board and game geometry
#define MAX_PIECES_COL     6         // Max 6 pieces per column 
#define WIN4               4         // 4 in a row won
#define MAX_MOVE          42         // Maximum so many moves possible
#define FIELD_OFFSET      10         // Offset

// AI rating and routines
#define LOWERT        -999999999L    // Init variables with this value
#define VICTORY_VALUE    9999999L    // Win or loss value
#define MAX_EXTRA_RECURSION    15    // Maximum so many extra recursions
#define PERCENT_FOR_INC_ITERATION 40 // If less than this amount of estimated moves are
#define EVAL_RANDOM        2500      // Randomize position evaluation by this
                                     // calculated increase recursion

// AI Learning
#define LEARN_DELTA   -15000L        // Learn if position drops by more than this
#define MAX_LEARNED_POSITIONS 100000 // Learn not more than this positions 
#define MIN_LEARN_LEVEL     3        // Use learning only >= this level
#define BRAIN_VERSION       1        // Version number


// Constructor: Setup AI
KComputer::KComputer()
{
  const char *s1="7777776666666123456654321123456654321";
  const char *s2="0000000000000000000123456000000123456";
  unsigned int i;

  // Init variables
  for (i=0;i<strlen(s1);++i)
    mRowLengths[i]=s1[i]-'0';
  for (i=0;i<strlen(s2);++i)
    mStartOfRows[i]=s2[i]-'0';

  // Unknown yet how fast AI calculates
  mCalcPosPerMS = -1.0; 

  // No brain loaded yet
  mBrainLoaded = false;
  mBrainUsed   = 0;

  // Connect signals of KGame framework
  connect(&proc, &KGameProcess::signalCommand, this, &KComputer::slotCommand);
  connect(&proc, &KGameProcess::signalInit, this, &KComputer::slotInit);
  connect(&proc, &KGameProcess::signalTurn, this, &KComputer::slotTurn);
}


// Received init command (unused)
void KComputer::slotInit(QDataStream & /*in */,int /*id*/)
{
  /*
  QByteArray buffer;
  QDataStream out(buffer,QIODevice::WriteOnly);
  int msgid=KGameMessage::IdProcessQuery;
  out << (int)1;
  proc.sendSystemMessage(out,msgid,0);
  */
}


// Received turn command
void KComputer::slotTurn(QDataStream &in, bool turn)
{
  QByteArray buffer;
  QDataStream out(&buffer,QIODevice::WriteOnly);
  //fprintf(stderr,"  KComputer::slotTurn(turn=%d)\n",turn);
  //fflush(stderr);
  if (turn)
  {
    // Create a move

    MoveResult result = think(in,out,false);
    int id            = KGameMessage::IdPlayerInput;
    proc.sendSystemMessage(out,id,0);
    sendValue(result.value, mCurMoveNo);
    //fprintf(stderr,"  KComputer::slotTurn sending value (value=%ld)\n",result.value);
    //fflush(stderr);
  }
}


// Send position data back to main program
void KComputer::sendValue(long value, int moveNo)
{
  qint8 cid = 1; // notifies our KGameIO that this is a value message
  int id    = KGameMessage::IdProcessQuery;
  QByteArray buffer;
  QDataStream out(&buffer,QIODevice::WriteOnly);
  out << cid << (qint32 )value << (qint32)moveNo << (qint32)mLevel << mMaxAIBoard;
  proc.sendSystemMessage(out,id,0);
  //fprintf(stderr,"  KComputer::sendValue (value=%ld)\n",value);
  //fflush(stderr);
}


// Load brain position cache
void KComputer::loadBrain()
{
  QFile file(mBrainDir+QStringLiteral("kwin4.brain"));
  if (!file.open(QIODevice::ReadOnly ))
  {
    fprintf(stderr,"  KComputer::Brain file cannot be opened.\n");
    fflush(stderr);
    return;
  }

  QDataStream in(&file);
  qint32 version;
  in >> version;
  if (version != (qint32)BRAIN_VERSION)
  {
    fprintf(stderr,"  KComputer::Loading brain version error %ld\n",(long)version);
    fflush(stderr);
    file.close();
    return;
  }

  qint32 noOfItems;
  in >> noOfItems;
  fprintf(stderr,"  KComputer::Loading %ld brain data\n",(long)noOfItems);
  fflush(stderr);

  bool erase = false;
  if (noOfItems >MAX_LEARNED_POSITIONS) erase = true; 

  for (int cnt=0; cnt<noOfItems; ++cnt)
  {
    AIBoard board;
    AIValue value;
    in >> board >> value;

    // Forget 10% positions in case of overload
    if (erase && cnt%10 != 0)
    {
      mBrain.insert(board, value);
    }
  }
  fflush(stderr);
  qint32 check;
  in >> check;
  file.close();

  if (check != (qint32)0x18547237)
  {
    fprintf(stderr,"  KComputer::Loading brain CRC error %ld cnt=%d\n",(long)check,noOfItems);
    fflush(stderr);
    mBrain.clear();
    return;
  }
  fprintf(stderr,"  KComputer::Loading brain (%d items) succeeded\n",(int)noOfItems);
  fflush(stderr);
}


// Save brain position cache
void KComputer::saveBrain()
{
  QFile file(mBrainDir+QStringLiteral("kwin4.brain"));
  if (!file.open(QIODevice::WriteOnly ))
  {
    fprintf(stderr,"  KComputer::saving brain failed.\n");
    fflush(stderr);
    return;
  }

  QDataStream out(&file);
  out << (qint32)BRAIN_VERSION; // Version
  out << (qint32)mBrain.size();
  
  QHashIterator<AIBoard, AIValue> it(mBrain);
  while (it.hasNext()) 
  {
      it.next();
      AIBoard board = it.key();
      AIValue value = it.value();
      out << board << value;
  }
  out << (qint32)0x18547237; // Checksum
  file.close();
  fprintf(stderr,"  KComputer:: Brain saved.\n");
  fflush(stderr);
}


// Received a command from the main program
void KComputer::slotCommand(QDataStream &in, int msgid, int /*receiver*/, int /*sender*/)
{
  //fprintf(stderr,"KComputer::slotCommand(Msgid=%d)\n",msgid);
  //fflush(stderr);
  QByteArray buffer;
  QDataStream out(&buffer,QIODevice::WriteOnly);
  switch(msgid)
  {
    case 2:  // hint
    {
      qint8 cid   = 2;
      quint32 recv = 0;
      out << cid;
      MoveResult result = think(in,out,true);
      out << ( qint32 )result.value;
      int id = KGameMessage::IdProcessQuery;
      proc.sendSystemMessage(out, id, recv);
    }
    break;
    case 3: // AI board value changed, maybe learn 
    {
      AIBoard aiBoard;
      qint32  value, level, delta;
      in >> aiBoard >> value >> delta >> level;
      // Only learn big changes
      if (mIsLearning && delta < LEARN_DELTA && level >= MIN_LEARN_LEVEL && level < MIN_LEARN_LEVEL+AIValue::NO_OF_LEVELS)
      {
        fprintf(stderr,"KComputer:: LEARNING Received AI board for value %ld delta %ld level %ld\n",
                       (long)value, (long)delta, (long)level);
        aiBoard.print();

        // Learn board?
        AIValue aiValue;
        // Do we already store board?
        if (mBrain.contains(aiBoard))
        {
          aiValue = mBrain.value(aiBoard);
        }
        // Do we store mirror board?
        else if (mBrain.contains(aiBoard.mirror()))
        {
          aiBoard = aiBoard.mirror();
          aiValue = mBrain.value(aiBoard);
        }
        // Set not set or decreased values in value structure
        if (!aiValue.isSet((int)level-MIN_LEARN_LEVEL) || aiValue.value((int)level-MIN_LEARN_LEVEL) > (long)value)
        {
          aiValue.setValue((int)level-MIN_LEARN_LEVEL, (long)value);
          mBrain.insert(aiBoard, aiValue);
          saveBrain();
          fprintf(stderr, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
          fprintf(stderr, "$        LEARNING                          $\n");
          fprintf(stderr, "$ Setting board level %d to value = %ld\n",(int)level, (long)value);
          fprintf(stderr, "$ mBrain size=%d\n", mBrain.size());
          fprintf(stderr, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
          fflush(stderr);
        }
        else  // TODO: Remove else
        {
          fprintf(stderr, "NOT LEARNING $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
          fprintf(stderr, "     REASON: %d %ld %ld\n",
                          aiValue.isSet((int)level-MIN_LEARN_LEVEL), aiValue.value((int)level-MIN_LEARN_LEVEL),(long)value);
          fflush(stderr);
        }
      }// end delta
    }
    break;
    default:
      fprintf(stderr,"KComputer:: unknown command (msgid=%d)\n",msgid);
      fflush(stderr);
  }
}


// Think up a move (plus reading data from stream)
KComputer::MoveResult KComputer::think(QDataStream& in, QDataStream& out, bool /*hint*/)
{
  qint32 pl;
  qint32 move;
  qint32 tmp;
  COLOUR secondPlayer;
  // Which color's move is to come
  COLOUR currentPlayer;   
  // Which player started the game
  COLOUR startPlayer; 

  // Read command data stream into local variables
  in >> pl ;
  in >> tmp;
  mCurMoveNo=tmp;
  in >> tmp;
  currentPlayer=(COLOUR)(tmp);
  in >> tmp;
  startPlayer=(COLOUR)(tmp);
  in >> tmp;
  secondPlayer=(COLOUR)(tmp);
  in >> tmp;
  mLevel=tmp;
  in >> tmp;
  mIsLearning=tmp;
  in >> mBrainDir;

  // Check proper learning level. If not proper switch learning off.
  if (mLevel<MIN_LEARN_LEVEL || mLevel>=MIN_LEARN_LEVEL+AIValue::NO_OF_LEVELS)
  {
    mIsLearning = false;
    fprintf(stderr,"KComputer:: Switching off learning for level %d\n",mLevel);
  }

  fprintf(stderr,"KComputer::think: pl=%d, mCurMoveNo=%d currentPlayer=%d begin=%d secondPlayer=%d level=%d learn=%d\n",
                 pl,mCurMoveNo,currentPlayer,startPlayer,secondPlayer,mLevel,mIsLearning);
  //fprintf(stderr,"KComputer::learning=%d path=%s\n",mIsLearning,mBrainDir.toLatin1().data());

  // Brain loaded?
  if (!mBrainLoaded && mIsLearning)
  {
    mBrainLoaded = true;
    loadBrain();
  }

  // Setup board (init)
  // The game board prepared for the AI
  FARBE fieldMatrix[SIZE_Y_ALL+1][SIZE_X+1];
  // Amount of pieces on the game board
  char numberMatrix[SIZE_Y_ALL+1];

  for (int y=0;y<=SIZE_Y_ALL;++y)
  {
    numberMatrix[y]=0;
  }

  for (int y=0;y<=SIZE_Y;++y)
  {
    for (int x=0;x<=SIZE_X;++x)
    {
      fieldMatrix[y][x]      = (FARBE)(y+FIELD_OFFSET);
      fieldMatrix[6+x][y]    = (FARBE)(y+FIELD_OFFSET);
      fieldMatrix[13+x+y][x] = (FARBE)(y+FIELD_OFFSET);
      fieldMatrix[30+x-y][x] = (FARBE)(y+FIELD_OFFSET);
    } // next x
  }// next y 

  // Field comes as 42 qint8's representing moves
  int i,j;
  for (i=0;i<=SIZE_Y;++i)
  {
    for (j=0;j<=SIZE_X;++j)
    {
      qint8 col;
      in >> col;
      DoMove(j, (COLOUR)col, fieldMatrix, numberMatrix);
    }
  }


  // Check final checksum
  in >> tmp;
  if ((long)tmp != 421256L)
  {
    fprintf(stderr,"CHECKSUM=%ld [should be 421256]\n",(long)tmp);
    fflush(stderr);
  }

  // Estimated number of positions to evaluate (MAX)
  int estimated = 0;
  for (int i=1; i<= mLevel+1; ++i)
  {
    estimated += int(pow(7.,i));
  }

  // Measure time of move and positions evaluated
  QElapsedTimer timer;
  timer.start();
  mPosEvaluations = 0;

  // Get move
  int gameOver = 0;
  int extraRecurstion = 0;
  MoveResult result;

  // Loop movement if not many positions are evaluated (end game)
  // to look ahead a bit more
  do
  {
    // Actually calculate move
    result = MinMax(currentPlayer, fieldMatrix, numberMatrix,  mLevel + extraRecurstion, mCurMoveNo, true);
    result.value = -result.value;

    // Do not recalculate for (nearly finished) games
   // if (result.value >= VICTORY_VALUE*0.95 || result.value <= -VICTORY_VALUE *0.95) gameOver = 1;
    extraRecurstion++;
  }while(PERCENT_FOR_INC_ITERATION/10*mPosEvaluations <= estimated && !gameOver && extraRecurstion < MAX_EXTRA_RECURSION);

  // Measure elapsed time
  int elapsed = timer.elapsed();
  if (elapsed < 1) elapsed = 1;
  mCalcPosPerMS = (float)mPosEvaluations/(float)elapsed;

  // Debug
  fprintf(stderr,"AI MOVE to %d value=%ld level %d took time=%d ms and evals=%d estimated=%d pos/ms=%f brain=%ld\n",
             result.move,result.value,mLevel, elapsed, mPosEvaluations,estimated,mCalcPosPerMS,mBrainUsed);

  // Sleep a minimum amount to slow down moves
  if (elapsed < MIN_TIME) 
  {
    // is usleep(1000*(MIN_TIME-elapsed));
    QMutex mutex;
    QWaitCondition cond;
    mutex.lock();
    cond.wait(&mutex, MIN_TIME-elapsed);
    
    elapsed = timer.elapsed();
    fprintf(stderr,"AI after sleeping time elapsed=%d\n",elapsed);
  }
  fflush(stderr);


  // Send out move
  move = result.move;
  out << pl << move;
  return result;
}


// Min-Max AI algorithm
KComputer::MoveResult KComputer::MinMax(COLOUR color, FARBE field[][SIZE_X+1], char numbers[], int reklev, int moveNo, bool first)
{
  // Modify move value
  static long gauss[]={10,50,300,500,300,50,10};

  // Local board
  FARBE locField[SIZE_Y_ALL+1][SIZE_X+1];
  char locNumbers[SIZE_Y_ALL+1];

  // Result of move
  MoveResult result;
  result.move  = -1; // No move found
  result.value = LOWERT; 

  for (int x=0; x<=SIZE_X; ++x)
  {
    long wert;
    if (numbers[6+x]>=MAX_PIECES_COL) continue;
    
    // Perform test move
    memcpy(locNumbers, numbers, sizeof(locNumbers));
    memcpy(locField, field, sizeof(locField));
    DoMove(x, color, locField, locNumbers);

    // Count evaluations
    mPosEvaluations++;


    // Check for game over
    COLOUR winner = isGameOver(field, numbers);
    if (winner != Nobody)
    {
      // Choose tree with fastest victory or slowest loss
      if (winner==color) wert = VICTORY_VALUE+reklev*5000;
      else wert = -VICTORY_VALUE-reklev*5000;
    }
    // Drawn / Remis
    else if (moveNo >= MAX_MOVE)
    {
      wert = 0;
    } 
    // End of recursion reached
    else if (reklev <= 0) 
    {
      wert = PositionEvaluation(color, field);
      //wert += gauss[x]; // TODO: Gauss should be here
      // No need for filling the test board here
    }
    // MinMax calculation for next recursion level
    else
    {
      // Check learning
      AIBoard testBoard(color, locField);
      // Found cached board or not
      bool haveToCalculate = true;

      // Check for learned board
      if (mIsLearning && mBrain.contains(testBoard))
      {
        AIValue testValue = mBrain.value(testBoard);
        if (testValue.isSet(mLevel-MIN_LEARN_LEVEL)) 
        {
          wert = testValue.value(mLevel-MIN_LEARN_LEVEL);
          mBrainUsed++;
          haveToCalculate = false;
          fprintf(stderr, "$$$$$$ Board[%d] found in cache value = %ld used=%ld\n",mLevel, wert, mBrainUsed);
          fflush(stderr);
        }
      }
      // Check for learned mirror board
      else if (mIsLearning && mBrain.contains(testBoard.mirror()))
      {
        testBoard = testBoard.mirror();
        AIValue testValue = mBrain.value(testBoard);
        if (testValue.isSet(mLevel-MIN_LEARN_LEVEL)) 
        {
          wert = testValue.value(mLevel-MIN_LEARN_LEVEL);
          mBrainUsed++;
          haveToCalculate = false;
          fprintf(stderr, "$$$$$$ MIRROR 2 Board[%d] found in cache value = %ld  used=%ld\n",mLevel, wert, mBrainUsed);
          fflush(stderr);
        }
      }

      // Real calculation necessary
      if (haveToCalculate)
      {
        // Swap color for next move
        COLOUR nextColor;
        if (color == Red) nextColor = Yellow;
        else nextColor = Red;
        MoveResult minmaxResult = MinMax(nextColor, locField, locNumbers, reklev-1, moveNo+1, false);
        wert = minmaxResult.value + gauss[x]; // TODO: Remove gauss here
      }
    }// end else MinMax 

    // New maximum?
    if (wert >= result.value)
    {
      result.value = wert;
      result.move  = x;
      if (first) mMaxAIBoard.fromField(color, false, locField);
      //if (result.value >= VICTORY_VALUE) break;
    }
   }//next x

   // MinMax
   result.value = -result.value; 
   return result;
}// end MinMax


// Position evaluation
long KComputer::PositionEvaluation(COLOUR curColor, FARBE field[][SIZE_X+1])
{
/* Abstand:              0    1    2    3    4    5    */
static long myWERT[]={2200,600, 300,  75,  20,   0};
//static long myWERT[]={0,0,0,0,0,0};
/* How many ofCOLOUR:       0     1     2     3    4 */
static long steinWERT[4][5]=
{
        {     0,  500L, 40000L,200000L,VICTORY_VALUE}, // Leerfelder=0
        {     0,  500L,  8000L, 40000L,VICTORY_VALUE}, //           =1
        {     0,   00L,  4000L, 25000L,VICTORY_VALUE}, //           =2
        {     0,   00L,  2000L, 12500L,VICTORY_VALUE}, //           =3
};

  // Initial values
  long yellow_value = random(EVAL_RANDOM);
  long red_value    = random(EVAL_RANDOM);

  for (int y=0; y<=SIZE_Y_ALL; ++y)
  {
    if (mRowLengths[y]<WIN4) continue;
    for (int i=0;i<=(mRowLengths[y]-WIN4);++i)
    {
      COLOUR color = Nobody;
      long value   = 0;
      int cntcol   = 0;
      int cnt      = 0;
      for (int j=0; j<WIN4; ++j)
      {
        FARBE checkField = field[y][i+j+mStartOfRows[y]];
        if ((COLOUR)checkField==Red)
        {
          if (color==Yellow) {color=Nobody;break;}
          cntcol++;
          color=Red;
        }
        else if ((COLOUR)checkField==Yellow)
        {
          if (color==Red) {color=Nobody;break;}
          cntcol++;
          color=Yellow;
        }
        else
        {
          cnt   += checkField-FIELD_OFFSET;
          value += myWERT[checkField-FIELD_OFFSET];
        }
      }/*next j */
      if (cnt>3) cnt=3;
      if (color==Red)         red_value    += (value+steinWERT[cnt][cntcol]);
      else if (color==Yellow) yellow_value += (value+steinWERT[cnt][cntcol]);
    }/*next i*/
  }/*next y*/

  // Return value
  if (curColor==Red) return red_value-yellow_value;
  else return yellow_value-red_value;
}


// Check for game over 
COLOUR KComputer::isGameOver(FARBE field[][SIZE_X+1],char numbers[])
{
  for (int y=0; y<=SIZE_Y_ALL; ++y)
  {
     if (numbers[y] < WIN4) continue;
     if (mRowLengths[y] < WIN4) continue;

     int cnt   = 0;
     COLOUR thiscolor = Nobody;
     for (int x=0; x<mRowLengths[y]; ++x)
     {
       COLOUR checkField = (COLOUR)field[y][x+mStartOfRows[y]];
       if (checkField == thiscolor) ++cnt;
       else {cnt=1; thiscolor=checkField;}
       if ( (cnt>=WIN4) &&( (thiscolor==Yellow)||(thiscolor==Red) ) ) return thiscolor;
     }// next x
  }// next y
  return Nobody ;
}


// Execute move on given board
void KComputer::DoMove(int move, COLOUR color, FARBE field[][SIZE_X+1], char numbers[])
{
  if (color==Tip || color==Nobody) return ;  // no real move
  int x       = move;
  int y       = numbers[6+move];
  field[y][x] = color;

  field[6+x][y]    = color;
  field[13+x+y][x] = color;
  field[30+x-y][x] = color;
  numbers[y]++;
  numbers[6+x]++;
  numbers[13+x+y]++;
  numbers[30+x-y]++;
  for (int i=y+1; i<=SIZE_Y; ++i)
  {
     field[i][x]--;
     field[6+x][i]--;
     field[13+x+i][x]--;
     field[30+x-i][x]--;
  }
}


// Retrieve random number 0..max
int KComputer::random(int max)
{
  //return 0; 
  return proc.random()->bounded(max);
}

// Main startup
int main()
{
  // This is the computer player...it should do the calculation
  // It doesn't do much here
  fprintf(stderr,"AI process starts up\n");
  fflush(stderr);
  KComputer comp;
  // And start the event loop
  comp.proc.exec();
  fprintf(stderr,"AI process exists.\n");
  fflush(stderr);
  return 1;
}

