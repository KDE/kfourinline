#include "scorewidget.h"
#include "kwlabel.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qsizepolicy.h>
#include <kdebug.h>
#include <klocale.h>

#define COL_STATUSBORDER black
#define COL_STATUSFIELD  QColor(130,130,255)
#define COL_STATUSDARK   QColor(0,0,65)
#define COL_STATUSLIGHT  QColor(210,210,255)

ScoreWidget::ScoreWidget( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
  if ( !name ) setName( "ScoreWidget" );
  //resize( 236, 201 ); 
  setBackgroundColor( COL_STATUSFIELD );

  kdDebug(12010) << "ScoreWidget:" << this->layout() << endl;

   resize( 255, 187 ); 
   int row=0;

    setCaption( i18n( "Form1" ) );
    //LayoutB = new QGridLayout( this,4,3,15,5 ); 
    LayoutB = new QGridLayout( this);
    LayoutB->setSpacing( 3 );
    LayoutB->setMargin( 15 );

    TextLabel7 = new KWLabel( this, "TextLabel7" );
    setPlayer("-----",0);
    TextLabel7->setBackgroundColor( COL_STATUSFIELD );
    TextLabel7->setAlignment(Qt::AlignHCenter);
    LayoutB->addMultiCellWidget( TextLabel7, row, row,0,2 );
    row++;
    
    TextLabel8 = new KWLabel( this, "TextLabel8" );
    TextLabel8->setText( i18n( "vs" ) );
    TextLabel8->setBackgroundColor( COL_STATUSFIELD );
    TextLabel8->setAlignment(Qt::AlignHCenter);
    LayoutB->addMultiCellWidget( TextLabel8, row, row,0,2 );
    row++;
    
    TextLabel9 = new KWLabel( this, "TextLabel9" );
    setPlayer("-----",1);
  //  TextLabel9->setFrameShape(QFrame::Box );
  //  TextLabel9->setLineWidth(5);
    TextLabel9->setBackgroundColor( COL_STATUSFIELD );
    TextLabel9->setAlignment(Qt::AlignHCenter);
    LayoutB->addMultiCellWidget( TextLabel9, row, row,0,2 );
    row++;

    QSpacerItem *Spacer2=new QSpacerItem(0,16,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer2, row, row,0,2 );
    row++;

    QSpacerItem *Spacer1=new QSpacerItem(25,0,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer1, row, row+2,1,1 );

    TextLabel1 = new KWLabel( this, "Level" );
    TextLabel1->setText( i18n( "Level" ) );
    TextLabel1->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel1, row, 0 );

    TextLabel4 = new KWLabel( this, "L" );
    setLevel(0);
    TextLabel4->setAlignment(Qt::AlignRight);
    TextLabel4->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel4, row, 2 );

    row++;


    TextLabel2 = new KWLabel( this, "Move" );
    TextLabel2->setText( i18n("number of MOVE in game", "Move" ) );
    TextLabel2->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel2, row, 0 );

    TextLabel5 = new KWLabel( this, "M" );
    setMove(0);
    TextLabel5->setAlignment(Qt::AlignRight);
    TextLabel5->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel5, row, 2 );

    row++;


    TextLabel3 = new KWLabel( this, "Chance" );
    TextLabel3->setText( i18n( "Chance" ) );
    TextLabel3->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel3, row, 0 );

    TextLabel6 = new KWLabel( this, "C" );
    setChance(0);
    TextLabel6->setAlignment(Qt::AlignRight);
    TextLabel6->setBackgroundColor( COL_STATUSFIELD );
    LayoutB->addWidget( TextLabel6, row, 2 );

    row++;

    QSpacerItem *Spacer3=new QSpacerItem(0,8,QSizePolicy::Preferred,QSizePolicy::Preferred);
    LayoutB->addMultiCell( Spacer3, row, row,0,2 );
    row++;



    adjustSize();
}

ScoreWidget::~ScoreWidget()
{
}

void ScoreWidget::paintEvent( QPaintEvent * p)
{
    QPainter paint( this );
    paint.setClipRect(p->rect());
    Paint( &paint, p->rect() );			
}
void ScoreWidget::Paint(QPainter *p,QRect cliprect)
{
  // Draw border and field
  p->setPen(COL_STATUSBORDER);
  p->setBrush(COL_STATUSFIELD);
  p->drawRect(rect());
  drawBorder(p,rect(),0,4,0);
	drawBorder(p,rect(),10,1,1);

}

void ScoreWidget::drawBorder(QPainter *p,QRect rect,int offset,int width,int mode)
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

void ScoreWidget::setMove(int i)
{
  TextLabel5->setText( QString("%1").arg(i));
}
void ScoreWidget::setLevel(int i)
{
  TextLabel4->setText( QString("%1").arg(i));
}
void ScoreWidget::setChance(int i)
{
  if (i==0)
    TextLabel6->setText("    ----");
  else if (i>=999) 
    TextLabel6->setText(i18n("Winner"));
  else if (i<=-999) 
    TextLabel6->setText(i18n("Loser"));
  else
    TextLabel6->setText(QString("%1").arg(i));
}
void ScoreWidget::setPlayer(QString s,int no)
{
  if (no==0) TextLabel7->setText(s);
  else TextLabel9->setText(s);
}

void ScoreWidget::setTurn(int i)
{
  if (i==0)
  {
    TextLabel7->setForegroundColor( yellow);
    TextLabel9->setForegroundColor(black);
  }
  else
  {
    TextLabel9->setForegroundColor( red);
    TextLabel7->setForegroundColor(black);
  }
  TextLabel7->update();
  TextLabel9->update();
}
#include "scorewidget.moc"
