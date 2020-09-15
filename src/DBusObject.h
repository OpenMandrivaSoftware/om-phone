#pragma once

#include <QString>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>

typedef QMap<QString,QVariantMap> InterfaceList;
typedef QMap<QDBusObjectPath, InterfaceList> ManagedObjects;

class DBusObject {
protected:
	DBusObject(QString const &service, QDBusObjectPath const &path, QString const &interfaceBase);
	template <class R> R property(char const * const p, QString const &subInterface=QString()) const {
		QString interface=_interfaceBase;
		if(!subInterface.isEmpty())
			interface += QStringLiteral(".") + subInterface;
		QDBusInterface i(_service, _path.path(), interface, QDBusConnection::systemBus());
		return qvariant_cast<R>(i.property(p));
	}
	ManagedObjects managedObjects() const;
protected:
	QString const		_service;
	QDBusObjectPath const	_path;
	QString const		_interfaceBase;
};
