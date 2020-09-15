#include "ModemApplication.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <qdbusmetatype.h>

#include <QDateTime>

#include <iostream>
#include <cassert>

static QString const mmService(QStringLiteral("org.freedesktop.ModemManager1"));
typedef QMap<QString,QVariantMap> InterfaceList;
typedef QMap<QDBusObjectPath, InterfaceList> ManagedObjectList;
Q_DECLARE_METATYPE(InterfaceList);
Q_DECLARE_METATYPE(ManagedObjectList);

ModemApplication::ModemApplication(int &argc, char **&argv):QCoreApplication(argc, argv) {
	assert(QDBusConnection::systemBus().interface()->isServiceRegistered(mmService));
	qDBusRegisterMetaType<InterfaceList>();
	qDBusRegisterMetaType<ManagedObjectList>();

	std::cerr << "Using ModemManager " << qPrintable(mmVersion()) << std::endl;
	QList<QDBusObjectPath> mdms = modems();
	std::cerr << "Found " << mdms.count() << " modems: " << std::endl;
	for(QDBusObjectPath const &p : mdms) {
		Modem *m = new Modem(p);
		_modems << m;
		std::cerr << "	" << qPrintable(m->manufacturer()) << " " << qPrintable(m->model()) << std::endl;
		std::cerr << "		IMEI: " << qPrintable(m->IMEI()) << std::endl;
		std::cerr << "		Network time: " << qPrintable(m->networkTime().toString()) << std::endl;
		std::cerr << "		SIM: " << qPrintable(m->SIM().path()) << std::endl;
	}
}

QString ModemApplication::mmVersion() const {
	QDBusInterface mm(mmService, QStringLiteral("/org/freedesktop/ModemManager1"), QStringLiteral("org.freedesktop.ModemManager1"), QDBusConnection::systemBus());
	return mm.property("Version").toString();
}

QList<QDBusObjectPath> ModemApplication::modems() const {
	QList<QDBusObjectPath> ret;
	QDBusInterface om(mmService, QStringLiteral("/org/freedesktop/ModemManager1"), QStringLiteral("org.freedesktop.DBus.ObjectManager"), QDBusConnection::systemBus());

	QDBusReply<ManagedObjectList> objects = om.call(QStringLiteral("GetManagedObjects"));
	ManagedObjectList obj=objects.value();
	for(ManagedObjectList::Iterator it=obj.begin(); it!=obj.end(); it++) {
		if(it.key().path().contains("/Modem/"))
			ret << it.key();
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
