/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Apr  9 22:56:15 CEST 2000
    copyright            : (C) 2000 by Martin Heni
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

#include <stdio.h>
#include "kproc4.h"



int main(int /*argc*/, char * /* argv[] */)
{
  KProc4 mComm;
  mComm.exec();
  return 1;
}
