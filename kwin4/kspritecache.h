/***************************************************************************
                          kspritecache.h
                             -------------------
    begin                : September 2001  by Martin Heni
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

#ifndef _KSPRITECACHE_H
#define _KSPRITECACHE_H
#include <qcanvas.h>
#include <qdict.h>
#include <qstring.h>
#include <qwidget.h>
#include <kconfig.h>
#include <qtimer.h>

class KSprite;

    /**
    * this is an internal class to provide a @ref QObject to emit
    * a signal from a sprite if a notify object is created
    * You do not need this directly.
    * TODO: Can be part of the KSprite class
    **/
    class KSpriteNotify : public QObject
    {
      Q_OBJECT

      public:
        KSpriteNotify() :QObject(0,0) {mRefCnt=0;}
        void emitSignal(QCanvasItem *parent,int mode) {emit signalNotify(parent,mode);}
        void incRefCnt() {mRefCnt++;}
        void decRefCnt() {mRefCnt--;}
        int refCnt() {return mRefCnt;}
      signals:
        void signalNotify(QCanvasItem *,int);
      private:
        int mRefCnt;
    };

    class KSpriteMove 
    {
      public:
        KSpriteMove() {};
        virtual ~KSpriteMove() {};
        virtual bool spriteMove(double ,double ,KSprite *) {return false;}
      private:
    };

/**
 * The KSprite class is an advance QCanvasSprite class which
 * is usable with the @ref KSpriteCache. It furthermore contains a
 * few useful functions like advanced movement and animations which
 * go beyond the QCanvasSprite versions of them. Also it provides
 * a signal which is emitted when movement or animation are finished.
 *
 * @short The main KDE game object
 * @author Martin Heni <martin@heni-online.de>
 *
 */
class KSprite : public QCanvasSprite
{
  public:
    /**
    * Contructs a KSprite object. It is anlogous to the @ref QCanvasSprite
    * constructor
    * 
    * @param array - the frames of the sprite
    * @param canvas - the canvas the sprites lives on
    **/
    KSprite(QCanvasPixmapArray* array, QCanvas* canvas);

    /**
    * Destructs the sprite
    **/
    virtual ~KSprite(); 

    /** 
    * The sprites runtime idendification (32)
    **/
    int rtti() const {return 32;}

    /**
    * returns a pointer to the pixmap array which holds the
    * frames of the sprite.
    **/
    QCanvasPixmapArray* images() const {return mImages;}

    /**
    * Moves the sprite to the given position with the given speed.
    * When it reaches its desitnation a signal is emmited if the
    * emmiter @ref createNotify is enabled
    *
    * @param x - the x coordinate
    * @param y - the y coordinate
    * @param speed - the speed to move . If zero the last set speed is taken
    **/
    void moveTo(double x,double y,double speed=0.0);

    /**
    * Generates a linear move to the target tx,ty from the current
    * position of the sprite with its set speed @ref setSpeed
    * Upon arrival the function returns false to indicate an end of the
    * movment. Otherwise true is returned. 
    * The sprite is moved in this function.
    **/
    bool spriteMove(double tx,double ty);

    /**
    * The sprites advance function. See the qt @ref QcanvasSprite advance
    **/
    void advance(int stage);

    /**
    * Sets the speed for the next move. Can be set with moveTo too.
    * 
    * @param v - the speed in pixel per animation cycle
    **/ 
    void setSpeed(double v) {mSpeed=v;}

    /**
    * returns the speed
    **/
    double speed() {return mSpeed;}

    /**
    * returns the notification QObject. You probably do not need this but
    * @ref createNotify instead
    **/
    QObject *notify() {return (QObject *)mNotify;}

    /**
    * Directly emits the notification signal with the given parameter
    *
    * @param the notification parameter
    **/
    void emitNotify(int mode);

    /**
    * Creates a notification object. You can connect to it and it will emit
    * the signal signalNotify(QCanvasItem *parent, intmode) when a move or
    * animation is finished.
    * Example:
    * <pre>
    *  connect(sprite->createNotify(),SIGNAL(signalNotify(QCanvasItem *,int)),
    *          this,SLOT(moveDone(QCanvasItem *,int)));
    * </pre>
    * In the move done function you best delete the notify again with 
    * @ref deleteNotify
    **/
    QObject *createNotify();

    /**
    * Deletes the sprite notify if it is no longer used. The notify keeps a
    * reference count which deletes the QObject when no reference to it is in
    * use. 
    **/ 
    void deleteNotify();

    /**
    * Reads the animation parameters into the given variables for the given
    * animation. Mostly used by @ref KSpriteCache
    * 
    * @param no - the animation number
    * @param startframe - the first frame of the animation
    * @param endframe - the last frame of the animation
    * @param mode - the mode of the animation see @ref creaetAnimation
    * @param delay - the delay in QCanvas animation cycles between two frames
    **/
    void getAnimation(int no,int &startframe,int &endframe,int &mode,int &delay);

