#include "statuswidget.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qsizepolicy.h>
#include <kdebug.h>
#include <klocale.h>

#define COL_STATUSBORDER black
#define COL_STATUSFIELD_L  QColor(130,130,255).light(110)
#define COL_STATUSFIELD_D  QColor(130,130,255).light(120)
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)

StatusWidget::StatusWidget( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
  if ( !name ) setName( "StatusWidget" );
  //resize( 236, 201 ); 
  setBackgroundColor( COL_STATUSFIELD );

  kdDebug(12010) << "StatusWidget:" << this->layout() << endl;

   //resize( 255, 187 ); 
   int row=0;
   int col=0;

    //setCaption( tr( "Form1" ) );
    LayoutB = new QGridLayout( this);
    LayoutB->setSpacing( 3 );
    LayoutB->setMargin( 15 );

    col=1;
    QSpacerItem *Spacer0=new QSpacerItem(16,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer0, row, row+3,col,col );
    col++;

    Label[0][1] = new KWLabel( this, "01" );
    Label[0][1]->setText( i18n( "1-2_letter_abbr_won","W" ) );
    Label[0][1]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][1]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][1], row, col );
    col++;
    Label[0][2] = new KWLabel( this, "02" );
    Label[0][2]->setText( i18n( "1-2_letter_abbr_drawn","D" ) );
    Label[0][2]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][2]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][2], row, col );
    col++;
    Label[0][3] = new KWLabel( this, "03" );
    Label[0][3]->setText( i18n("1-2_letter_abbr_lost","L" ) );
    Label[0][3]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][3]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][3], row, col );
    col++;
    QSpacerItem *Spacer1=new QSpacerItem(16,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer1, row, row+3,col,col );
    col++;
    Label[0][4] = new KWLabel( this, "04" );
    Label[0][4]->setText( i18n("1-2_letter_abbr_number","No" ) );
    Label[0][4]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][4]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][4], row, col );
    col++;
    Label[0][5] = new KWLabel( this, "05" );
    Label[0][5]->setText( i18n( "1-2_letter_abbr_breaks/aborted","Bk" ) );
    Label[0][5]->setBackgroundColor( COL_STATUSFIELD );
    Label[0][5]->setAlignment(Qt::AlignHCenter);
    LayoutB->addWidget( Label[0][5], row, col );
    col++;

    row++;

    QSpacerItem *Spacer2=new QSpacerItem(0,3,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer2, row, row,0,col-1 );

    col=0;
    row++;

    Label[1][0] = new KWLabel( this, "10" );
    setPlayer("-------------------",0);
    Label[1][0]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][0]->setAlignment(Qt::AlignLeft);
    LayoutB->addWidget( Label[1][0], row, col );
    col++;
    col++;
    Label[1][1] = new KWLabel( this, "11" );
    setWin(0,0);
    Label[1][1]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][1]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][1], row, col );
    col++;
    Label[1][2] = new KWLabel( this, "12" );
    setRemis(0,0);
    Label[1][2]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][2]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][2], row, col );
    col++;
    Label[1][3] = new KWLabel( this, "13" );
    setLost(0,0);
    Label[1][3]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][3]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][3], row, col );
    col++;
    col++;
    Label[1][4] = new KWLabel( this, "14" );
    setSum(0,0);
    Label[1][4]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][4]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][4], row, col );
    col++;
    Label[1][5] = new KWLabel( this, "15" );
    setBrk(0,0);
    Label[1][5]->setBackgroundColor( COL_STATUSFIELD_L );
    Label[1][5]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[1][5], row, col );
    col++;


    col=0;
    row++;

    Label[2][0] = new KWLabel( this, "20" );
    setPlayer("-----",1);
    Label[2][0]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][0]->setAlignment(Qt::AlignLeft);
    LayoutB->addWidget( Label[2][0], row, col );
    col++;
    col++;
    Label[2][1] = new KWLabel( this, "21" );
    setWin(0,1);
    Label[2][1]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][1]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][1], row, col );
    col++;
    Label[2][2] = new KWLabel( this, "22" );
    setRemis(0,1);
    Label[2][2]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][2]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][2], row, col );
    col++;
    Label[2][3] = new KWLabel( this, "23" );
    setLost(0,1);
    Label[2][3]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][3]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][3], row, col );
    col++;
    col++;
    Label[2][4] = new KWLabel( this, "24" );
    setSum(0,1);
    Label[2][4]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][4]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][4], row, col );
    col++;
    Label[2][5] = new KWLabel( this, "25" );
    setBrk(0,1);
    Label[2][5]->setBackgroundColor( COL_STATUSFIELD_D );
    Label[2][5]->setAlignment(Qt::AlignRight);
    LayoutB->addWidget( Label[2][5], row, col );
    col++;


    adjustSize();
}

