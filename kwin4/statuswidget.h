#ifndef _STATUSWIDGET_H
#define _STATUSWIDGET_H

#include <qframe.h>
class QLabel;
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;

class StatusWidget : public QFrame
{ 
    Q_OBJECT

public:
    StatusWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    void setBrk(int i,int no);
    void setSum(int i,int no);
    void setLost(int i,int no);
    void setRemis(int i,int no);
    void setPlayer(QString s,int no);

public slots:
     void setWin(int i,int no);
   
protected:
    void paintEvent( QPaintEvent * );
    void Paint(QPainter *p,QRect rect);

protected:
    QGridLayout* LayoutB;
    QGroupBox* GroupBox1;
    QLabel* Label[3][6];
};

#endif  // _STATUSWIDGET_H

