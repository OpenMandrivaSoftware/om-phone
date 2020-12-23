#include "Phone.h"
#include "PhoneWidget.h"
#include "AudioManager.h"
#include "SIM.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>

#include <QDateTime>
#include <QTimer>

#include <QLabel>

#include <iostream>
#include <cassert>

Phone::Phone(int &argc, char **&argv):QApplication(argc, argv),DBusObject(QStringLiteral("org.freedesktop.ModemManager1"), QDBusObjectPath("/org/freedesktop/ModemManager1"), "org.freedesktop.ModemManager1") {
	setApplicationDisplayName("Phone");
	setOrganizationName("LinDev");
	setOrganizationDomain("lindev.ch");
	setQuitOnLastWindowClosed(false);

	assert(QDBusConnection::systemBus().interface()->isServiceRegistered(_service));
	QDBusConnection sb=QDBusConnection::sessionBus();

	if(!sb.registerService("ch.lindev.phone")) {
		qCritical("Failed to register on session bus");
		QTimer::singleShot(0, this, &QCoreApplication::quit);
		return;
	}
	sb.registerObject("/", this, QDBusConnection::ExportScriptableSlots);
	qInfo("Using ModemManager %s", qPrintable(mmVersion()));

	QDBusConnection::systemBus().connect(_service, _path.path(), QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesAdded"), this, SLOT(interfaceAdded()));
	QDBusConnection::systemBus().connect(_service, _path.path(), QStringLiteral("org.freedesktop.DBus.ObjectManager"), QStringLiteral("InterfacesRemoved"), this, SLOT(interfaceRemoved()));
	scanModems();

	_ui=new PhoneWidget;
	_ui->resize(300, 400);
	connect(_ui, &PhoneWidget::callRequested, this, &Phone::call);
	if(!arguments().contains("--start-hidden"))
		_ui->show();

	AudioManager::get()->enableSpeaker();
}

void Phone::scanModems() {
	QList<QDBusObjectPath> mdms = modems();
	qInfo("Found %u modems:", mdms.count());
	for(QDBusObjectPath const &p : mdms) {
		Modem *m = new Modem(p);
		connect(m, &Modem::incomingCall, this, &Phone::incomingCall);
		_modems << m;
		qInfo("	%s %s", qPrintable(m->manufacturer()), qPrintable(m->model()));
		qInfo("		IMEI: %s", qPrintable(m->IMEI()));
		qInfo("		Network time: %s", qPrintable(m->networkTime().toString()));
		SIM s = m->SIM();
		qInfo("		SIM identifier: %s", qPrintable(s.identifier()));
		qInfo("		SIM IMSI: %s", qPrintable(s.IMSI()));
		qInfo("		SIM emergency numbers: %s", qPrintable(s.emergencyNumbers().join(QStringLiteral(", "))));
		qInfo("		SIM operator: %s (%s)", qPrintable(s.operatorName()), qPrintable(s.operatorID()));
	}
}

QString Phone::mmVersion() const {
	return DBusObject::property<QString>("Version");
}

QList<QDBusObjectPath> Phone::modems() const {
	QList<QDBusObjectPath> ret;
	ManagedObjects obj=managedObjects();
	for(auto const &k : obj.keys()) {
		if(k.path().contains("/Modem/"))
			ret << k;
	}
	return ret;
}

// We're returning an std::string rather than a QString because the
// only real use of the function is passing its result to libphonenumber
std::string Phone::phoneLocale() const {
	// FIXME given this is used for formatting phone numbers, it may
	// make more sense to determine the SIM card's home country
	// instead of going for the system locale
	return QLocale::system().name().split('_').constLast().toUtf8().constData();
}

void Phone::messageAdded(QDBusObjectPath path, bool received) {
	if(received)
		qInfo("Message received: %s", qPrintable(path.path()));
	else
		qInfo("Message added: %s", qPrintable(path.path()));
}

void Phone::incomingCall(Call* call) {
	qInfo("Incoming call detected");
	qInfo("Calling number: %s", qPrintable(call->number()));
	qInfo("Formatted: %s", qPrintable(call->formattedNumber()));
	_ui->startIncomingCall(call);
}

bool Phone::show(QString url) {
	QString number;
	if(url.startsWith("call://"))
		number=url.mid(7);
	else if(url.startsWith("call:"))
		number=url.mid(5);
	else if(url.startsWith("tel://"))
		number=url.mid(6);
	else if(url.startsWith("tel:"))
		number=url.mid(4);
	else
		number=url;
	qDebug("Show called");
	if(!number.isEmpty()) {
		qDebug("with number %s", qPrintable(number));
		_ui->setNumber(number);
	}
	_ui->show();
	_ui->raise();
	if(!number.isEmpty())
		_ui->call();
	return true;
}

bool Phone::hide() {
	qDebug("Hide called");
	return false;
}

bool Phone::call(QString const &number) {
	if(_modems.count()<1) {
		qCritical("No modem found -- can't call");
		return false;
	}
	// FIXME at some point, we should handle devices with
	// multiple modems instead of using the first one
	Call *c = _modems.at(0)->call(number);
	return _ui->startOutgoingCall(c);
}

void Phone::interfaceRemoved() {
	// ModemManager notified us an interface has been removed.
	// Let's make sure it isn't a modem we're using.
	qInfo("ModemManager D-Bus interface removed");
	if(_modems.count()) {
		if(!_modems.at(0)->ping()) {
			qCritical("Modem was removed");
			delete _modems.at(0);
			_modems.clear();
			_ui->reset();
		} else
			qInfo("Modem is still ok");
	}
}

void Phone::interfaceAdded() {
	qInfo("ModemManager D-Bus interface added -- checking if it's a new modem");
	if(!_modems.count()) {
		scanModems();
		if(_modems.count())
			qInfo("Modem found, resuming normal operations");
	}
}