StatusWidget::~StatusWidget()
{
}

void StatusWidget::paintEvent( QPaintEvent * p)
{
    QPainter paint( this );
    paint.setClipRect(p->rect());
    Paint( &paint, p->rect() );			
}
void StatusWidget::Paint(QPainter *p,QRect cliprect)
{
  QRect lrect;

  // Draw border and field
  p->setPen(COL_STATUSBORDER);
  p->setBrush(COL_STATUSFIELD);
  p->drawRect(rect());

  p->setPen(COL_STATUSFIELD_L);
  p->setBrush(COL_STATUSFIELD_L);
  lrect=LayoutB->cellGeometry(2,0)|LayoutB->cellGeometry(2,7);
  p->drawRect(lrect);

  p->setPen(COL_STATUSFIELD_D);
  p->setBrush(COL_STATUSFIELD_D);
  lrect=LayoutB->cellGeometry(3,0)|LayoutB->cellGeometry(3,7);
  p->drawRect(lrect);

  
  p->setPen(COL_STATUSBORDER);
  drawBorder(p,rect(),0,4,0);
	drawBorder(p,rect(),10,1,1);


}

void StatusWidget::drawBorder(QPainter *p,QRect rect,int offset,int width,int mode)
{
  QPen graypen;
  int i;
  QPoint p1,p2;

	if (mode!=0 && mode!=1) return;

	p1=rect.topLeft();
	p2=rect.bottomRight();

	if (mode==0) p->setPen(COL_STATUSLIGHT);
	else if (mode==1) p->setPen(COL_STATUSDARK);
	
	for (i=0;i<width;i++)
	{
		p->moveTo(p1.x()+offset+i,p2.y()-offset-i);
		p->lineTo(p1.x()+offset+i,p1.y()+offset+i);
		p->lineTo(p2.x()-offset-i,p1.y()+offset+i);
	}
	if (mode==1) p->setPen(COL_STATUSLIGHT);
	else if (mode==0) p->setPen(COL_STATUSDARK);
	for (i=0;i<width;i++)
	{
		p->moveTo(p1.x()+offset+i,p2.y()-offset-i);
		p->lineTo(p2.x()-offset-i,p2.y()-offset-i);
		p->lineTo(p2.x()-offset-i,p1.y()+offset+i);
	}
}

void StatusWidget::setPlayer(QString s,int no)
{
  if (no==0)  Label[1][0]->setText(s);
  else Label[2][0]->setText(s);
  adjustSize();
}
void StatusWidget::setWin(int i,int no)
{
  if (no==0)  Label[1][1]->setText(QString("%1").arg(i));
  else Label[2][1]->setText( QString("%1").arg(i) );
  adjustSize();
}
void StatusWidget::setRemis(int i,int no)
{
  if (no==0)  Label[1][2]->setText(QString("%1").arg(i));
  else Label[2][2]->setText( QString("%1").arg(i) );
  adjustSize();
}
void StatusWidget::setLost(int i,int no)
{
  if (no==0)  Label[1][3]->setText(QString("%1").arg(i));
  else Label[2][3]->setText( QString("%1").arg(i) );
  adjustSize();
}
void StatusWidget::setSum(int i,int no)
{
  if (no==0)  Label[1][4]->setText(QString("%1").arg(i));
  else Label[2][4]->setText( QString("%1").arg(i) );
  adjustSize();
}
void StatusWidget::setBrk(int i,int no)
{
  if (no==0)  Label[1][5]->setText(QString("%1").arg(i));
  else Label[2][5]->setText( QString("%1").arg(i) );
  adjustSize();
}
#include "statuswidget.moc"
