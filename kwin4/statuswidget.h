#ifndef _STATUSWIDGET_H
#define _STATUSWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
#include "kwlabel.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;

class StatusWidget : public QWidget
{ 
    Q_OBJECT

public:
    StatusWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~StatusWidget();
    void setBrk(int i,int no);
    void setSum(int i,int no);
    void setLost(int i,int no);
    void setRemis(int i,int no);
    void setWin(int i,int no);
    void setPlayer(QString s,int no);


protected:
    void paintEvent( QPaintEvent * );
    void Paint(QPainter *p,QRect rect);
    void drawBorder(QPainter *p,QRect rect,int offset,int width,int mode);

protected:
    QGridLayout* LayoutB;
    QGroupBox* GroupBox1;
    KWLabel* Label[3][6];
};

#endif 
