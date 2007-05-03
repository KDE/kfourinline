/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000
    copyright            : (C) 1995-2007 by Martin Heni
    email                : kde@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KWIN4_H
#define KWIN4_H

// Qt includes
#include <QGraphicsScene>

// KDE includes
#include <kxmlguiwindow.h>
#include <kdemacros.h>

// local includes
#include "kwin4global.h"
#include "thememanager.h"


class Kwin4Player;
class KWin4Doc;
class KWin4View;
class KButtonGroup;
class KPlayer;
class KGameChat;
class ChatDlg;
class KGame;

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
    void initDocument();

    /** Save the properties of the application.
      * @param cfg Save to this config group
      */
    virtual void saveProperties(KConfigGroup& cfg);

    /** Read the properties of the application.
      * @param cfg Load from this config group
      */
    virtual void readProperties(const KConfigGroup& cfg);

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
      * @param name The theme name
      */
    void changeTheme(const QString& name);

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

  private:
    // The theme manager used
    ThemeManager* mTheme;

    // The game view
    KWin4View *mView;

    // The game document/engine
    KWin4Doc *mDoc;

    // The graphics scene to use
    QGraphicsScene* mScene;

    // The directory for the theme files
    QString mThemeDirName;

    // Buttons of the network dialog
    KButtonGroup *mColorGroup;

    // The chat dialog
    ChatDlg *mMyChatDlg;

};

#endif // KWIN4_H

