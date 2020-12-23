#include "Modem.h"
#include "Call.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <qdbusmetatype.h>
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
		qDebug("Message received: %s", qPrintable(path.path()));
	else
		qDebug("Message added: %s", qPrintable(path.path()));
}

void Modem::voiceCallAdded(QDBusObjectPath path) {
	qDebug("Voice call added: %s", qPrintable(path.path()));
	Call *c=Call::get(path);
	if(c->direction() == Call::Direction::Incoming) {
		qDebug("Incoming call");
		emit incomingCall(c);
	} else if(c->direction() == Call::Direction::Outgoing) {
		qDebug("Outgoing call");
	} else
		qDebug("Wrong direction on call");
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
	qDebug("Creating call to %s", qPrintable(number));
	QDBusReply<QDBusObjectPath> msg = callInterface.call(QStringLiteral("CreateCall"), callProperties);
	if(msg.isValid())
		qDebug("call is valid");
	else {
		qCritical("call is NOT valid:");
		qCritical("Interface: %s", qPrintable(msg.value().path()));
		qCritical("Error: %s", qPrintable(msg.error().message()));
		qCritical("Error name: %s", qPrintable(msg.error().name()));
	}
	QDBusObjectPath o = msg.value();
	QString const path = o.path();
	if(path.isEmpty())
		return nullptr;
	return Call::get(o);
}

QList<Operator> const &Modem::scanNetworks() {
	_operators.clear();
	QDBusInterface scanInterface(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Modem3gpp"), QDBusConnection::systemBus());
	qInfo("Scanning networks...");
	scanInterface.setTimeout(600000);
	QDBusReply<QVariantMapList> msg = scanInterface.call(QStringLiteral("Scan"));
	if(msg.isValid()) {
		QVariantMapList operators = msg.value();
		for(auto const &v: operators) {
			qInfo("Found operator:");
			for(QVariantMap::ConstIterator vit=v.constBegin(); vit!=v.constEnd(); vit++) {
				qInfo("%s=(%s) %s", qPrintable(vit.key()), vit.value().typeName(), qPrintable(vit.value().toString()));
			}
			_operators << v;
		}
		for(auto const &o: _operators) {
			qInfo("Identified operator %s (%s), technology %x, status %u", qPrintable(o.operatorName()), qPrintable(o.operatorCode()), o.accessTechnology(), o.status());
		}
	} else
		qCritical("Failed to scan networks: %s (%s)", qPrintable(msg.error().message()), qPrintable(msg.error().name()));

	return _operators;
}

void Modem::selectNetwork(Operator const &newOperator) {
	selectNetwork(newOperator.operatorCode());
}

void Modem::selectNetwork(QString const &id) {
	QDBusInterface networkInterface(_service, _path.path(), QStringLiteral("org.freedesktop.ModemManager1.Modem.Modem3gpp"), QDBusConnection::systemBus());
	qInfo("Joining network %s", qPrintable(id));
	networkInterface.call(QStringLiteral("Register"), id);
}

bool Modem::ping() const {
	return !DBusObject::property<QString>("Model").isEmpty();
}
