#pragma once

#include <QCoreApplication>
#include <QDBusObjectPath>
#include <QString>

#include "DBusObject.h"
#include "SIM.h"
#include "Call.h"
#include "Operator.h"

class Modem:public QObject,public DBusObject {
	Q_OBJECT
public:
	Modem(QDBusObjectPath const &obj);
	Modem(int index);

	QDateTime networkTime() const;
	QString IMEI() const { return DBusObject::property<QString>("Imei", QStringLiteral("Modem3gpp")); }
	QString manufacturer() const { return DBusObject::property<QString>("Manufacturer"); }
	QString model() const { return DBusObject::property<QString>("Model"); }

	SIM SIM() const;
	Call *call(QString const &number) const;

	QList<Operator> const &scanNetworks();
	void selectNetwork(Operator const &newOperator);
	void selectNetwork(QString const &id);

	bool ping() const;

Q_SIGNALS:
	void incomingCall(Call *call);

public Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void voiceCallAdded(QDBusObjectPath path);

	bool sendSMS(QString const &recipient, QString const &text);

protected:
	QList<Operator>	_operators;
};