    /**
    * Creates an animation of the sprite between two frames in one of the
    * following modes
    * 0: no animation
    * 1: single shot a->b
    *-1: single shot b->a
    * 2: cycle  a->b->a
    *-2: cycle  b->a->b
    * 3: cycle  a->b 
    *-3: cycle  b->a 
    * 
    * The single shot animations will emit the above mentioned signal over the
    * notify object if it is created.
    * If you load the sprite over the KSpriteCache's config file you need not
    * bother about calling this function.
    **/
    void createAnimation(int no,int startframe,int endframe,int mode,int delay);

    /**
    * Switches on the animation of the given number. Of course it needs to be
    * defined beforehand either via loading the sprite with the 
    * @ref KSpriteCache or be calling @ref createAnimation
    *
    * @param no - the number of the animation
    **/
    void setAnimation(int no);

    /**
    * Returns how many different animations are stored
    **/
    unsigned int animationCount() {return mAnimFrom.count();}

    void setMoveObject(KSpriteMove *m) {mMoveObj=m;}
    KSpriteMove *moveObject() {return mMoveObj;}
    


  protected:
    KSpriteMove *mMoveObj;


  private:
    KSpriteNotify *mNotify;
    QCanvasPixmapArray* mImages;
    QByteArray mAnimFrom;
    QByteArray mAnimTo;
    QByteArray mAnimDirection;
    QByteArray mAnimDelay;

    double mTargetX,mTargetY;
    double mSpeed;
    int mAnimationNumber;
    int mAnimSpeedCnt;
    int mCurrentAnimDir;
};

 


/**
 * The KSpriteCache class is used to load and cache sprites. Loading
 * is done via a @ref KConfig file which contains the definitions of the
 * sprite in text form. Usng this approach allows you to tun the sprites
 * without chaning the sourcecode of the program. This is especially useful if
 * the graphics team is independent of the programmer or if you want to write
 * external themes for your game.
 * Furhtermore the class keeps sprites in memory so that they are fastly
 * reloaded when you use more than one sprite of a given type.
 * 
 * Example:
 * <pre>
 * # Sprite with three frames and a common mask for them
 * # z position given but x,y set manually in the program
 *  [arrow]
 *  file=arrow%d.png
 *  mask=arrow_mask.png
 *  number=3
 *  offset=19,5
 *  rtti=32
 *  z=9.0
 *
 * # Simple sprite which is already positioned correcly. No mask just one file
 *  [board]
 *  file=board.png
 *  rtti=32
 *  x=15.0
 *  y=40.0
 *  z=0.0
 * 
 * # Sprite with one cyclic (2) animation of  5 frames (0-4) and
 * # a slow delay of 8
 *  [star]
 *  anim0=0,4,2,8
 *  file=star%d.png
 *  mask=star%d_mask.png
 *  number=5
 *  offset=19,20
 *  rtti=32
 *  z=100.0
 *
 * </pre>
 *
 * @todo Support single sprites (only one copy in memory)
 *       Support more sprite types (currently KSprite and QCanvasText)
 *
 * @short The main KDE game object
 * @author Martin Heni <martin@heni-online.de>
 *
 */
class KSpriteCache : public QObject
{
  Q_OBJECT


  public:
  /**
  * Create a sprite cache. Usuzally you will need one per program only.
  * 
  * @param grafixdir - the directory where the configuration file and the graphics reside
  **/
  KSpriteCache(QString grafixdir, QObject* parent=0,const char * name=0);

  /**
  * Delete the sprite cache
  **/
  ~KSpriteCache();

  /**
  * Change the grafichs directory.
  * 
  * @todo  this does not flush the cache or so...
  **/
  bool setGrafixDir(QString dir); // dir and load config

  /**
  * Change the name of the config file. Its default is <em>grafix.rc</em>
  **/
  void setRcFile(QString file);

  /**
  * return the graphics directory
  **/
  QString grafixDir() {return mGrafixDir;}

  /**
  * return the rc/configuration file
  **/
  QString rcFile() {return mRcFile;}

  /**
  * returns the canvas which belongs to the cache
  **/
  QCanvas *canvas() const {return mCanvas;}

  /**
  * sets the canvas belonging to the cache
  * 
  * @todo could be done in the constructor
  **/
  void setCanvas(QCanvas *c) {mCanvas=c;}

  /**
  * returns the @ref KConfig configuration file where thegraphical data is
  * read. Access to this is necessary if you want to store general game infos
  * in theis file to or if you want to read additional sprite data which are
  * not read be the default functions.
  **/
  KConfig *config() {return mConfig;}

  /**
  * Main function to create a sprite. You call this like
  * <pre>
  * KSprite *sprite=(KSprite *)(yourcahce->getItem("hello",1));
  * if (sprite) sprite->show();
  * </pre>
  * Calling this function will load the given sprite from the
  * configuration file. Its type is determined by the rtti= entry
  * of the section [hello] in that file. Default is a KSprite.
  * This file defines all data of the sprite so that you just have to show it.
  * Each copy of the sprite gets its own number (1,2,...)
  * Note: The sprite is hidden upon creation and you need to show it
  * explicitely.
  * TODO: What definitions are possible in the rc file
  * 
  * @param name - the name of the sprite resp. the secion in the rc file
  * @param no - the unique number of the sprite 
  * @return sprite - returns the sprite pointer as @ref QCanvasItem
  *
  * @todo support call without number argument as single sprite's
  * @todo support loading of frame sequence via one big pixmap
  *
  **/
  QCanvasItem *getItem(QString name, int no);

