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

// General includes

// Qt includes

// KDE includes
#include <kdebug.h>

// Local includes
#include "textsprite.h"

// Constructor for the view
TextSprite::TextSprite(QGraphicsScene* canvas)
    : QGraphicsTextItem(0, canvas)
{
  setZValue(200.0);
  hide();
  resetMatrix();
}


// Switch center aligned or left aligned text
void TextSprite::setCenterAlign(bool b)
{
  resetMatrix();
  if (b)
  {
    translate(-boundingRect().width()/2.0, 0.0);
  }
}

