#pragma once

#include <QApplication>
#include <QDBusObjectPath>
#include <QList>

#include "PhoneWidget.h"
#include "Modem.h"
#include "Call.h"

class Phone:public QApplication,public DBusObject {
	Q_OBJECT
public:
	Phone(int &argc, char **&argv);
	QString mmVersion() const;
	QList<QDBusObjectPath> modems() const;
	std::string phoneLocale() const;

private Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void incomingCall(Call* call);
	void scanModems();
	void interfaceRemoved();
	void interfaceAdded();

public Q_SLOTS:
	// D-Bus interface
	Q_SCRIPTABLE bool show(QString url=QString());
	Q_SCRIPTABLE bool hide();
	Q_SCRIPTABLE bool call(QString const &number);

protected:
	QList<Modem*>	_modems;
	PhoneWidget *	_ui;
};
