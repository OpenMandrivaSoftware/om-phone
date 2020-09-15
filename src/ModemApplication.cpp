#include "ModemApplication.h"
#include "SIM.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>

#include <QDateTime>

#include <iostream>
#include <cassert>

static QString const mmService(QStringLiteral("org.freedesktop.ModemManager1"));

ModemApplication::ModemApplication(int &argc, char **&argv):QCoreApplication(argc, argv),DBusObject(mmService, QDBusObjectPath("/org/freedesktop/ModemManager1"), "org.freedesktop.ModemManager1") {
	assert(QDBusConnection::systemBus().interface()->isServiceRegistered(mmService));

	std::cerr << "Using ModemManager " << qPrintable(mmVersion()) << std::endl;
	QList<QDBusObjectPath> mdms = modems();
	std::cerr << "Found " << mdms.count() << " modems: " << std::endl;
	for(QDBusObjectPath const &p : mdms) {
		Modem *m = new Modem(p);
		_modems << m;
		std::cerr << "	" << qPrintable(m->manufacturer()) << " " << qPrintable(m->model()) << std::endl;
		std::cerr << "		IMEI: " << qPrintable(m->IMEI()) << std::endl;
		std::cerr << "		Network time: " << qPrintable(m->networkTime().toString()) << std::endl;
		SIM s = m->SIM();
		std::cerr << "		SIM identifier: " << qPrintable(s.identifier()) << std::endl;
		std::cerr << "		SIM IMSI: " << qPrintable(s.IMSI()) << std::endl;
		std::cerr << "		SIM emergency numbers: " << qPrintable(s.emergencyNumbers().join(QStringLiteral(", "))) << std::endl;
		std::cerr << "		SIM operator: " << qPrintable(s.operatorName()) << " (" << qPrintable(s.operatorID()) << ")" << std::endl;
	}
}

QString ModemApplication::mmVersion() const {
	return DBusObject::property<QString>("Version");
}

QList<QDBusObjectPath> ModemApplication::modems() const {
	QList<QDBusObjectPath> ret;
	ManagedObjects obj=managedObjects();
	for(auto const &k : obj.keys()) {
		if(k.path().contains("/Modem/"))
			ret << k;
	}
	return ret;
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
