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
#ifndef STATDLG_H
#define STATDLG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QGroupBox;
class QLabel;
class QPushButton;

class StatDlg : public QDialog
{ 
    Q_OBJECT

public:
    StatDlg( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~StatDlg();
   void SetNames(QString n1,QString n2);
   void SetStat1(int x1,int x2,int x3,int x4,int x5);
   void SetStat2(int x1,int x2,int x3,int x4,int x5);
   void SetPixmap(QPixmap &pixmap);

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QLabel* TextPlayer1;
    QLabel* TextPlayer2;
    QLabel* TextLabel2;
    QLabel* TextWon1;
    QLabel* TextWon2;
    QLabel* TextLabel3;
    QLabel* TextDrawn1;
    QLabel* TextDrawn2;
    QLabel* TextLabel4;
    QLabel* TextLost1;
    QLabel* TextLost2;
    QLabel* TextLabel5;
    QLabel* TextAborted1;
    QLabel* TextAborted2;
    QLabel* TextLabel6;
    QLabel* TextSum1;
    QLabel* TextSum2;
    QLabel* PixmapLabel2;
    QPushButton* PushButton1;
    QLabel* PixmapLabel1;
    QFrame* Line1;
    QFrame* Frame3;
    QPushButton* PushButton2;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QHBoxLayout* hbox_3;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
    QVBoxLayout* vbox_4;
    QVBoxLayout* vbox_5;
    QVBoxLayout* vbox_6;
    QVBoxLayout* vbox_7;

protected slots:
  void clearStat();
};

#endif // STATDLG_H
