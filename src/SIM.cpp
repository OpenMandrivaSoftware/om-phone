#include "SIM.h"

SIM::SIM(QDBusObjectPath path):DBusObject(QStringLiteral("org.freedesktop.ModemManager1"), path, "org.freedesktop.ModemManager1.Sim") {
}

QString SIM::IMSI() const {
	return DBusObject::property<QString>("Imsi");
}

QStringList SIM::emergencyNumbers() const {
	return DBusObject::property<QStringList>("EmergencyNumbers");
}

QString SIM::operatorID() const {
	return DBusObject::property<QString>("OperatorIdentifier");
}

QString SIM::operatorName() const {
	return DBusObject::property<QString>("OperatorName");
}

QString SIM::identifier() const {
	return DBusObject::property<QString>("SimIdentifier");
}
