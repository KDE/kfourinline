/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2001 by Martin Heni
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

#include "kspritecache.h"

#include <kconfig.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qwmatrix.h>
#include <qdir.h>
#include <kdebug.h>

// KSprite
#include <math.h>

KSpriteCache::KSpriteCache(QString grafixdir, QObject* parent,const char * name)
  : QObject(parent,name)
{
  kdDebug(11002) << "KSpriteCache:: grafixdir=" << grafixdir << endl;
  mConfig=0;
  mCanvas=0;
  setRcFile(QString("grafix.rc"));
  setGrafixDir(grafixdir);
  kdDebug(11002) << "Grafixdir=" << grafixDir() << " rcfile=" << rcFile() << endl;
  reset();
}

KSpriteCache::~KSpriteCache()
{
  kdDebug(11002) << "KSpriteCache: ItemDict=" << mItemDict.count() << endl;
  kdDebug(11002) << "KSpriteCache: CloneDict=" << mCloneDict.count() << endl;
  reset();
  delete mConfig;
}

void KSpriteCache::setRcFile(QString name)
{
  mRcFile=name;
}
bool KSpriteCache::setGrafixDir(QString name)
{
  delete mConfig;

  QDir dir(name);
  QString d;
  d=dir.absPath()+QString("/");
  QString file=d+rcFile();

  // TODO check for filename
  kdDebug(11002) << "Opening config " << file << endl;
  mConfig=new KConfig(file,false,false);
  mGrafixDir=d;
  return true;
}

void KSpriteCache::reset()
{
  mItemDict.setAutoDelete(false);
  mCloneDict.setAutoDelete(false);
  mItemDict.clear();
  mCloneDict.clear();
}


void KSpriteCache::deleteAllItems()
{
  QDictIterator<QCanvasItem> it( mItemDict );
  //kdDebug(11002)  << "KSpriteCache::deleteAllItems items in cache=" << mItemDict.size() << endl;
  while ( it.current() )
  {
    QCanvasItem *item=it.current();
    mItemDict.remove(it.currentKey());
    delete item;
  }
}
void KSpriteCache::deleteItem(QString s,int no)
{
  QCanvasItem *item;
  QString name=s+QString("_%1").arg(no);
  //kdDebug(11002) << "KSpriteCache::deleteItem name=" << name << endl;
  item=mItemDict[name];
  if (item)
  {
    mItemDict.remove(name);
//    kdDebug(11002) << "deleteitem "<<name<<" as sprite="<<item<<endl;
    delete item;
  }
}

void KSpriteCache::deleteItem(QCanvasItem *item)
{
  QDictIterator<QCanvasItem> it( mItemDict );
  while ( it.current() )
  {
    if (item==it.current())
    {
 //     kdDebug(11002) << "KSpriteCache::deleteitem sprite="<<item<<" it="<<it.currentKey()<<endl;
      mItemDict.remove(it.currentKey());
      delete item;
      return ;
    }
    ++it;
  }
}



QCanvasItem *KSpriteCache::getItem(QString name,int no)
{

  QString dictname=name+QString("_%1").arg(no);
  QCanvasItem *item=mItemDict[dictname];
  //kdDebug(11002) << " -> getItem("<<name<<","<<no<<") =>"<<dictname<<endl;
  // Directly found item
  if (item)
  {
    //kdDebug(11002) << "found item "<<dictname<<" as directly existing "<<item << endl;
    return item;
  }

  item=mCloneDict[name];
  // load item
  if (!item)
  {
    // Now first time load the items
    if (!mConfig->hasGroup(name))
    {
      kdError() << "Item "<<name <<" not defined! " <<endl;
      return 0;
    }
    mConfig->setGroup(name);
    item=loadItem(mConfig,name);
    // kdDebug(11002) << "Inserting sprite="<<item << " as " << name << " into CloneDict"<< endl;
    mCloneDict.insert(name,item);
  }

  // Clone new item
  item=cloneItem(item);
  mItemDict.insert(dictname,item);
  // kdDebug(11002) << "Inserting sprite="<<item << " as " << dictname << " into ItemDict"<< endl;

  return item;
}
QPixmap * KSpriteCache::loadPixmap(QString file,QString mask,QString dir)
{
  QPixmap *newP=new QPixmap;
  bool result1=false;
  bool result2=false;
  if (dir.isNull()) dir=grafixDir(); // default dir
  if (!file.isNull())
  {
    result1=newP->load(dir+file);
  }
  if (result1 && !mask.isNull())
  {
    QBitmap bitmask;
    if (mask==QString("auto"))  // self mask..slow but quick to create
    {
      newP->setMask( newP->createHeuristicMask() );
      result2=true;
    }
    else
    {
      result2=bitmask.load(dir+mask);
      if (result2) newP->setMask(bitmask);
    }
  }
  //kdDebug(11002) << "KSpriteCache::loadPixmap: file="<<file<<"   mask="<<mask<<"   result1="<<result1<<"   result2="<<result2<<endl;
  return newP;
}



