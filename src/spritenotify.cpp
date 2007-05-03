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
#include "spritenotify.h"

// Qt includes

// KDE includes
#include <kdebug.h>

// Construct the object
SpriteNotify::SpriteNotify(QGraphicsItem* parent)
            : QObject(0)
{
  mParent = parent;
}


// Emit the signal igven the user defined mode parameter
void SpriteNotify::emitSignal(int mode)
{
  emit signalNotify(mParent,mode);
}

#include "spritenotify.moc"

