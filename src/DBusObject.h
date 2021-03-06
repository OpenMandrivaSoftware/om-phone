#pragma once

#include <QString>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>
#include <qdbusmetatype.h>

typedef QMap<QString,QVariantMap> InterfaceList;
typedef QMap<QDBusObjectPath, InterfaceList> ManagedObjects;
typedef QList<QVariantMap> QVariantMapList;

class DBusObject {
protected:
	DBusObject(QString const &service, QDBusObjectPath const &path, QString const &interface);
	template <class R> R property(char const * const p, QString const &subInterface=QString()) const {
		QString interface=_interface;
		if(!subInterface.isEmpty())
			interface += QStringLiteral(".") + subInterface;
		QDBusInterface i(_service, _path.path(), interface, QDBusConnection::systemBus());
		return qvariant_cast<R>(i.property(p));
	}
	ManagedObjects managedObjects() const;
	bool operator == (DBusObject const &other) const { return _path.path() == other._path.path(); }
protected:
	QString const		_service;
	QDBusObjectPath const	_path;
	QString const		_interface;
};
