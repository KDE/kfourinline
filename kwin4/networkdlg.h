/****************************************************************************
** Form interface generated from reading ui file 'network.ui'
**
** Created: Tue Jan 23 17:27:52 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NETWORKDLG_H
#define NETWORKDLG_H

#include <qvariant.h>
#include <qdialog.h>
#include <kdialogbase.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QCheckBox;

class NetworkDlg : public KDialogBase
{ 
    Q_OBJECT

public:
    NetworkDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~NetworkDlg();

   void SetHost(QString s);
   void SetPort(unsigned short port);
   unsigned short QueryPort();
   QString QueryHost();

    QGroupBox* GroupBoxRemote;
    QButtonGroup* ButtonGroup2;
    QRadioButton* radioSlave;
    QRadioButton* radioMaster;
    QLineEdit* remoteHost;
    QLabel* TextLabelHost;
    QLabel* TextLabelPort;
    QLineEdit* port;
    QCheckBox* CheckBox1;

protected:
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QGridLayout* grid;
    QGridLayout* grid_2;
};

#endif // NETWORKDLG_H
