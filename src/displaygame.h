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
class ReflectionGraphicsScene;

/**
 * The display engine for the actual game.
 */
class DisplayGame : public QObject, public virtual Themeable
{
  Q_OBJECT

  public:
    /** Constructor for the game display.
     *  @param scene         The graphics scene
     *  @param theme         The theme manager
     *  @param parent        The parent window
     */
    DisplayGame(ReflectionGraphicsScene* scene, ThemeManager* theme,  QGraphicsView* parent = 0);
    
    /** Destructor
      */
    ~DisplayGame();

    /** Start the display (setup and initialzation)
      */
    void start();

    /** Main theme function. Called by the theme manager. Redraw and resize 
      * display.
      */
    virtual void changeTheme();

    /** Set a game piece to a certain position on the game board. If an animation
      * is requested the piece 'falls' to this position.
      * @param x         The x-position on the game board [0-6]
      * @param y         The y-position on the game board [0-5]
      * @param color     Which color to use for the piece [0:hide piece, 1: yellow, 2: red]
      * @param no        Which sprite to use for the piece [0-41]
      * @param animation True to use animation, false to just plot the piece
      * @return The sprite notification object. Its signal indicate the end of the animation.
      */
    SpriteNotify* displayPiece(int x,int y,int color,int no,bool animation);
    
    /** Sets the movement indicator arrow to the given position.
      * @param x      The x position [0-6]
      * @param color  The arrow color [0:hide piece, 1: yellow, 2: red]
      */
    void displayArrow(int x, int color);
    
    /** Sets the hint indicator on the game board. This is a little symbol to suggest
      * where the next move is recommended to go.
      * @param x         The x-position on the game board [0-6]
      * @param y         The y-position on the game board [0-5]
      * @param show      True to show the indicator, false to hide it.
      */
    void displayHint(int x, int y, bool show);
    
    /** Sets on of the winning move indicator sprites on the game board.
      * They should be set on top of the winning pieces to show them.
      * @param x         The x-position on the game board [0-6]
      * @param y         The y-position on the game board [0-5]
      * @param no        Which sprite to use for the piece [0-3]
      */
    void displayStar(int x,int y,int no);
    
    /** Transform the mouse coordinates to a game board movement coordinate.
      * That is a coordinate [0-6] corresponding to a move on the board.
      * @param pos  The mouse position.
      * @return     The game board position [0-6] or [-1] on error.
      */
    int mapMouseToMove(const QPoint &pos);
    
    /** Shows the game over sprite.
      */
    void displayEnd();

    /** Retrieve the score status sprite.
      * @return The score sprite.
      */
    ScoreSprite* score();

   protected slots:  
    /** Run the main game animation (if existent). This is done via a periodic timer.
      */
    void advance();

  
  private:
    // The theme manager
    ThemeManager* mTheme;
    
    // The grapics scene used for display
    ReflectionGraphicsScene* mScene;
    
    // The graphics view used for display
    QGraphicsView* mView;
    
    // List of all sprites used
    QList<QGraphicsItem*> mSprites;
    
    // List of all game pieces used #=42
    QList<PieceSprite*> mPieces;

    // The game boad sprite
    PixmapSprite* mBoard;
    
    // All arrow sprites
    QList<PixmapSprite*> mArrows;
    
    // The score sprite
    ScoreSprite*  mScoreBoard;
    
    // The winning indicator sprites
    QList<PixmapSprite*> mStars;
    
    // The hint sprite
    PixmapSprite* mHint;
    
    // The time used for advance()
    QTimer* mTimer;
};

#endif
