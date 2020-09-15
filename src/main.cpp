#include "Phone.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>

#include <iostream>

int main(int argc, char **argv) {
	if(QDBusConnection::sessionBus().interface()->isServiceRegistered("ch.lindev.phone")) {
		// Activate the previous instance instead of launching a new one
		QDBusInterface intf(QStringLiteral("ch.lindev.phone"), "/", "", QDBusConnection::sessionBus());
		QDBusReply<void> r;
		if(argc>1)
			r=intf.call("show", argv[1]);
		else
			r=intf.call("show");
		return 0;
	}

	Phone app(argc, argv);
	app.exec();
}
