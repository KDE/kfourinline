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
#ifndef __STATDLG_H_
#define __STATDLG_H_
#include <qlined.h>
//#include <ktabctl.h>
//#include <qkeycode.h>
//#include <qmlined.h>
//#include <unistd.h>
#include <kapp.h>
#include <qlabel.h>
//#include <qtooltip.h>
//#include <kmsgbox.h>
//#include <ktopwidget.h>
//#include <kprocess.h>
//#include <kstdaccel.h>
#include <qstring.h>
#include <qdialog.h>

class StatDlg : public QDialog
{
  Q_OBJECT

  public:
   StatDlg (QWidget* parent = NULL,const char* name = NULL,const char *sufi=NULL);

   void SetNames(QString n1,QString n2);
   void SetStat1(int x1,int x2,int x3,int x4,int x5);
   void SetStat2(int x1,int x2,int x3,int x4,int x5);
   void Init();

   private:
   QString name1;
   QString name2;
   int win1,remis1,loose1,brk1,sum1;
   int win2,remis2,loose2,brk2,sum2;

protected slots:
  void clearStat();

protected:
};
#endif
