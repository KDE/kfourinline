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
#ifndef __NETWORKDLG_H_
#define __NETWORKDLG_H_
#include <qlined.h>
//#include <ktabctl.h>
//#include <qkeycode.h>
//#include <qmlined.h>
//#include <unistd.h>
#include <kapp.h>
#include <qlabel.h>
#include <kconfig.h>
#include <qdialog.h>
//#include <qtooltip.h>
//#include <kmsgbox.h>
//#include <ktopwidget.h>
//#include <kprocess.h>
//#include <kstdaccel.h>
#include <qstring.h>

class NetworkDlg : public QDialog
{
  Q_OBJECT

  public:
   NetworkDlg (QWidget* parent = NULL,const char* name = NULL,const char *sufi=NULL);

   void SetHost(QString s);
   void SetPort(unsigned short port);
   unsigned short QueryPort();
   QString QueryHost();

protected slots:
    void accept();

protected:
    KConfig *config;
    KApplication *app;
    QLineEdit *PortEdit;
//    QLineEdit *SPortEdit;
    QLineEdit *IPEdit;

};
#endif
