#ifndef TEXT_SPRITE_H
#define TEXT_SPRITE_H
/*
   This file is part of the KDE games lskat program
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
#include <QGraphicsTextItem>


/**
 * The sprite for a card on the canvas.
 */
class TextSprite : public QGraphicsTextItem
{

  public:
    /** Constructor for the sprite.
     *  @param parent The parent canvas
     */
    TextSprite(QGraphicsScene* canvas);

    /** Should the text be drawn center aligned or not.
     *  Default is left aligned.
     *  @param b True for aligned text.
     */
    void setCenterAlign(bool b);

  private:

};

#endif
