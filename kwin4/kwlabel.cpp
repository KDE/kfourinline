/***************************************************************************
                          kwlabel - QLabel with extensions
                             -------------------
    begin                : 2001 
    copyright            : (C) 2001 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kwlabel.h"
#include <kdebug.h>
#include <qstyle.h>

KWLabel::KWLabel( QWidget * parent, const char * name, WFlags f ) :
          QLabel(parent,name,f)
{
  mForeColor=black;
}


void KWLabel::drawContents ( QPainter * p )
{
  if (mForeColor==black) 
  {
    QLabel::drawContents(p);
  }
  else
  {
    QRect cr = contentsRect();
    QColorGroup cg=colorGroup();
    cg.setColor(QColorGroup::Foreground,mForeColor);
    style().drawItem( p, cr, alignment(), cg, isEnabled(), 0, text() );
  }
}
         

#include "kwlabel.moc"
