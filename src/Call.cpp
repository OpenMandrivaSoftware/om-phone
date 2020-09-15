#include "Call.h"
#include <QDBusReply>

#include <iostream>

QMap<QString,Call*> Call::_calls;

Call *Call::get(QDBusObjectPath path) {
	QString const p=path.path();
	if(!_calls.contains(p))
		_calls.insert(p, new Call(path));
	return _calls[p];
}

Call::Call(QDBusObjectPath path):DBusObject(QStringLiteral("org.freedesktop.ModemManager1"), path, "org.freedesktop.ModemManager1.Call") {
	QDBusConnection::systemBus().connect(_service, _path.path(), _interface, QStringLiteral("DtmfReceived"), this, SLOT(dtmfHandler(QString)));
	QDBusConnection::systemBus().connect(_service, _path.path(), _interface, QStringLiteral("StateChanged"), this, SLOT(stateChangeHandler(int, int, unsigned int)));
}

Call::~Call() {
	_calls.remove(_path.path());
}

bool Call::accept() {
	QDBusInterface intf(_service, _path.path(), _interface, QDBusConnection::systemBus());
	QDBusReply<void> reply = intf.call(QStringLiteral("Accept"));
	return reply.error().type() == QDBusError::NoError;
}

bool Call::deflect(QString const &number) {
	QDBusInterface intf(_service, _path.path(), _interface, QDBusConnection::systemBus());
	QDBusReply<void> reply = intf.call(QStringLiteral("Deflect"), number);
	return reply.error().type() == QDBusError::NoError;
}

bool Call::hangup() {
	QDBusInterface intf(_service, _path.path(), _interface, QDBusConnection::systemBus());
	QDBusReply<void> reply = intf.call(QStringLiteral("Hangup"));
	return reply.error().type() == QDBusError::NoError;
}

bool Call::sendDtmf(QString const &dtmf) {
	QDBusInterface intf(_service, _path.path(), _interface, QDBusConnection::systemBus());
	QDBusReply<void> reply = intf.call(QStringLiteral("SendDtmf"), dtmf);
	return reply.error().type() == QDBusError::NoError;
}

bool Call::start() {
	QDBusInterface intf(_service, _path.path(), _interface, QDBusConnection::systemBus());
	QDBusReply<void> reply = intf.call(QStringLiteral("Start"));
	return reply.error().type() == QDBusError::NoError;
}

void Call::dtmfHandler(QString dtmf) {
	emit dtmfReceived(dtmf);
}

void Call::stateChangeHandler(int old, int New, unsigned int reason) {
	std::cerr << qPrintable(_path.path()) << ": Call state changed from " << static_cast<State>(old) << " to " << static_cast<State>(New) << " because " << static_cast<Reason>(reason) << std::endl;
	emit stateChanged(old, New, reason);
}

Call::Direction Call::direction() const {
	return static_cast<Direction>(DBusObject::property<int>("Direction"));
}

Call::State Call::state() const {
	return static_cast<State>(DBusObject::property<int>("State"));
}

QString Call::number() const {
	return DBusObject::property<QString>("Number");
}

std::ostream &operator <<(std::ostream &o, Call::State const &s) {
	switch(s) {
	case Call::State::Unknown:
		return o << "Unknown";
	case Call::State::Dialing:
		return o << "Dialing";
	case Call::State::RingingOut:
		return o << "Ringing (out)";
	case Call::State::RingingIn:
		return o << "Ringing (in)";
	case Call::State::Active:
		return o << "Active";
	case Call::State::Held:
		return o << "Held";
	case Call::State::Waiting:
		return o << "Waiting";
	case Call::State::Terminated:
		return o << "Terminated";
	}
	return o << "Unknown state " << static_cast<int>(s);
}

std::ostream &operator <<(std::ostream &o, Call::Direction const &d) {
	switch(d) {
	case Call::Direction::Unknown:
		return o << "Unknown";
	case Call::Direction::Incoming:
		return o << "Incoming";
	case Call::Direction::Outgoing:
		return o << "Outgoing";
	}
	return o << "Unknown direction " << static_cast<int>(d);
}

std::ostream &operator <<(std::ostream &o, Call::Reason const &r) {
	switch(r) {
	case Call::Reason::Unknown:
		return o << "Unknown";
	case Call::Reason::OutgoingStarted:
		return o << "Outgoing started";
	case Call::Reason::IncomingNew:
		return o << "Incoming new";
	case Call::Reason::Accepted:
		return o << "Accepted";
	case Call::Reason::Terminated:
		return o << "Terminated";
	case Call::Reason::RefusedOrBusy:
		return o << "Refused or busy";
	case Call::Reason::Error:
		return o << "Error";
	case Call::Reason::AudioSetupFailed:
		return o << "Audio setup failed";
	case Call::Reason::Transferred:
		return o << "Transferred";
	case Call::Reason::Deflected:
		return o << "Deflected";
	}
	return o << "Unknown reason " << static_cast<int>(r);
}
