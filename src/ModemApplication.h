#pragma once

#include <QCoreApplication>
#include <QDBusObjectPath>
#include <QList>

#include "Modem.h"

class ModemApplication:public QCoreApplication {
	Q_OBJECT
public:
	ModemApplication(int &argc, char **&argv);
	QString mmVersion() const;
	QList<QDBusObjectPath> modems() const;

public Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void voiceCallAdded(QDBusObjectPath path);

protected:
	QList<Modem*> _modems;
};
