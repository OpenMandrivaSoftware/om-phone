#include "DBusObject.h"

#include <QDBusReply>
#include <qdbusmetatype.h>

Q_DECLARE_METATYPE(InterfaceList);
Q_DECLARE_METATYPE(ManagedObjects);
Q_DECLARE_METATYPE(QVariantMapList);

static bool initialized = false;

static void init() {
	if(!initialized) {
		qDBusRegisterMetaType<InterfaceList>();
		qDBusRegisterMetaType<ManagedObjects>();
		qDBusRegisterMetaType<QVariantMapList>();
		initialized = true;
	}
}

DBusObject::DBusObject(QString const &service, QDBusObjectPath const &path, QString const &interface):_service(service),_path(path),_interface(interface) {
	init();
}

ManagedObjects DBusObject::managedObjects() const {
	QDBusInterface objectManager(_service, _path.path(), QStringLiteral("org.freedesktop.DBus.ObjectManager"), QDBusConnection::systemBus());
	QDBusReply<ManagedObjects> objects = objectManager.call(QStringLiteral("GetManagedObjects"));
	return objects.value();
}
