#pragma once

#include <QDBusObjectPath>
#include "DBusObject.h"

class SIM:public DBusObject {
public:
	SIM(QDBusObjectPath path);
	QString IMSI() const;
	QStringList emergencyNumbers() const;
	QString operatorID() const;
	QString operatorName() const;
	QString identifier() const;
};