QCanvasPixmapArray *KSpriteCache::createPixmapArray(KConfig *config,QString name)
{
  config->setGroup(name);
  QPoint defaultoffset=QPoint(0,0);
  // offset for the sprite
  QPoint offset=config->readPointEntry("offset",&defaultoffset);

  // operatins to perform. Can be ommited if you want only one operation
  QStringList operationList=config->readListEntry("pixmaps");
  // Append default entry (empty string)
  if (operationList.count()==0)
  {
    operationList.append(QString::null);
  }


  // Prepare for the reading of the pixmaps
  QPixmap *pixmap=0;
  QPtrList<QPixmap> pixlist;
  pixlist.setAutoDelete(true);
  QPtrList<QPoint> hotlist;
  hotlist.setAutoDelete(true);

  // work through the operations list and create pixmaps
  for ( QStringList::Iterator it = operationList.begin(); it !=operationList.end(); ++it )
  {
    QString name=*it;
    // Try to find out what we want to do, e.g. load, scale, ...
    QString type=config->readEntry(name+"method");
    if (type.isNull()) type=QString("load"); // default load
    //kdDebug(11002) << " Processing operation " << (name.isNull()?"default":name) << "type="<<type << endl;

    unsigned int number=config->readNumEntry(name+"number",1);
    //kdDebug(11002) << " Reading " << number << " frames " << endl;

    QString pixfile=config->readPathEntry(name+"file");
    QString maskfile=config->readPathEntry(name+"mask");

    // Load a given set of images or replace a %d by a sequence if there are
    // less image names than number given
    if (type==QString("load"))
    {
      // Read images
      for (unsigned int i=0;i<number;i++)
      {
        QString tmpfile,tmpmask;
        tmpfile.sprintf(pixfile.latin1(),i);
        tmpmask.sprintf(maskfile.latin1(),i);

        pixmap=loadPixmap(tmpfile,tmpmask);
        if (!pixmap) kdError() << "Could not create pixmap="<<tmpfile << " with mask " << tmpmask << endl;
        else
        {
          applyFilter(pixmap,config,name);

          pixlist.append(pixmap);
          QPoint *copyoffset=new QPoint(-offset);
          hotlist.append(copyoffset);
        }
      }
    }
    // Scale some images in given axis
    else if (type==QString("scale"))
    {
      // scale images
      int axis=config->readNumEntry(name+"axis",0);
      double finalscale=config->readDoubleNumEntry(name+"final",0.0);
      double step;
      if (number>1) step=(100.0-finalscale)/100.0/(double)(number-1);
      else step=1.0;
      //kdDebug(11002) << " Scaling " << number << " pics axis="<<axis<<" final="<<finalscale<<" file="<<pixfile<< " mask="<<maskfile<<endl;

      pixmap=loadPixmap(pixfile,maskfile);
      for (unsigned int j=0;j<(unsigned int)number;j++)
      {
        QWMatrix matrix;
        double sc=1.0-(double)(j)*step;

        // scale it
        if (axis==1) matrix.scale(sc,1.0);
        else if (axis==2) matrix.scale(1.0,sc);
        else matrix.scale(sc,sc);

        QPixmap *copypixmap=new QPixmap(pixmap->xForm(matrix));

        applyFilter(copypixmap,config,name);

        pixlist.append(copypixmap);
        QPoint *copyoffset=new QPoint((-pixmap->width()+copypixmap->width())/2,(-pixmap->height()+copypixmap->height())/2);
        hotlist.append(copyoffset);
      }
      delete pixmap;

    }
    else
    {
      kdDebug(11002) << "WARNING: Unknown algorithm " << type << " for " << name << " not supported " << endl;
    }
  }// end create images

 //kdDebug(11002) <<"Pixarray count="<<pixlist.count()<<endl;
 if (pixlist.count()<1) return 0;

 QCanvasPixmapArray *pixmaparray=new QCanvasPixmapArray(pixlist,hotlist);
 return pixmaparray;
}

