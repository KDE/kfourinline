/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2001 by Martin Heni
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

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <kdialogbase.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class aboutDlg : public KDialogBase
{ 
    Q_OBJECT

public:
    aboutDlg( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~aboutDlg();
    void SetPixmap(QPixmap &pixmap);

    QFrame* Frame5;
	  QLabel *Label1;
    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QPushButton* PushButton15;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;

private: 
};

#endif // ABOUTDLG_H

