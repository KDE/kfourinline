#ifndef _SCOREWIDGET_H
#define _SCOREWIDGET_H

#include <qwidget.h>
#include "kwlabel.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;

class ScoreWidget : public QWidget
{ 
    Q_OBJECT

public:
    ScoreWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ScoreWidget();
    void setMove(int i);
    void setLevel(int i);
    void setChance(int i);
    void setPlayer(QString s,int no);
    void setTurn(int i); 

protected:
    QGroupBox* GroupBox1;
    KWLabel* TextLabel4;
    KWLabel* TextLabel5;
    KWLabel* TextLabel6;
    KWLabel* TextLabel1;
    KWLabel* TextLabel2;
    KWLabel* TextLabel3;
    KWLabel* TextLabel7;
    KWLabel* TextLabel8;
    KWLabel* TextLabel9;

protected:
    void paintEvent( QPaintEvent * );
    void Paint(QPainter *p,QRect rect);
    void drawBorder(QPainter *p,QRect rect,int offset,int width,int mode);

protected:
    QGridLayout* LayoutB;
};

#endif 
