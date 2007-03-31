#ifndef AIBOARD_H
#define AIBOARD_H
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


// Qt includes
#include <QDataStream>
#include <QHash>

#include "kwin4global.h"


/** Store the values of the cached positions for s set of levels.
  */
class AIValue
{
  /** Support streaming.
    * @param s The stream
    * @param v The value to stream
    * @return The stream.
    */
  friend QDataStream &operator<<(QDataStream& s, const AIValue& v);

  /** Support streaming.
    * @param s The stream
    * @param v The value to stream
    * @return The stream.
    */
  friend QDataStream &operator>>(QDataStream& s, AIValue& v);

  public:
     /** Constructur.
       */
     AIValue();

     /** Set a value for the given level.
       * @param level The level
       * @param value The value
       */
     void setValue(int level, long value);

     /** Get the value for a given level.
       * @param level The level
       * @return The value.
       */
     long value(int level) const;

     /** Checks whether a value is set for the given level.
       * @param level The level
       * @return True if it is set.
       */
     bool isSet(int level) const;

     /** How many levels do we support.
       */
     const static int NO_OF_LEVELS = 5;

   private:
     // Store the level values
     long mValue[NO_OF_LEVELS];
};

/** Support streaming.
  * @param s The stream
  * @param v The value to stream
  * @return The stream.
  */

QDataStream &operator<<(QDataStream& s, const AIValue& value);

/** Support streaming.
  * @param s The stream
  * @param v The value to stream
  * @return The stream.
  */
QDataStream &operator>>(QDataStream& s, AIValue& value);


/**
 * Store an AI field in a compressed structure. One row of pieces
 * is stored as bit sequence with two bits for each field. The bits
 * corresond to the colors (Nobody, Yellow, Red, unusued).
 */
class AIBoard 
{ 
  /** Support streaming.
    * @param s The stream
    * @param v The value to stream
    * @return The stream.
    */
  friend QDataStream &operator<<(QDataStream&, const AIBoard&);

  /** Support streaming.
    * @param s The stream
    * @param v The value to stream
    * @return The stream.
    */
  friend QDataStream &operator>>(QDataStream&, AIBoard&);

  /** Qt Hast function.
    * @param board The board to hash.
    * @return The hash values.
    */
  friend uint qHash(const AIBoard& board);

  public:
    /** Construct an object
      */
    AIBoard();

    /** Construct board from AI field structure 
      * @param current The current mover's color
      * @param field The game field
      */
    AIBoard(uchar current, const char field[][7]);

    /** Compare to boards.
      * @param other The other board
      * @return True if the boards are equal.
      */
    bool operator==(const AIBoard& other) const;
    
    /** Retrieve a mirrored board. The original is not
      * changed.
      * @param The mirrored board.
      */
    AIBoard mirror() const;

    /** Debug function to print out a board to the console.
      */
    void print() const;
    
    /** Convert an AI field to the board structure.
      * @param current The current mover's color
      * @param mirror Mirror the board?
      * @param field The game field
      */
    void fromField(uchar current, bool mirror, const char field[][7]);

  private:
    // Store board pieces per row
    ushort mBoard[6];
    // Mover's color
    uchar mCurrent; 
};

/** Support streaming.
  * @param s The stream
  * @param v The value to stream
  * @return The stream.
  */
QDataStream &operator<<(QDataStream& s, const AIBoard& board);

/** Support streaming.
  * @param s The stream
  * @param v The value to stream
  * @return The stream.
  */
QDataStream &operator>>(QDataStream& s, AIBoard& board);

/** Qt Hast function.
  * @param board The board to hash.
  * @return The hash values.
  */
uint qHash(const AIBoard& key);
#endif 