void KSpriteCache::applyFilter(QPixmap *pixmap,KConfig *config,QString name)
{
  QValueList<int> filterList;
  filterList=config->readIntListEntry(name+"colorfilter");
  QValueList<int> transformList;
  transformList=config->readIntListEntry(name+"transformfilter");

  // apply transformation filter
  if (transformList.count()>0)
  {
    if (transformList[0]==1 && transformList.count()==2)  // rotate
    {
      QWMatrix rotate;
      rotate.rotate(transformList[1]);
      *pixmap=pixmap->xForm(rotate);
    }
    else if (transformList[0]==2 && transformList.count()==3) // scale
    {
      QWMatrix scale;
      scale.scale((double)transformList[1]/100.0,(double)transformList[2]/100.0);
      *pixmap=pixmap->xForm(scale);
    }
  }
  // apply colorfilter
  if (filterList.count()>0)
  {
    // Only filter 1 HSV and 2: grey are implemented
    if (filterList[0]==1 && filterList.count()==4) changeHSV(pixmap,filterList[1],filterList[2],filterList[3]);
    else if (filterList[0]==2 && filterList.count()==2) changeGrey(pixmap,filterList[1]);
    else if (filterList[0]==2 && filterList.count()==1) changeGrey(pixmap);
    else kdWarning(11002) << "WARNING: Colorfilter parameter incorrect "<< endl;
  }
}

void KSpriteCache::changeHSV(QPixmap *pixmap,int dh,int ds,int dv)
{
  if (!pixmap || (dh==0 && ds==0 && dv==0)) return ;
  if (pixmap->isNull()) return ;
  if (pixmap->width()==0 && pixmap->height()==0) return ;

  int h,s,v;
  QColor black=QColor(0,0,0);
  QImage img=pixmap->convertToImage();  // slow

  for (int y=0;y<img.height();y++)
  {
    for (int x=0;x<img.width();x++)
    {
      QRgb pix=img.pixel(x,y);
      QColor col(pix);
      col.hsv(&h,&s,&v);
      if (col==black) continue;  // speed up?
      h=((unsigned int)(h+dh))%360;
      s=((unsigned int)(s+ds)%256);
      v=((unsigned int)(v+dv)%256);
      col.setHsv(h,s,v);
      img.setPixel(x,y,qRgba(col.red(),col.green(),col.blue(),qAlpha(pix)));
    }
  }
  pixmap->convertFromImage(img); // slow
}
void KSpriteCache::changeGrey(QPixmap *pixmap,int lighter)
{
  if (!pixmap) return ;
  if (pixmap->isNull()) return ;
  if (pixmap->width()==0 && pixmap->height()==0) return ;

  QImage img=pixmap->convertToImage();  // slow

  for (int y=0;y<img.height();y++)
  {
    for (int x=0;x<img.width();x++)
    {
      QRgb pix=img.pixel(x,y);
      int gray=qGray(qRed(pix),qGreen(pix),qBlue(pix));
      QColor col(gray,gray,gray);
      if (lighter>0) col=col.light(lighter);
      if (lighter<0) col=col.dark(-lighter);
      img.setPixel(x,y,qRgba(col.red(),col.green(),col.blue(),qAlpha(pix)));
    }
  }
  pixmap->convertFromImage(img); // slow
}

