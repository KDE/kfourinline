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
