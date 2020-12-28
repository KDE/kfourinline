/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SPRITE_NOTIFY_H
#define SPRITE_NOTIFY_H

// Qt
#include <QGraphicsItem>
#include <QObject>


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
    explicit SpriteNotify(QGraphicsItem* parent);
    
    /** Emit the notification signal.
      * @param mode A user defined parameter.
      */
     void emitSignal(int mode);
     
  Q_SIGNALS:
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
