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

#ifndef KENDDLG_H
#define KENDDLG_H

//Generated area. DO NOT EDIT!!!(begin)
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
//Generated area. DO NOT EDIT!!!(end)

#include <qdialog.h>
#include <klocale.h>
/**
  *@author Martin Heni
  */

class kEndDlg : public QDialog  {
   Q_OBJECT
public: 
	kEndDlg(QPixmap *pix,QWidget *parent=0, const char *name=0);
	~kEndDlg();

protected: 
	void initDialog();
	//Generated area. DO NOT EDIT!!!(begin)
	QLabel *QLabel_1;
	QLabel *QLabel_2;
	QPushButton *QPushButton_1;
	QPushButton *QPushButton_2;
	QGroupBox *QGroupBox_1;
	QLabel *QLabel_4;
	QLabel *QLabel_5;
	QLabel *QLabel_6;
	//Generated area. DO NOT EDIT!!!(end)

private: 
  QPixmap *mPixmap;
};

#endif
