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
#include <QGraphicsItem>


/** SpriteNotify provides a QObject to a sprite that allows to emit
 * a signal from athis sprite if it is necessary to notify another object
 * with an action of the sprite, like animation or movement finished.
 **/
class SpriteNotify : public QObject
{
  Q_OBJECT

  public:
    /** Create the object.
      * @param parent The parent graphics item.
      */
    SpriteNotify(QGraphicsItem* parent);
    
    /** Emit the notification signal.
      * @param mode A user defined parameter.
      */
     void emitSignal(int mode);
     
  signals:
     /** Signal the event for the graphics item and the mode parameter.
       *  @param item The sprite
       *  @param mode The user defined mode
       */
     void signalNotify(QGraphicsItem* item, int mode);
     
  private:
     // The sprite
     QGraphicsItem* mParent;
};

#endif