QCanvasItem *KSpriteCache::loadItem(KConfig *config,QString name)
{
  if (!config) return 0;
  int rtti=config->readNumEntry("rtti",0);
  QCanvasItem *item=0;
  switch(rtti)
  {
    case QCanvasItem::Rtti_Text:
    {
      QCanvasText *sprite=new QCanvasText(canvas());
      //kdDebug(11002) << "new CanvasText =" << sprite << endl;
      QString text=config->readEntry("text");
      sprite->setText(text);
      QColor color=config->readColorEntry("color");
      sprite->setColor(color);
      QFont font=config->readFontEntry("font");
      sprite->setFont(font);
      item=(QCanvasItem *)sprite;
      configureCanvasItem(config,item);
    }
    break;
    case 32:
    {
      QCanvasPixmapArray  *pixmaps=createPixmapArray(config,name);
      KSprite *sprite=new KSprite(pixmaps,canvas());
      //kdDebug(11002) << "new sprite =" << sprite << endl;
      double speed=config->readDoubleNumEntry("speed",0.0);
      sprite->setSpeed(speed);
      //kdDebug(11002) << "speed=" << sprite->speed() << endl;
      createAnimations(config,sprite);

      item=(QCanvasItem *)sprite;
      configureCanvasItem(config,item);

    }
    break;
    default:
    {
      kdError() << "KSpriteCache::loadItem: Should create unkwown rtti " << rtti << "...overwrite this function" << endl;
    }
    break;
  }
  return item;
}

QCanvasItem *KSpriteCache::cloneItem(QCanvasItem *original)
{
  if (!original) return 0;
  int rtti=original->rtti();
  QCanvasItem *item=0;
  switch(rtti)
  {
    case QCanvasItem::Rtti_Text:
    {
      QCanvasText *sprite=(QCanvasText *)original;
      QCanvasText *copy=new QCanvasText(canvas());
      configureCanvasItem(original,(QCanvasItem *)copy);
      copy->setText(sprite->text());
      copy->setColor(sprite->color());
      copy->setFont(sprite->font());
      item=(QCanvasItem *)copy;
    }
    break;
    case 32:
    {
      KSprite *sprite=(KSprite *)original;
      KSprite *copy=new KSprite(sprite->images(),canvas());
      configureCanvasItem(original,(QCanvasItem *)copy);
      copy->setSpeed(sprite->speed());
      createAnimations(sprite,copy);
      item=(QCanvasItem *)copy;
    }
    break;
    default:
    {
      kdError() << "KSpriteCache::cloneItem: Should create unkwown rtti " << rtti << "...overwrite this function" << endl;
    }
    break;
  }
  return item;
}


void KSpriteCache::configureCanvasItem(KConfig *config, QCanvasItem *sprite)
{
  double x=config->readDoubleNumEntry("x",0.0);
  double y=config->readDoubleNumEntry("y",0.0);
  double z=config->readDoubleNumEntry("z",0.0);
  sprite->setX(x);
  sprite->setY(y);
  sprite->setZ(z);
  //kdDebug(11002) << "x=" << sprite->x() << endl;
  //kdDebug(11002) << "y=" << sprite->y() << endl;
  //kdDebug(11002) << "z=" << sprite->z() << endl;
}

void KSpriteCache::configureCanvasItem(QCanvasItem *original, QCanvasItem *copy)
{
  copy->setX(original->x());
  copy->setY(original->y());
  copy->setZ(original->z());
}


void KSpriteCache::createAnimations(KSprite *original,KSprite *sprite)
{
  unsigned int no=original->animationCount();
  for (unsigned int i=0;i<no;i++)
  {
    int start,end,mode,delay;
    original->getAnimation(i,start,end,mode,delay);
    sprite->createAnimation(i,start,end,mode,delay);
  }
}

void KSpriteCache::createAnimations(KConfig *config,KSprite *sprite)
{
  if (!sprite) return ;
  for (int i=0;i<1000;i++)
  {
    QString anim=QString("anim%1").arg(i);
    if (config->hasKey(anim))
    {
      //kdDebug(11002) << "Found animation key " << anim << endl;
      QValueList<int> animList=config->readIntListEntry(anim);
      if (animList.count()!=4)
      {
        kdWarning(11002) << "KSpriteCache::createAnimations:: warning animation parameter " << anim << " needs four arguments" << endl;
      }
      else
      {
        sprite->createAnimation(i,animList[0],animList[1],animList[2],animList[3]);
      }
    }
    else
    {
      break;
    }
  }
}


// ----------------------- KSPRITE --------------------------------
KSprite::KSprite(QCanvasPixmapArray* array, QCanvas* canvas)
          :QCanvasSprite(array,canvas)
{
  mImages=array;
  mSpeed=0.0;
  mNotify=0;
  mAnimationNumber=-1;
  mAnimSpeedCnt=0;
  mMoveObj=0;

}

