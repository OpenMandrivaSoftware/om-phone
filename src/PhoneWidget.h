#pragma once

#include <QStackedWidget>
#include <QProcess>
#include "PhoneKeyboard.h"
#include "CallUi.h"
#include "IncomingCallUi.h"

class PhoneWidget:public QStackedWidget {
	Q_OBJECT
public:
	PhoneWidget(QWidget *parent=nullptr);
Q_SIGNALS:
	void callRequested(QString const &number);
public Q_SLOTS:
	bool startIncomingCall(Call* call);
	bool startOutgoingCall(Call* call);
	void setNumber(QString const &number) { _kbd->setNumber(number); }
	void call() { _kbd->call(); }

	void acceptIncoming();
	void rejectIncoming();
	void hangup();
	void reset();
protected Q_SLOTS:
	void callStateChanged(Call::State old, Call::State New, Call::Reason reason);
protected:
	QProcess*	_ringtonePlayer;
	PhoneKeyboard*	_kbd;
	IncomingCallUi*	_incomingCall;
	CallUi*		_call;
	Call*		_activeCall;
};
