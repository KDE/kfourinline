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
#ifndef __MSGDLG_H_
#define __MSGDLG_H_

#include <qdialog.h>

class QMultiLineEdit;

class MsgDlg : public QDialog
{
  Q_OBJECT

  public:
   MsgDlg (QWidget* parent = NULL,const char* name = NULL,const char *sufi=NULL);
   QString GetMsg();

  protected slots:

  protected:
    QMultiLineEdit *MultiLine;

};

#endif // __MSGDLG_H_

