#include "ModemApplication.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>

#include <QDateTime>

#include <iostream>
#include <cassert>

static QString const mmService(QStringLiteral("org.freedesktop.ModemManager1"));

ModemApplication::ModemApplication(int &argc, char **&argv):QCoreApplication(argc, argv),_m0(0) {
	assert(QDBusConnection::systemBus().interface()->isServiceRegistered(mmService));
	std::cerr << "Using ModemManager " << qPrintable(mmVersion()) << " at " << qPrintable(_m0.networkTime().toString()) << std::endl;
}

QString ModemApplication::mmVersion() const {
	QDBusInterface mm(mmService, QStringLiteral("/org/freedesktop/ModemManager1"), QStringLiteral("org.freedesktop.ModemManager1"), QDBusConnection::systemBus());
	return mm.property("Version").toString();
}

void ModemApplication::messageAdded(QDBusObjectPath path, bool received) {
	if(received)
		std::cerr << "Message received: ";
	else
		std::cerr << "Message added: ";

	std::cerr << qPrintable(path.path()) << std::endl;
}

void ModemApplication::voiceCallAdded(QDBusObjectPath path) {
	std::cerr << "Voice call added: " << qPrintable(path.path()) << std::endl;
}
