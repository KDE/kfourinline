/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)
    SPDX-FileCopyrightText: 2001 Martin Heni (kde at heni-online.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

// NAMING
// please follow these naming rules if you add/change classes:
// the main dialog is named KGameDialog and the base config widget
// KGameDialogConfig. All config widgets are named KGameDialogXYZConfig (where
// XYZ = the name of the config widget, like "general" or "network") and are
// inherited from KGameDialogConfig.

#ifndef __KGAMEDIALOGCONFIG_H__
#define __KGAMEDIALOGCONFIG_H__

// Qt
#include <QWidget>

class KGame;
class KPlayer;

class KGameDialogConfigPrivate;
/**
 * \class KGameDialogConfig kgamedialogconfig.h <KGameDialogConfig>
 * 
 * Base class for configuration widgets.
 *
 * You can inherit from this and implement @ref submitToKGame, @ref
 * setOwner and @ref setKGame to create your personal @ref KGame configuration widget :-)
 * @short Base class for configuration widgets
 * @author Andreas Beckermann <b_mann@gmx.de>
 **/
class KGameDialogConfig : public QWidget
{
	Q_OBJECT
public:
    explicit KGameDialogConfig(QWidget* parent = nullptr);
	virtual ~KGameDialogConfig();

	/**
	 * Called by @ref KGameDialog to submit all settings to the KGame
	 * Object.
	 * You have to replace this if you add your own widgets!
	 * @param g A pointer to your KGame.
	 * @param p A pointer to the player owning this dialog
	 **/
	virtual void submitToKGame(KGame* g, KPlayer* p) = 0;

	/**
	 * The owner player of the dialog has been changed. The default
	 * changes the pointer for owner so don't forget to call the
	 * default implementation if you overwrite this!
	 *
	 * You can use this e.g. to change a line edit widget containing the 
	 * player name.
	 *
	 * Note: even NULL players are allowed!
	 * @param p The new owner player of the dialog
	 **/
	virtual void setOwner(KPlayer* p);

	/**
	 * The KGame object of the dialog has been changed. The default
	 * implementation changes the pointer for game so don't forget to
	 * call the default implementation if you overwrite this!
	 *
	 * You can use this e.g. to re-read the min/max player settings.
	 * @param g The KGame object
	 **/
	virtual void setKGame(KGame* g);

	/**
	 * The admin status has been changed.
	 * If the KGame object of this config widget is the
	 * admin the user is allowed to configure it. Otherwise most
	 * widgets will have to be disabled. Note that you don't necessarily
	 * need to deactivate all widget - e.g. the player name must be
	 * configured by the player. Mainly the KGame configuration can be done
	 * by the admin only.
	 *
	 * By default this does nothing. Changes the value for admin so 
	 * don't forget to call the default implementation in derived classes!
	 * @param admin Whether the KGame object of this dialog can be
	 * configured
	 **/
	virtual void setAdmin(bool admin);

	/**
	 * A pointer to the     KGame object that has been set by @ref setKGame.
	 *
	 * Note that NULL is allowed!
	 * @return The KGame object assigned to this dialog
	 **/
	KGame* game() const;

	/**
	 * A pointer to the KPlayer object that has been set by @ref
	 * setOwner.
	 *
	 * Note that NULL is allowed!
	 * @return The owner of the dialog
	 **/
	KPlayer* owner() const;

	/**
	 * @return True if the owner is ADMIN otherwise FALSE. See also
	 * @ref setAdmin
	 **/
	bool admin() const;

protected:

private:
	KGameDialogConfigPrivate* const d;
};

class KGameDialogNetworkConfigPrivate;
/**
 * \class KGameDialogNetworkConfig kgamedialogconfig.h <KGameDialogConfig>
 */
class KGameDialogNetworkConfig : public KGameDialogConfig
{
	Q_OBJECT
public:
    explicit KGameDialogNetworkConfig(QWidget* parent = nullptr);
	virtual ~KGameDialogNetworkConfig();


	/**
	 * Called by @ref KGameDialog to submit all settings to the KGame
	 * Object.
	 * You have to replace this if you add your own widgets!
	 * @param g A pointer to your KGame.
	 * @param p A pointer to the player owning this dialog
	 **/
	void submitToKGame(KGame* g, KPlayer* p) override;

	void setKGame(KGame* g) override;

	/**
	 * This sets the default port and host used in @ref KGameConnectDialog.
	 * The user will be able to change these defaults!
	 *
	 * If you don't call this then host "localhost" and port "0" is used.
	 * You are strongly encouraged to change at least the port!
	 * @param port The default port to connect to / listen on
	 * @param host The default host to connect to
	 * @param server The default state. 0 For a server game, 1 to join a game
	 **/
	void setDefaultNetworkInfo(const QString& host, unsigned short int port,bool server=true);
	
	/**
	 * Set service type that will be published or browsed for and game name that will be displayed in 
	 * server browser. Without this  publishing and discovery of LAN servers will not be enabled.
	 * @param name Game name. Important only for server mode. If not
	 * set hostname will be used. In case of name conflict -2, -3 and so on will be added to name.
	 * @param type Service type (something like _kwin4._tcp). It should be unique for application.
	 **/
	void setDiscoveryInfo(const QString& type, const QString& name=QString());
	
Q_SIGNALS:
  /**
  * This signal is emitted if the user changes the server type (client/server)
  * in the network configuration dialog. 
  *
  * @param t type (0/1) of the connection
  **/
  void signalServerTypeChanged(int t);


protected:
	void setConnected(bool connected, bool master = false);

protected Q_SLOTS:
	void slotInitConnection();
	void slotExitConnection();
	void slotConnectionBroken();


private:
	KGameDialogNetworkConfigPrivate* d;
};

#endif