void KSprite::moveTo(double tx,double ty,double speed)
{
  if (speed>0.0)
  {
    mSpeed=speed;
  }

  //kdDebug(11002) <<"KSprite::moveTo x=" << tx << " y="<<ty<< " speed=" << mSpeed<<endl;
  mTargetX=tx;
  mTargetY=ty;
  if ((fabs(mTargetX-x())+fabs(mTargetY-y())) >0.0)
  {
    //kdDebug(11002) << "     animation on" << endl;
    setAnimated(true);
  }
  else
  {
    //kdDebug(11002) << "     animation NOT on ihn moveTO" << endl;
  }
}

void KSprite::advance(int stage)
{
  if (stage!=1) return ;
  if (!isVisible()) return ;

  int emitsignal=0;
  bool isMoving=false;
  bool isAnimated=true;

  // Animation
  // mode  0: no animation
  //       1: single shot a->b
  //      -1: single shot b->a
  //       2: cycle  a->b->a
  //      -2: cycle  b->a->b
  //       3: cycle  a->b
  //      -3: cycle  b->a
  mAnimSpeedCnt++;
  if (mAnimationNumber<0 || mAnimDelay[mAnimationNumber]==0)
  {
    // nothing to do?
    isAnimated=false;
    mAnimSpeedCnt=0;
  }
  if (mAnimationNumber>=0 && mAnimSpeedCnt>=mAnimDelay[mAnimationNumber])
  {
    switch(mAnimDirection[mAnimationNumber])
    {
      case 1:
        if (frame()+1 <= mAnimTo[mAnimationNumber]) setFrame(frame()+1);
        else emitsignal=2;
      break;
      case -1:
        if (frame()-1 >= mAnimFrom[mAnimationNumber]) setFrame(frame()-1);
        else emitsignal=2;
      break;
      case 2:
        if (mAnimDirection[mAnimationNumber]==mCurrentAnimDir)
        {
          if (frame()+1 <= mAnimTo[mAnimationNumber]) setFrame(frame()+1);
          else
          {
            mCurrentAnimDir=-mCurrentAnimDir;
            if (frame()>0) setFrame(frame()-1);
          }
        }
        else
        {
          if (frame()-1 >= mAnimFrom[mAnimationNumber]) setFrame(frame()-1);
          else
          {
            mCurrentAnimDir=-mCurrentAnimDir;
            if (frame()+1<frameCount()) setFrame(frame()+1);
          }
        }
      break;
      case -2:
        if (mAnimDirection[mAnimationNumber]==mCurrentAnimDir)
        {
          if (frame()-1 >= mAnimFrom[mAnimationNumber]) setFrame(frame()-1);
          else
          {
            mCurrentAnimDir=-mCurrentAnimDir;
            if (frame()+1<frameCount()) setFrame(frame()+1);
          }
        }
        else
        {
          if (frame()+1 <= mAnimTo[mAnimationNumber]) setFrame(frame()+1);
          else
          {
            mCurrentAnimDir=-mCurrentAnimDir;
            if (frame()>0) setFrame(frame()-1);
          }
        }
      break;
      case 3:
        if (frame()+1 <= mAnimTo[mAnimationNumber]) setFrame(frame()+1);
        else setFrame(mAnimFrom[mAnimationNumber]);
      break;
      case -3:
        if (frame()-1 >= mAnimFrom[mAnimationNumber]) setFrame(frame()-1);
        else setFrame(mAnimTo[mAnimationNumber]);
      break;
      default: //0
        isAnimated=false;
        setFrame(0);
    }
    if (emitsignal) isAnimated=false;

    mAnimSpeedCnt=0;
  }



  // Movement to target
  if (!moveObject() && (fabs(mTargetX-x())+fabs(mTargetY-y())) >0.0 && mSpeed>0.0)
  {
    isMoving=spriteMove(mTargetX,mTargetY);
    if (!isMoving) emitsignal=1;
  }
  else if (moveObject())
  {
    isMoving=moveObject()->spriteMove(mTargetX,mTargetY,this);
    if (!isMoving) emitsignal=1;
  }


  // Final checks
  if (!isAnimated && !isMoving)
  {
    //kdDebug(11002) << "Animation over" << endl;
    setAnimated(false);
  }

  if (mNotify && emitsignal)
  {
    //kdDebug(11002) << " ADVANCE emits signal " << emitsignal << " for item "<< this << endl;
    mNotify->emitSignal((QCanvasItem *)this,emitsignal);
  }
}

