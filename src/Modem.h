#pragma once

#include <QCoreApplication>
#include <QDBusObjectPath>
#include <QString>

#include "DBusObject.h"

class Modem:public QObject,public DBusObject {
	Q_OBJECT
public:
	Modem(QDBusObjectPath const &obj);
	Modem(int index);

	QDateTime networkTime() const;
	QString IMEI() const { return DBusObject::property<QString>("Imei", QStringLiteral("Modem3gpp")); }
	QString manufacturer() const { return DBusObject::property<QString>("Manufacturer"); }
	QString model() const { return DBusObject::property<QString>("Model"); }

	QDBusObjectPath SIM() const;

public Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void voiceCallAdded(QDBusObjectPath path);

	bool sendSMS(QString const &recipient, QString const &text);

protected:
	QDBusObjectPath	_dbusPath;
};
