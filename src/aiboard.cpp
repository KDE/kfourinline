/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

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

// Header includes
#include "aiboard.h"

// Standard includes
#include <stdio.h>

// KDE includes
#include <kdebug.h>

// Construct a board object
AIBoard::AIBoard()
{
}


// Construct a board object from a given game field.
AIBoard::AIBoard(uchar current, const char field[][7])
{
  fromField(current, false, field);
}


// Compare two boards
bool AIBoard::operator==(const AIBoard& other) const
{
  if (mCurrent != other.mCurrent) return false;
  for (int i=0; i<6; i++) 
  {
    if (mBoard[i] != other.mBoard[i]) return false;
  }
  return true;
}


// Create a board from a given game field. Support mirroring the field.
void AIBoard::fromField(uchar current, bool mirror, const char field[][7])
{
  // Store current player
  mCurrent = current;

  // Loop rows
  for (int y=0;y<=5;y++)
  {
    ushort tmp = 0;
    // Loop coloums
    for (int x=0; x<=6; x++)
    {
      tmp = tmp << 2;
      // Mirror board?
      if (mirror)
      {
        if (field[y][6-x] == Yellow)   tmp |= 1;
        else if (field[y][6-x] == Red) tmp |= 2;
        // else tmp |= 0; 
      }
      else
      {
        if (field[y][x] == Yellow)   tmp |= 1;
        else if (field[y][x] == Red) tmp |= 2;
        // else tmp |= 0; 
      }
    }
    mBoard[y] = tmp;
  }
}


// Retrieve a mirrored board.
AIBoard AIBoard::mirror() const
{
  AIBoard other;
  other.mCurrent = mCurrent;

  // Loop rows
  for (int y=0;y<=5;y++)
  {
    ushort tmp  = mBoard[y];
    ushort ntmp = 0;
    // Loop coloums
    for (int x=0; x<=6; x++)
    {
      ntmp = ntmp << 2;
      ntmp |= (tmp >> (x<<1)) & 3; 
    }
    other.mBoard[y] = ntmp;
  }
  return other;
}


// Debug function to output the board.
void AIBoard::print() const
{
  fprintf(stderr," AIBOARD ======== COLOR %d ========\n", mCurrent);
  for (int i=5;i>=0;i--)
  {
    ushort tmp = mBoard[i];
    fprintf(stderr, "   Row %d: [%04x]", i, tmp);
    for (int x=6; x>=0; x--)
    {
      if (((tmp >> (x*2)) & 3) == 1) fprintf(stderr," 1 ");
      else if (((tmp>>(x*2) & 3)) == 2) fprintf(stderr," 2 ");
      else fprintf(stderr," 0 ");
    }
    fprintf(stderr,"\n");
  }
  fflush(stderr);
}
    

// Stream the board
QDataStream& operator<<(QDataStream& s, const AIBoard& board)
{
    s << (qint8)board.mCurrent;
    for (int i=0;i<6;i++) s << (qint16)board.mBoard[i]; 
    return s;
}


// Stream the board
QDataStream& operator>>(QDataStream& s, AIBoard& board)
{
    qint8 tmp8;
    qint16 tmp16;
    s >> tmp8;
    board.mCurrent = tmp8;
    for (int i=0;i<6;i++) {s >> tmp16; board.mBoard[i] = tmp16;}
    return s;
}


// Create hashkey for a board
uint qHash(const AIBoard& key)
{
    uint h;
    h = (key.mCurrent&1) << 15;

    h += key.mBoard[0];
    h += key.mBoard[1] << 1;
    h += key.mBoard[2];
    h += (~(key.mBoard[3])     )&0x7fff;
    h += (~(key.mBoard[4] << 1))&0x7fff;
    h += (~(key.mBoard[5]))     &0x7fff;
    
    return h;
}


// Create a value structure. Fill it with an unused value as
// marker for 'unused'.
AIValue::AIValue()
{
  for (int i=0; i<NO_OF_LEVELS;i++)
  {
    mValue[i] = 0x40000000;
  }
}


// Store a value for a given level.
void AIValue::setValue(int level, long value)
{
  if (level<0 || level>=NO_OF_LEVELS)
  {
    fprintf(stderr, "AIValue::setValue: Level %d not supported\n", level);
    fflush(stderr);
    return;
  }
  mValue[level] = value;
}


// Retrieve a value for a given level.
long AIValue::value(int level) const
{
  if (level<0 || level>=NO_OF_LEVELS)
  {
    fprintf(stderr, "AIValue::value: Level %d not supported\n", level);
    fflush(stderr);
    return 0;
  }
  return mValue[level];
}


// Check whether a given level has a stored value.
bool AIValue::isSet(int level) const
{
  if (level<0 || level>=NO_OF_LEVELS)
  {
    fprintf(stderr, "AIValue::isSet: Level %d not supported\n", level);
    fflush(stderr);
    return 0;
  }
  if (mValue[level] == 0x40000000) return false;
  return true;
}


// Stream a value
QDataStream& operator<<(QDataStream& s, const AIValue& value)
{
    for (int i=0;i<AIValue::NO_OF_LEVELS;i++) s << (qint32)value.mValue[i];
    return s;
}


// Stream a value
QDataStream& operator>>(QDataStream& s, AIValue& value)
{
    qint32 tmp32;
    for (int i=0;i<AIValue::NO_OF_LEVELS;i++) {s >> tmp32; value.mValue[i] = tmp32;}
    return s;
}

