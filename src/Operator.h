#pragma once
#include <QVariant>
#include <cstdint>
#include "DBusObject.h"

class Operator:public QVariantMap {
public:
	Operator(QVariantMap const &info) { for(QVariantMap::ConstIterator it=info.constBegin(); it!=info.constEnd(); it++) insert(it.key(), it.value()); }
	uint32_t accessTechnology() const { return value(QStringLiteral("access-technology"), QVariant(0)).toUInt(); }
	QString operatorName() const { return value(QStringLiteral("operator-long")).toString(); }
	QString operatorCode() const { return value(QStringLiteral("operator-code")).toString(); }
	uint32_t status() const { return value(QStringLiteral("status"), QVariant(0)).toUInt(); }
};
