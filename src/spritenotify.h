#ifndef SPRITE_NOTIFY_H
#define SPRITE_NOTIFY_H
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
#include <QObject>
#include <QGraphicsPixmapItem>


/**
    * this is an internal class to provide a @ref QObject to emit
    * a signal from a sprite if a notify object is created
    * You do not need this directly.
    **/
    class SpriteNotify : public QObject
    {
      Q_OBJECT

      public:
        SpriteNotify(QGraphicsItem* parent);
        void emitSignal(int mode);
      signals:
        void signalNotify(QGraphicsItem *,int);
      private:
        QGraphicsItem* mParent;
    };

#endif
