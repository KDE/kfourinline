#ifndef DISPLAY_GAME_H
#define DISPLAY_GAME_H
/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Qt includes
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHash>
#include <QList>
#include <QTimer>

// Local includes
#include "thememanager.h"

// Forward declaration
class ThemeManager;
class PieceSprite;
class SpriteNotify;
class PixmapSprite;
class ScoreSprite;

/**
 * The view object which shows the graphics in a
 * canvas view.
 */
class DisplayGame : public QObject, public virtual Themable
{
  Q_OBJECT

  public:
    /** Constructor for the game display.
     *  @param advancePeriod The canvas advance period
     *  @param scene The graphics scene
     *  @param parent The parent window
     */
    DisplayGame(int advancePeriod, QGraphicsScene* scene, ThemeManager* theme,  QGraphicsView* parent = 0);
    ~DisplayGame();

    void start();

    virtual void changeTheme();

    SpriteNotify* setPiece(int x,int y,int color,int no,bool animation);
    void setArrow(int x, int color);
    void drawStar(int x,int y,int no);
    int mapMouseToMove(QPoint pos);

    ScoreSprite* score();

 protected:

  public slots:  

  protected slots:  
    void run();

  signals:

  protected:

  private:
    ThemeManager* mTheme;
    QGraphicsScene* mScene;
    QGraphicsView* mView;
    int mAdvancePeriod;
    QList<QGraphicsItem*> mSprites;
    QList<PieceSprite*> mPieces;
    QTimer* mTimer;
    PixmapSprite* mBoard;
    QList<PixmapSprite*> mBoardHoles;
    QList<PixmapSprite*> mArrows;
    ScoreSprite*  mScoreBoard;
    QList<PixmapSprite*> mStars;
    
};

#endif
