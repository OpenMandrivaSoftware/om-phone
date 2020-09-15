#pragma once

#include <QCoreApplication>
#include <QDBusObjectPath>
#include <QString>

#include "Modem.h"

class ModemApplication:public QCoreApplication {
	Q_OBJECT
public:
	ModemApplication(int &argc, char **&argv);
	QString mmVersion() const;

public Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void voiceCallAdded(QDBusObjectPath path);

protected:
	Modem _m0;
};
