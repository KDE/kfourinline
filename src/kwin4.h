/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2000 Martin Heni <kde@heni-online.de>

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

#ifndef KWIN4_H
#define KWIN4_H

// KDE includes
#include <kxmlguiwindow.h>
#include <kdemacros.h>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgameio.h>

// local includes
#include "kwin4global.h"
#include "thememanager.h"

class KWin4Doc;
class KWin4View;
class KButtonGroup;
class KPlayer;
class KGame;
class ChatDlg;
class ReflectionGraphicsScene;

/**
 * The base class for Kwin4 application.
 */
class KWin4App : public KXmlGuiWindow
{

  Q_OBJECT

  public:
    /** Construct the application.
      * @param parent The parent
      */
    KWin4App(QWidget *parent=0);

    /** Destruct the application.
      */
    ~KWin4App();

  protected:
    /** The game is ended. Either because it is finised or because it is
      * aborted by the player.
      * @param mode Result of the game
      */
    void EndGame(TABLE mode);

    /** Flags which menus should be checked and set by below functions.
      */
    enum CheckFlags {All=0,CheckFileMenu=1,CheckEditMenu=2};

    /** Enable or disable an action depending on the boolean parameter.
      * @param action The action to affect
      * @param enable True if the action should be enabled, false otherwise.
      */
    void changeAction(const char* action,bool enable);

    /** Enable oactions that is menu/toolbar items.
      * @param action The action to affect
      */
    void enableAction(const char *action) {changeAction(action, true); }

    /** Disable actions that is menu/toolbar items.
      * @param action The action to affect
      */
    void disableAction(const char *action) {changeAction(action, false); }

    /** Checks all menus and depening on the game status enables or disables them.
      * This is usually done on  program init.
      * @param menu Which menu to check, default all menus
      */
    void checkMenus(CheckFlags menu=All);

    /** Setup all actions (menus and toolbar items).
      */
    void initGUI();

    /** Setup the statusbar.
      */
    void initStatusBar();

    /** Prepare the game engine (document) and connect its signals.
      */
    void connectDocument();

    /** Save the properties of the application.
      */
    void saveProperties();

    /** Save instance-specific properties. The function is
     * invoked when the session manager requests your application
     * to save its state.
     * @param grp The config group
     */
    virtual void saveProperties(KConfigGroup& grp);

    /** Read the properties of the application.
      */
    void readProperties();

   /** Read instance-specific properties.
     * @param grp The config group
    */
    virtual void readProperties(const KConfigGroup& grp);

    /** Called by KMainWindow when the last window of the application is
     * going to be closed.
     * @see KMainWindow#queryExit
     * @see KMainWindow#closeEvent
     */
    virtual bool queryExit();

    /** Retrieve the theme file from the theme index number give.
      * @param idx The theme index number [0..]
      * @return The theme file name.
      */
    QString themefileFromIdx(int idx);

    /** Retrieve the theme idx number from the theme name give.
      * @param name The theme file name.
      * @return The theme index number [0..]
      */
    int themeIdxFromName(QString name);

  public slots:
    /** The server type was changed (KGame)
      * @param t The server type
      */
    void serverTypeChanged(int t);

    /** The remote player changed (GUI signal)
      * @param who  The remote player is 0:y ellow, 1: red
      */
    void remoteChanged(int who);

    /** The game is over indication.
      * @param status  -1: remis, 1: won/loss 2: abort
      * @param p       The player who made the last move
      * @param me      The game
      */
    void slotGameOver(int status, KPlayer* p, KGame* me);

    /** A move has been performed. Update the status etc.
      * @param playerNumber Number of the next player to move
      */
    void moveDone(int playerNumber);

    /** The network connection has been lost.
      * @param id        Game id
      * @param oldstatus Previous game status
      * @param game      Our game
      */
    void networkBroken(int id, int oldstatus ,KGame* game);

    /** Being noticed by KGAme that a new game started
      */
    void gameRun();

    /** Update the names in the status bar.
      */
    void updateStatusNames();

    /** Show the network configuration dialog.
      */
    void configureNetwork();

    /** Show the network chat dialog.
      */
    void configureChat();

    /** Show the debug a KGame dialog.
      */
    void debugKGame();

    /** The New game menu is selected.
      */
    void menuNewGame();

    /** The Load game menu is selected.
      */
    void menuOpenGame();

    /** The Save game menu is selected.
      */
    void menuSaveGame();

    /** Called when the game is to be ended, e.g. via the abort game menu.
      */
    void endGame();

    /** Shows the statistics dialog.
      */
    void showStatistics();

    /** A game hint is asked via the menu. The game will give a
      * movement tip.
      */
    void askForHint();

    /** A new theme is selected from the menu.
      * @param idx The theme index
      */
    void changeTheme(int idx);

    /** A move undo request is given via the menu.
      */
    void undoMove();

    /** A move redo request is given via the menu.
      */
    void redoMove();

    /** Display the current player to move in the statusbar.
      * @param text The text to display
      */
    void displayStatusbarMover(const QString& text);

    /** Display a message in the status bar.
      * @param text The message to display
      */
    void displayStatusMessage(const QString& text);

    /** Show the settings dialog.
      */
    void configureSettings();

    /** SLot if a new game is started from the intro display.
      * @param startPlayer Color of the starting player
      * @param input0      Input device of player 1
      * @param input1      Input device of player 2
      * @param aiLevel     Level for AI (-1: no change)
      */
    void quickStart(COLOUR startPlayer, KGameIO::IOMode input0, KGameIO::IOMode input1, int level);

  private:
    // The theme manager used
    ThemeManager* mTheme;

    // The available themes
    QHash<QString,QString> mThemeFiles;

    // Current theme index
    int mThemeIndexNo;

    // Default theme 
    QString mThemeDefault;

    // The game view
    KWin4View *mView;

    // The game document/engine
    KWin4Doc *mDoc;

    // The graphics scene to use
    ReflectionGraphicsScene* mScene;

    // Buttons of the network dialog
    KButtonGroup *mColorGroup;

    // The chat dialog
    ChatDlg *mMyChatDlg;

};

#endif // KWIN4_H

