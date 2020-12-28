/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)
    SPDX-FileCopyrightText: 2001 Martin Heni (kde at heni-online.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef __KGAMEDEBUGDIALOG_H__
#define __KGAMEDEBUGDIALOG_H__

// KF
#include <KPageDialog>

class QListWidgetItem;

class KGame;
class KPlayer;

class KGameDebugDialogPrivate;

/**
 * \class KGameDebugDialog kgamedebugdialog.h <KGameDebugDialog>
 */
class KGameDebugDialog : public KPageDialog
{
	Q_OBJECT
public:
	KGameDebugDialog(KGame* g, QWidget* parent, bool modal = false);
	~KGameDebugDialog();

	/**
	 * Automatically connects the KGame object to all error dependent slots. 
	 * @param g The KGame which will emit the errors (or not ;-) )
	 **/
	void setKGame(const KGame* g);

public Q_SLOTS:
	/**
	 * Unsets a @ref KGame which has been set using @ref setKGame before.
	 * This is called automatically when the @ref KGame object is destroyed
	 * and you normally don't have to call this yourself.
	 *
	 * Note that @ref setKGame also unsets an already existing @ref KGame
	 * object if existing.
	 **/
	void slotUnsetKGame();

	/**
	 * Update the data of the @ref KGame object
	 **/
	void slotUpdateGameData();

	/**
	 * Update the properties of the currently selected player
	 **/
	void slotUpdatePlayerData();

	/**
	 * Updates the list of players and calls @ref clearPlayerData. Note that
	 * after this call NO player is selected anymore.
	 **/
	void slotUpdatePlayerList();

	void slotClearMessages();

Q_SIGNALS:
	/**
	 * This signal is emitted when the "debug messages" page couldn't find
	 * the name of a message id. This is usually the case for user-defined
	 * messages. KGameDebugDialog asks you to give the msgid a name.
	 * @param messageid The ID of the message. As given to @ref
	 * KGame::sendMessage
	 * @param userid User defined msgIds are internally increased by
	 * KGameMessage::IdUser. You don't have to care about this but if
	 * this signal is emitted with userid=false (shouldn't happen) then the
	 * name of an internal message as defined in 
	 * KGameMessage::GameMessageIds couldn't be found.
	 * @param name The name of the msgid. You have to fill this!
	 **/
	void signalRequestIdName(int messageid, bool userid, QString& name);

protected:
	void clearPages();

	/**
	 * Clear the data of the player view. Note that the player list is NOT
	 * cleared.
	 **/
	void clearPlayerData();

	/**
	 * Clear the data view of the @ref KGame object
	 **/
	void clearGameData();

	/**
	 * Add a new player to the player list
	 **/
	void addPlayer(KPlayer* p);

	/**
	 * Remove a player from the list
	 **/
	void removePlayer(QListWidgetItem* item);

	/**
	 * @return Whether messages with this msgid shall be displayed or not
	 **/
	bool showId(int msgid);

protected Q_SLOTS:
	/**
	 * Update the data of the player specified in item
	 * @param item The @ref QListWidgetItem of the player to be updated. Note
	 * that the text of this item MUST be the ID of the player
	 **/
	void slotUpdatePlayerData(QListWidgetItem* item);

	void slotShowId();
	void slotHideId();

	/**
	 * A message has been received - see @ref KGame::signalMessageUpdate
	 **/
	void slotMessageUpdate(int msgid, quint32 receiver, quint32 sender);

private:
	void initGamePage();
	void initPlayerPage();
	void initMessagePage();

private:
	KGameDebugDialogPrivate* const d;
};


#endif
