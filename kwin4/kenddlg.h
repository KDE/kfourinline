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

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class kEndDlg : public QDialog
{ 
    Q_OBJECT

public:
    kEndDlg( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~kEndDlg();
    void SetPixmap(QPixmap &pixmap);

    QFrame* Frame3;
    QLabel* PixmapLabel1;
    QPushButton* PushButton1;
    QFrame* Frame5;
    QLabel* TextLabel1;
    QLabel* PixmapLabel2;
    QPushButton* PushButton1_2;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
    QVBoxLayout* vbox_4;
};

#endif // KENDDLG_H
