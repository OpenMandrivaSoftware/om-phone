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
		std::cerr << "Failed to register on session bus" << std::endl;
		QTimer::singleShot(0, this, &QCoreApplication::quit);
		return;
	}
	sb.registerObject("/", this, QDBusConnection::ExportScriptableSlots);

	std::cerr << "Using ModemManager " << qPrintable(mmVersion()) << std::endl;
	QList<QDBusObjectPath> mdms = modems();
	std::cerr << "Found " << mdms.count() << " modems: " << std::endl;
	for(QDBusObjectPath const &p : mdms) {
		Modem *m = new Modem(p);
		connect(m, &Modem::incomingCall, this, &Phone::incomingCall);
		_modems << m;
		std::cerr << "	" << qPrintable(m->manufacturer()) << " " << qPrintable(m->model()) << std::endl;
		std::cerr << "		IMEI: " << qPrintable(m->IMEI()) << std::endl;
		std::cerr << "		Network time: " << qPrintable(m->networkTime().toString()) << std::endl;
		SIM s = m->SIM();
		std::cerr << "		SIM identifier: " << qPrintable(s.identifier()) << std::endl;
		std::cerr << "		SIM IMSI: " << qPrintable(s.IMSI()) << std::endl;
		std::cerr << "		SIM emergency numbers: " << qPrintable(s.emergencyNumbers().join(QStringLiteral(", "))) << std::endl;
		std::cerr << "		SIM operator: " << qPrintable(s.operatorName()) << " (" << qPrintable(s.operatorID()) << ")" << std::endl;
	}

	_ui=new PhoneWidget;
	_ui->resize(300, 400);
	connect(_ui, &PhoneWidget::callRequested, this, &Phone::call);
	if(!arguments().contains("--start-hidden"))
		_ui->show();

	AudioManager::get()->enableSpeaker();
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
		std::cerr << "Message received: ";
	else
		std::cerr << "Message added: ";

	std::cerr << qPrintable(path.path()) << std::endl;
}

void Phone::incomingCall(Call* call) {
	std::cerr << "incoming call detected" << std::endl;
	std::cerr << "Calling number: " << qPrintable(call->number()) << std::endl;
	std::cerr << "Formatted: " << qPrintable(call->formattedNumber()) << std::endl;
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
	std::cerr << "Show called" << std::endl;
	std::cerr << qPrintable(number) << std::endl;
	if(!number.isEmpty())
	_ui->setNumber(number);
	_ui->show();
	_ui->raise();
	if(!number.isEmpty())
		_ui->call();
	return true;
}

bool Phone::hide() {
	std::cerr << "Hide called" << std::endl;
	return false;
}

bool Phone::call(QString const &number) {
	if(_modems.count()<1) {
		std::cerr << "No modem found -- can't call" << std::endl;
		return false;
	}
	// FIXME at some point, we should handle devices with
	// multiple modems instead of using the first one
	Call *c = _modems.at(0)->call(number);
	return _ui->startOutgoingCall(c);
}
