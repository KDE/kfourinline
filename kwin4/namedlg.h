/****************************************************************************
** Form interface generated from reading ui file 'namedlg.ui'
**
** Created: Thu Nov 23 11:10:15 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NAMEDLG_H
#define NAMEDLG_H

#include <qvariant.h>
#include <kdialogbase.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class NameDlg : public KDialogBase
{ 
    Q_OBJECT

public:
    NameDlg( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~NameDlg();
   void SetNames(QString n1,QString n2);
   void GetNames(QString &n1,QString &n2);

    QGroupBox* player_names;
    QLabel* text_player1;
    QLineEdit* edit_player1;
    QLabel* text_player2;
    QLineEdit* edit_player2;
    QPushButton* PushButton1;
    QPushButton* PushButton2;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QHBoxLayout* hbox_3;
    QHBoxLayout* hbox_4;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
};

#endif // NAMEDLG_H
