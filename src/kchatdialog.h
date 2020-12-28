/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef __KCHATDIALOG_H__
#define __KCHATDIALOG_H__

// Qt
#include <QDialog>

class QDialogButtonBox;
class QVBoxLayout;

class KChatBase;
class KChatDialogPrivate;

/**
 * \class KChatDialog kchatdialog.h <KChatDialog>
 * */
class KChatDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	 * Construct a KChatDialog widget
	 **/
	explicit KChatDialog(QWidget* parent, bool modal = false);

	/**
	 * Construct a KChatDialog widget which automatically configures the
	 * @ref KChatBase widget. You probably want to use this as you don't
	 * have to care about the configuration stuff yourself.
	 **/
	KChatDialog(KChatBase* chatWidget, QWidget* parent, bool modal = false);

	/**
	 * Destruct the dialog
	 **/
	~KChatDialog();

	/**
	 * @return The font that shall be used as the "name: " part of a normal
	 * message.
	 **/
	QFont nameFont() const;

	/**
	 * @return The font that shall be used for normal messages.
	 **/
	QFont textFont() const;
	
	/**
	 * @return The font that shall be used as the "name: " part of a system
	 * (game) message.
	 **/
	QFont systemNameFont() const;
	
	/**
	 * @return The font that shall be used for a system (game) message.
	 **/
	QFont systemTextFont() const;

	/**
	 * Set the widget that will be configured by the dialog. Use this if you
	 * don't want to configure the widget yourself.
	 * @param widget The chat widget that shall be configured
	 * @param applyFonts Whether you want to have the current @ref KChatBase fonts as
	 * defaults in the dialog
	 **/
	void plugChatWidget(KChatBase* widget, bool applyFonts = true);

	/**
	 * Used to configure the chat widget according to the user settings.
	 * This is called automatically if @ref plugChatWidget was called
	 * before.
	 * @param widget The chat widget that shall be configured
	 **/
	void configureChatWidget(KChatBase* widget);

	/**
	 * @return The maximal allowed messages in the chat widget. -1 is
	 * unlimited
	 **/
	int maxMessages() const;

protected Q_SLOTS:
	void slotGetNameFont();
	void slotGetTextFont();
	void slotGetSystemNameFont();
	void slotGetSystemTextFont();

	virtual void slotApply();
	virtual void slotOk();

private:
	void setNameFont(const QFont&);
	void setTextFont(const QFont&);
	void setSystemNameFont(const QFont&);
	void setSystemTextFont(const QFont&);
	void setMaxMessages(int max);

private:
	void init();

private:
	KChatDialogPrivate* const d;
	QVBoxLayout* mainLayout;
	QDialogButtonBox* buttonBox;
};

#endif
