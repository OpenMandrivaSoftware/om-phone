#include "ModemApplication.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>

#include <iostream>

int main(int argc, char **argv) {
	ModemApplication app(argc, argv);
	app.exec();
}
