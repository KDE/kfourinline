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

#ifndef KWLABEL_H
#define KWLABEL_H

#include <qlabel.h>

class KWLabel : public QLabel
{ 
    Q_OBJECT

public:
  KWLabel ( QWidget * parent, const char * name=0, WFlags f=0 );
  void setForegroundColor(QColor c) {mForeColor=c;}
  QColor foregroundColor() {return mForeColor;}

protected:
  void drawContents ( QPainter * p );

private:
  QColor mForeColor;

};

#endif  // KWLABEL_H

