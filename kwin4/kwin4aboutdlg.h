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

#ifndef KWIN4ABOUTDLG_H
#define KWIN4ABOUTDLG_H

//Generated area. DO NOT EDIT!!!(begin)
#include <qwidget.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Generated area. DO NOT EDIT!!!(end)
#include <klocale.h>
#include <qdialog.h>

/**
  *@author Martin Heni
  */

class kwin4AboutDlg : public QDialog  {
   Q_OBJECT
public: 
	kwin4AboutDlg(QPixmap *pix,QWidget *parent=0, const char *name=0);
	~kwin4AboutDlg();

protected: 
	void initDialog();
	//Generated area. DO NOT EDIT!!!(begin)
	QGroupBox *QGroupBox_1;
	QLabel *QLabel_1;
	QLabel *QLabel_7;
	QLabel *QLabel_2;
	QLabel *QLabel_8;
	QLabel *QLabel_5;
	QLabel *QLabel_9;
	QLabel *QLabel_11;
	QLabel *QLabel_10;
	QPushButton *QPushButton_1;
	//Generated area. DO NOT EDIT!!!(end)

private: 
  QPixmap *mPixmap;
};

#endif
