#include "PhoneWidget.h"
#include "AudioManager.h"
#include <QProcess>
#include <QDBusReply>

PhoneWidget::PhoneWidget(QWidget* parent):QStackedWidget(parent),_ringtonePlayer(nullptr),_activeCall(nullptr) {
	_kbd = new PhoneKeyboard(this);
	connect(_kbd, &PhoneKeyboard::callRequested, this, &PhoneWidget::callRequested);
	addWidget(_kbd);
	_incomingCall = new IncomingCallUi(this);
	connect(_incomingCall, &IncomingCallUi::accept, this, &PhoneWidget::acceptIncoming);
	connect(_incomingCall, &IncomingCallUi::reject, this, &PhoneWidget::rejectIncoming);
	addWidget(_incomingCall);
	_call = new CallUi(this);
	connect(_call, &CallUi::hangup, this, &PhoneWidget::hangup);
	addWidget(_call);
	setCurrentWidget(_kbd);
}

bool PhoneWidget::startIncomingCall(Call* call) {
	// Make sure we're visible:
	// Make sure the screen is turned on
	QDBusInterface powerInterface(QStringLiteral("local.org_kde_powerdevil"), QStringLiteral("/org/kde/Solid/PowerManagement"), QStringLiteral("org.kde.Solid.PowerManagement"), QDBusConnection::sessionBus());
	powerInterface.call("wakeup");
	// Kill the screen locker
	QDBusInterface lockInterface(QStringLiteral("org.freedesktop.login1"), QStringLiteral("/org/freedesktop/login1"), QStringLiteral("org.freedesktop.login1.Manager"), QDBusConnection::systemBus());
	lockInterface.call("UnlockSessions");
	// And make sure brightness isn't 0
	QDBusInterface backlightInterface(QStringLiteral("org.kde.Solid.PowerManagement"), QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"), QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"), QDBusConnection::sessionBus());
	QDBusReply<int> brightness=backlightInterface.call("brightness");
	QDBusReply<int> maxBrightness=backlightInterface.call("brightnessMax");
	qDebug("brightness: %u / %u", brightness.value(), maxBrightness.value());
	if(brightness < maxBrightness/10)
		backlightInterface.call("setBrightness", maxBrightness*.8);

	_incomingCall->incomingCall(call);
	setCurrentWidget(_incomingCall);
	show();
	raise();
	_activeCall=call;
	connect(call, &Call::stateChanged, this, &PhoneWidget::callStateChanged);
	call->start();
	// Make sure the ringtone is audible
	// FIXME this needs some abstraction to work on devices other than the PinePhone
	AudioManager::get()->enableSpeaker();
	if(_ringtonePlayer) {
		_ringtonePlayer->terminate();
		delete _ringtonePlayer;
	}
	_ringtonePlayer = new QProcess(this);
	// FIXME don't hardcode the filename
	_ringtonePlayer->start("/usr/bin/mpv", QStringList() << "-loop" << "/usr/share/sounds/Oxygen-Im-Phone-Ring.ogg");
	return true;
}

bool PhoneWidget::startOutgoingCall(Call* call) {
	setCurrentWidget(_call);
	show();
	raise();
	connect(call, &Call::stateChanged, this, &PhoneWidget::callStateChanged);
	_activeCall=call;
	bool res= call->start();
	AudioManager::get()->enableRouting();
	return res;
}

void PhoneWidget::callStateChanged(Call::State old, Call::State New, Call::Reason reason) {
	if(old == Call::State::RingingIn) {
		_ringtonePlayer->terminate();
		delete _ringtonePlayer;
		_ringtonePlayer = nullptr;
	}
	if(New == Call::State::Active || New == Call::State::Dialing || New == Call::State::RingingOut) {
		setCurrentWidget(_call);
	} else if(New == Call::State::Terminated) {
		setCurrentWidget(_kbd);
	}
}

// FIXME this stuff needs to move to class Phone so we can
// handle headless/scripted stuff
void PhoneWidget::acceptIncoming() {
	if(_activeCall)
	{
		_activeCall->accept();
		AudioManager::get()->enableRouting();
	}
}

void PhoneWidget::rejectIncoming() {
	if(_activeCall)
		_activeCall->hangup();
}

void PhoneWidget::hangup() {
	if(_activeCall)
		_activeCall->hangup();
}

void PhoneWidget::reset() {
	// This happens if ModemManager tells us the modem has been
	// removed -- could be powered off, or down for a firmware
	// update, or whatever. Either way we shouldn't be stuck
	// on a call.
	setCurrentWidget(_kbd);
}
