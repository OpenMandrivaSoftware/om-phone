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
		QString arg;
		for(int i=1; i<argc; i++) {
			if(argv[i][0] != '-') {
				arg=argv[i];
				break;
			}
		}
		if(!arg.isEmpty())
			r=intf.call("show", arg);
		else
			r=intf.call("show");
		return 0;
	}

	Phone app(argc, argv);
	app.exec();
}