  /**
  * This function loads a pixmap from the given file. Optional you can also
  * provide a mask file. Also optinal you can provide the directory. Default
  * is the directory which is set with this @ref KSpriteCache
  **/
  QPixmap * loadPixmap(QString file,QString mask=QString::null,QString dir=QString::null);

  /** 
  * Deletes a item form the sprite cache given as a pointer to it
  **/
  void deleteItem(QCanvasItem *item);

  /**
  * Same as above but delete the item with the name and number
  **/
  void deleteItem(QString s,int no);

  /**
  * Deletes all items in the cache
  **/
  void deleteAllItems();

  protected:
  /**
  * Loads the default properties for all QCanvasItems from the given config
  * file. This is at the moment
  * <pre>
  * x=(double)
  * y=(double)
  * z=(double)
  * </pre>
  **/
  void configureCanvasItem(KConfig *config,QCanvasItem *item);
  
  /**
  * Copies the default properties for all QCanvasItems from another sprite.
  * Same as above.
  **/
  void configureCanvasItem(QCanvasItem *original,QCanvasItem *item);

  /**
  * Loads an item with the given name form the given config file. From the
  * rtti entry it is determined what type it is and then it is loaded.
  **/
  virtual QCanvasItem *loadItem(KConfig *config,QString name);

  /**
  * Clone the sprite from another sprite, mostly from the copy stored in the
  * cache.
  **/
  virtual QCanvasItem *cloneItem(QCanvasItem *original);

  /**
  * Creates a pixmap array for a @ref KSprite from the given config file
  * for the sprite with the given name (is the name necessary?).
  * Parameters are
  * <pre>
  *   offset=(QPoint)       : The sprites offset (where 0,0 is)
  *   pixmaps=(QStringList) : List of operations to create frames (TODO *   rename)
  *                           if ommited one operation without name is used
  * </pre>
  * All following calls have to be preceeded by every given string of the
  * pixmaps section. If this section is not supplied they can be used without
  * prefix but only one frame sequence is created.
  * <pre>
  *   method=(QString)  : load, scale (default=load)
  *                       load: loads number frames from file
  *                       scale: scales  number frames from one loaded file
  *   number=(int)      : how many frames to generate
  *   file=(Qstring)    : the filename to load (can contain printf format args
  *                       as %d which are replaced, e.g. file=hello_%d.png
  *   mask=(QString)    : Same for the mask of the pixmaps
  *   axis=(int)        : (scale only): scale axis (1=x,2=y,3=x+y)
  *   final=(double)    : final scale in percent (default 0.0, i.e. complete scaling)
  *   colorfilter=1,h,s,v: make a HSV transform of all sprite images
  *   colorfilter=2     :  make it gray (lighter=100 is default)
  *   colorfilter=2,g   : make it gray and lighter (positiv) or darker (negativ)
  * </pre>
  **/
  virtual QCanvasPixmapArray *createPixmapArray(KConfig *config,QString name);

  /**
  * Reads the animations from the config file and calls the corresponding
  * KSprite function to create them.
  * <pre>
  *  anim0=a,b,c,d
  *  anim1=e,f,g,h
  * </pre>
  *  Where the animations have to to be in sequence starting with 0 (i.e.
  *  anim0). <em>a</em> is the first frame of the animation. <em>b</em> is
  * the last frame of the animation. <em>c</em> is the mode of the animations,
  * see @ref KSprite::createAnimation and <em>d</em> is the delay in cycles
  * of the qcanvas animnation.
  *
  * @param config - the config file the sprite is read from
  * @param sprite - the sprite whose animations are set
  **/
  void createAnimations(KConfig *config,KSprite *sprite);

  /**
  * Same as above but to copy the animations from an existing sprite
  **/
  void createAnimations(KSprite *original,KSprite *sprite);

  /**
  * Change a pixmap to grey values. If the second argument is bigger
  * than 100 the pixmap is made lighter and if it less then 100 it is
  * made darker too
  **/
  virtual void changeGrey(QPixmap *pixmap,int lighter=100);

  /**
  * Change the HAS value of the pixmap by dH, dS and dV
  **/
  virtual void changeHSV(QPixmap *pixmap,int dh,int ds,int dv);

  /**
  * Apply the filters as defined in the config file to the sprite name
  * (TODO is this argument needed) to the pixmap.
  */
  virtual void applyFilter(QPixmap *pixmap,KConfig *config,QString name);

  /**
  * resets the cache (?)
  */
  void reset();


  protected:
    QDict<QCanvasItem> mItemDict;                // Spritename lookup
    QDict<QCanvasItem> mCloneDict;               // clone Items lookup

    QString mGrafixDir;
    QString mRcFile;
    KConfig *mConfig;
    QCanvas *mCanvas;
 
};


#endif 
