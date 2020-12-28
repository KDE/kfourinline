/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "spritenotify.h"

// own
#include "kfourinline_debug.h"

// Construct the object
SpriteNotify::SpriteNotify(QGraphicsItem* parent)
            : QObject()
{
  mParent = parent;
}


// Emit the signal igven the user defined mode parameter
void SpriteNotify::emitSignal(int mode)
{
  Q_EMIT signalNotify(mParent,mode);
}



