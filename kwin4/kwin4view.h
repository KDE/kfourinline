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

#ifndef KWIN4VIEW_H
#define KWIN4VIEW_H

#include <qcanvas.h>
#include <kgameio.h>

class Kwin4Doc;
class ScoreWidget;
class StatusWidget;
class KSpriteCache;

/**
 * The Kwin4View class provides the view widget for the Kwin4App instance.
 */
class Kwin4View : public QCanvasView
{
Q_OBJECT

public:
  Kwin4View(Kwin4Doc *theDoc, QWidget *parent = 0, const char *name=0);

  void initView(bool deleteall=true);
  void drawBoard(bool remove=false);
  void drawIntro(bool remove=false);
  void hideIntro();
  void drawStar(int x,int y,int no);
  void setArrow(int x,int color);
  void setPiece(int x,int y,int color,int no,bool animation=true);
  void setHint(int x,int y,bool enabled);
  ScoreWidget *scoreWidget() {return mScoreWidget;}
  StatusWidget *statusWidget() {return mStatusWidget;}
  void setSprite(int no,int x, int col, bool enable);
  void clearError();
  void EndGame();

public slots:
  void slotMouseInput(KGameIO *input,QDataStream &stream,QMouseEvent *e,bool *eatevent);
  void slotKeyInput(KGameIO *input,QDataStream &stream,QKeyEvent *e,bool *eatevent);
  void introMoveDone(QCanvasItem *item,int mode);
 
protected:
  QPixmap *loadPixmap(QString name);
  void resizeEvent(QResizeEvent *e);
  bool wrongPlayer(KPlayer *player,KGameIO::IOMode io);

private:
  Kwin4Doc *doc;
  QCanvas *mCanvas;    // our drawing canvas
  KSpriteCache *mCache; // The sprite cache
  QString mGrafix;      // grafix dir

  int mLastArrow;       // last drawn arrow
  int mLastX;           // last piece
  int mLastY;
  int mSpreadX;         // spread x,y board pieces
  int mSpreadY;
  int mBoardX;          // board offset
  int mBoardY;

  ScoreWidget *mScoreWidget; // score widget
  StatusWidget *mStatusWidget; // score widget
};

#endif // KWIN4VIEW_H

