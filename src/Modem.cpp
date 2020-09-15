#include "Modem.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>

#include <QDateTime>

#include <iostream>
#include <cassert>

static QString const mmService(QStringLiteral("org.freedesktop.ModemManager1"));

Modem::Modem(QDBusObjectPath const &obj):_dbusPath(obj) {
	QDBusConnection::systemBus().connect(mmService, _dbusPath.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Voice"), QStringLiteral("CallAdded"), this, SLOT(voiceCallAdded(QDBusObjectPath)));
	QDBusConnection::systemBus().connect(mmService, _dbusPath.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Messaging"), QStringLiteral("Added"), this, SLOT(messageAdded(QDBusObjectPath, bool)));
}

Modem::Modem(int index):Modem(QDBusObjectPath(QStringLiteral("/org/freedesktop/ModemManager1/Modem/") + QString::number(index))) {
}

QDateTime Modem::networkTime() const {
	QDBusInterface timeInterface(mmService, _dbusPath.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Time"), QDBusConnection::systemBus());
	QDBusReply<QString> networkTime = timeInterface.call(QStringLiteral("GetNetworkTime"));
	return QDateTime::fromString(networkTime, Qt::ISODate);
}

void Modem::messageAdded(QDBusObjectPath path, bool received) {
	if(received)
		std::cerr << "Message received: ";
	else
		std::cerr << "Message added: ";

	std::cerr << qPrintable(path.path()) << std::endl;
}

void Modem::voiceCallAdded(QDBusObjectPath path) {
	std::cerr << "Voice call added: " << qPrintable(path.path()) << std::endl;
}

bool Modem::sendSMS(QString const &recipient, QString const &text) {
	QDBusInterface messagingInterface(mmService, _dbusPath.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Messaging"), QDBusConnection::systemBus());
	QVariantMap messageProperties;
	messageProperties.insert(QStringLiteral("number"), recipient);
	messageProperties.insert(QStringLiteral("text"), text);
	QDBusReply<QDBusObjectPath> msg = messagingInterface.call(QStringLiteral("Create"), messageProperties);
	QString const path = msg.value().path();
	if(path.isEmpty())
		return false;

	QDBusInterface messageInterface(mmService, path, QStringLiteral("org.freedesktop.ModemManager1.Sms"), QDBusConnection::systemBus());
	messageInterface.call(QStringLiteral("Send"));
	// FIXME check message status
	return true;
}

QDBusObjectPath Modem::SIM() const {
	QString interface=QStringLiteral("org.freedesktop.ModemManager1.Modem");
	QDBusInterface modemInterface(mmService, _dbusPath.path(), interface, QDBusConnection::systemBus());
	return qvariant_cast<QDBusObjectPath>(modemInterface.property("Sim"));
}

QString Modem::property(char const * const p, QString const &subInterface) const {
	QString interface=QStringLiteral("org.freedesktop.ModemManager1.Modem");
	if(!subInterface.isEmpty())
		interface += QStringLiteral(".") + subInterface;
	QDBusInterface modemInterface(mmService, _dbusPath.path(), interface, QDBusConnection::systemBus());
	return modemInterface.property(p).toString();
}
