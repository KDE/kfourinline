#ifndef _SCOREWIDGET_H
#define _SCOREWIDGET_H

#include <qframe.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;

class ScoreWidget : public QFrame
{ 
    Q_OBJECT

public:
    ScoreWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    void setMove(int i);
    void setLevel(int i);
    void setChance(int i);
    void setPlayer(QString s,int no);
    void setTurn(int i); 

protected:
    QGroupBox* GroupBox1;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QLabel* TextLabel6;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLabel* TextLabel3;
    QLabel* TextLabel7;
    QLabel* TextLabel8;
    QLabel* TextLabel9;

protected:
    void paintEvent( QPaintEvent * );
    void Paint(QPainter *p,QRect rect);
    void drawBorder(QPainter *p,QRect rect,int offset,int width,int mode);

protected:
    QGridLayout* LayoutB;
};

#endif // _SCOREWIDGET_H

