/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Martin Heni (kde at heni-online.de)
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef __KGAMECONNECTDIALOG_H__
#define __KGAMECONNECTDIALOG_H__

// Qt
#include <QWidget>

class KGameConnectWidgetPrivate;
class QAbstractButton;
/**
 * \class KGameConnectWidget kgameconnectdialog.h <KGameConnectWidget>
 */
class KGameConnectWidget : public QWidget
{
	Q_OBJECT
public:
	explicit KGameConnectWidget(QWidget* parent);
	virtual ~KGameConnectWidget();

	/**
	 * @param host The host to connect to by default
	 **/
	void setHost(const QString& host);

	/**
	 * @return The host to connect to or QString() if the user wants to
	 * be the MASTER
	 **/ 
	QString host() const;

	/**
	 * @param port The port that will be shown by default
	 **/
	void setPort(unsigned short int port);

	/**
	 * @return The port to connect to / to listen
	 **/
	unsigned short int port() const;

	/**
	 * Specifies which state is the default (0 = server game; 1 = join game)
	 * @param state The default state. 0 For a server game, 1 to join a game
	 **/
	void setDefault(int state);
	
	/**
	 * Sets DNS-SD service type, both for publishing and browsing
	 * @param type Service type (something like _kwin4._tcp). 
	 * It should be unique for application.
	 **/
	void setType(const QString& type);
	
	/**
	 * @return service type
	 */
	QString type() const;
	
	/** 
	 * Set game name for publishing. 
	 * @param name Game name. Important only for server mode. If not
	 * set hostname will be used. In case of name conflict -2, -3 and so on will be added to name.
	 */
	void setName(const QString& name);
	
	/**
	 * @return game name. 
	 */
	QString gameName() const;

protected Q_SLOTS:
	/**
	 * The type has changed, ie the user switched between creating or
	 * joining.
	 **/
        void slotTypeChanged(QAbstractButton *button);
	void slotGamesFound();
	void slotGameSelected(int);

Q_SIGNALS:
	void signalNetworkSetup();
  void signalServerTypeChanged(int);

private:
	void showDnssdControls();
	KGameConnectWidgetPrivate* d;

};

#endif
