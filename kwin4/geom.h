/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2000 by Martin Heni
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


#ifndef __GEOM_H_
#define __GEOM_H_

#include <qrect.h>

class Geom {

  public:
   Geom (); 

  // geometry
  QPoint intro_origin;
  QPoint field_origin;
  QPoint field_offset;
  QRect status_rect;
  QRect table_rect;
  int status_cols[6];
  int status_rows[3];
  int table_cols[4];
  int table_rows[6];

  int field_dx;   // row and coloum spacing
  int field_dy;
  int field_arrow_dy; // arrow offset

  int field_mx;  // size of field
  int field_my; 

};

// Make geometry data gloabally available
#ifdef _GEOM_C_
class Geom geom;
#else
extern class Geom geom;
#endif // _GEOM_C_

#endif // __GEOM_H_