// Generates linear movement to tx,ty
bool KSprite::spriteMove(double tx,double ty)
{
  bool isMoving=false;
  double dx,dy;
  double vx,vy;
  dx=tx-x();
  dy=ty-y();
  vx=0.0;
  vy=0.0;

  // first pure x,y movements
  if (fabs(dy)<=0.0001)
  {
    if (dx>0.0) vx=mSpeed;
    else if (dx<0.0) vx=-mSpeed;
    else vx=0.0;
  }
  else if (fabs(dx)<=0.0001)
  {
    if (dy>0.0) vy=mSpeed;
    else if (dy<0.0) vy=-mSpeed;
    else vy=0.0;
  }
  else // diagonal
  {
    double alpha=atan2(dy,dx);
    vx=cos(alpha)*mSpeed;
    vy=sin(alpha)*mSpeed;
  }

  if (fabs(dx)<=fabs(vx) && fabs(dy)<=fabs(vy))
  {
    move(tx,ty);
    isMoving=false;
  }
  else if (fabs(dx)<=fabs(vx))
  {
    moveBy(dx,vy);
    isMoving=true;
  }
  else if (fabs(dy)<=fabs(vy))
  {
    moveBy(vx,dy);
    isMoving=true;
  }
  else
  {
    moveBy(vx,vy);
    isMoving=true;
  }
  return isMoving;
}

void KSprite::emitNotify(int mode)
{
  if (!mNotify) return ;
  //kdDebug(11002) << " ADVANCE emits DIRECT signal " << mode << " for item "<< this << endl;
  mNotify->emitSignal((QCanvasItem *)this,mode);
}
QObject *KSprite::createNotify()
{
  if (!mNotify) mNotify=new KSpriteNotify;
  mNotify->incRefCnt();
  return (QObject *)mNotify;
}

void KSprite::deleteNotify()
{
  if (!mNotify) return ;
  mNotify->decRefCnt();
  if (mNotify->refCnt()<=0)
  {
    //kdDebug(11002) << "REALLY deleting notify" << endl;
    delete mNotify;
    mNotify=0;
  }
}

KSprite::~KSprite()
{
  delete mNotify;
  mNotify=0;
}

void KSprite::setAnimation(int no)
{
  if ((int)mAnimFrom.count()<=no)
  {
    kdError(11002) << "KSprite::setAnimation:: Animation " << no << " not defined " << endl;
    return ;
  }
  mAnimationNumber=no;
  if (no<0) return ;
  mAnimSpeedCnt=0;
  mCurrentAnimDir=mAnimDirection[no];

  // Start frame
  if (mCurrentAnimDir>0) setFrame(mAnimFrom[no]);
  else if (mCurrentAnimDir<0) setFrame(mAnimTo[no]);

  // animated
  if (mCurrentAnimDir!=0 && mAnimTo[no]>=mAnimFrom[no]) setAnimated(true);
  else setAnimated(false);

  //kdDebug(11002) << this << " setAnimation("<<no<<") delay="<<mAnimDelay[no]<<" frames="<<mAnimFrom[no]<<"->"<<mAnimTo[no]<<" mode="<<mAnimDirection[no]<<" animated="<<animated()<<endl;
}

void KSprite::getAnimation(int no,int &startframe,int &endframe,int &mode,int &delay)
{
  if ((int)mAnimFrom.count()<=no) return ;
  startframe=mAnimFrom[no];
  endframe=mAnimTo[no];
  mode=mAnimDirection[no];
  delay=mAnimDelay[no];
}

void KSprite::createAnimation(int no,int startframe,int endframe,int mode,int delay)
{
  //kdDebug(11002) << this << " createAnimation " << no << endl;
  // resize?
  if ((int)mAnimFrom.count()<=no)
  {
    mAnimFrom.resize(no+1);
    mAnimTo.resize(no+1);
    mAnimDirection.resize(no+1);
    mAnimDelay.resize(no+1);
  }
  mAnimFrom[no]=startframe;
  mAnimTo[no]=endframe;
  mAnimDirection[no]=mode;
  mAnimDelay[no]=delay;
  //kdDebug(11002) << "from=" << startframe << " to="<<endframe<<" mode="<<mode<<" delay="<<delay<<endl;
}

#include "kspritecache.moc"
