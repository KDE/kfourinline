/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Sun Mar 26 2000
    copyright            : (C) |1995-2000 by Martin Heni
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
/*
    The geom class as utilised by the GUI

    $Id$


    (c) Martin Heni, martin@heni-online.de
	      June 1999

    License: GPL

*/

#define _GEOM_C_
#include "geom.h"

// #include "geom.moc"

Geom::~Geom()
{
}

// Simply set the geometry of the layout
Geom::Geom(QObject * parent, const char * name )
  : QObject(parent,name)
{
  // geometry
  intro_origin=QPoint(150,40);
  field_origin=QPoint(15,40);
  field_offset=QPoint(20,20);
  status_rect=QRect(385,25,155,158);
  table_rect=QRect(345,250,220,100);
  status_cols[0]=9;
  status_cols[1]=95;
  status_cols[2]=115;
  status_cols[3]=135;
  status_cols[4]=175;
  status_cols[5]=195;

  status_rows[0]=27;
  status_rows[1]=47;
  status_rows[2]=64;

  table_cols[0]=70;
  table_cols[1]=18;
  table_cols[2]=55;
  table_cols[3]=55;

  table_rows[0]=40;
  table_rows[1]=76; // 88
  table_rows[2]=98;  //110  -12
  table_rows[3]=120;  //132  -12
  table_rows[4]=23;
  table_rows[5]=57;

  field_dx=40;
  field_dy=40;
  field_arrow_dy=-54;

  field_mx=7;
  field_my=6;
}

#include "geom.moc"
