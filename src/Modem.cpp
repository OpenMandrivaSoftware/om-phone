#include "Modem.h"
#include "Call.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <QTimer>

#include <QDateTime>

#include <iostream>
#include <cassert>

extern "C" {
#include <ModemManager.h>
}

Modem::Modem(QDBusObjectPath const &obj):QObject(0),DBusObject(QString("org.freedesktop.ModemManager1"), obj, "org.freedesktop.ModemManager1.Modem") {
	QDBusConnection::systemBus().connect(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Voice"), QStringLiteral("CallAdded"), this, SLOT(voiceCallAdded(QDBusObjectPath)));
	QDBusConnection::systemBus().connect(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Messaging"), QStringLiteral("Added"), this, SLOT(messageAdded(QDBusObjectPath, bool)));
}

Modem::Modem(int index):Modem(QDBusObjectPath(QStringLiteral("/org/freedesktop/ModemManager1/Modem/") + QString::number(index))) {
}

QDateTime Modem::networkTime() const {
	QDBusInterface timeInterface(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Time"), QDBusConnection::systemBus());
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

	std::cerr << "Accepting" << std::endl;
	Call *c=Call::get(path);
	if(c) {
		std::cerr << "Call from " << qPrintable(c->number()) << std::endl;
		c->accept();
		QTimer::singleShot(120000, c, &Call::hangup);
	}
}

bool Modem::sendSMS(QString const &recipient, QString const &text) {
	QDBusInterface messagingInterface(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Messaging"), QDBusConnection::systemBus());
	QVariantMap messageProperties;
	messageProperties.insert(QStringLiteral("number"), recipient);
	messageProperties.insert(QStringLiteral("text"), text);
	QDBusReply<QDBusObjectPath> msg = messagingInterface.call(QStringLiteral("Create"), messageProperties);
	QString const path = msg.value().path();
	if(path.isEmpty())
		return false;

	QDBusInterface messageInterface(_service, path, QStringLiteral("org.freedesktop.ModemManager1.Sms"), QDBusConnection::systemBus());
	messageInterface.call(QStringLiteral("Send"));
	// FIXME check message status
	return true;
}

SIM Modem::SIM() const {
	return ::SIM(DBusObject::property<QDBusObjectPath>("Sim"));
}

Call *Modem::call(QString const &number) const {
	QDBusInterface callInterface(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Voice"), QDBusConnection::systemBus());
	QVariantMap callProperties;
	callProperties.insert(QStringLiteral("number"), number);
	std::cerr << "Creating call to " << qPrintable(number) << std::endl;
	QDBusReply<QDBusObjectPath> msg = callInterface.call(QStringLiteral("CreateCall"), callProperties);
	if(msg.isValid())
		std::cerr << "valid" << std::endl;
	std::cerr << "Interface: " << qPrintable(msg.value().path()) << std::endl;
	std::cerr << "Error: " << qPrintable(msg.error().message()) << std::endl;
	std::cerr << "Error name: " << qPrintable(msg.error().name()) << std::endl;
	QDBusObjectPath o = msg.value();
	QString const path = o.path();
	if(path.isEmpty())
		return nullptr;
	std::cerr << "Call::get(" << qPrintable(path) << ")" << std::endl;
	return Call::get(o);
}
