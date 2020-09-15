#pragma once

#include <QDBusObjectPath>
#include "DBusObject.h"

extern "C" {
// Used only for enums -- no linkage
#include <ModemManager.h>
}

class Call:public QObject,public DBusObject {
	Q_OBJECT
public:
	enum class State : uint8_t {
		Unknown = MM_CALL_STATE_UNKNOWN,
		Dialing = MM_CALL_STATE_DIALING,
		RingingOut = MM_CALL_STATE_RINGING_OUT,
		RingingIn = MM_CALL_STATE_RINGING_IN,
		Active = MM_CALL_STATE_ACTIVE,
		Held = MM_CALL_STATE_HELD,
		Waiting = MM_CALL_STATE_WAITING,
		Terminated = MM_CALL_STATE_TERMINATED
	};

	enum class Direction : uint8_t {
		Unknown = MM_CALL_DIRECTION_UNKNOWN,
		Incoming = MM_CALL_DIRECTION_INCOMING,
		Outgoing = MM_CALL_DIRECTION_OUTGOING
	};

	enum class Reason : uint8_t {
		Unknown = MM_CALL_STATE_REASON_UNKNOWN,
		OutgoingStarted = MM_CALL_STATE_REASON_OUTGOING_STARTED,
		IncomingNew = MM_CALL_STATE_REASON_INCOMING_NEW,
		Accepted = MM_CALL_STATE_REASON_ACCEPTED,
		Terminated = MM_CALL_STATE_REASON_TERMINATED,
		RefusedOrBusy = MM_CALL_STATE_REASON_REFUSED_OR_BUSY,
		Error = MM_CALL_STATE_REASON_ERROR,
		AudioSetupFailed = MM_CALL_STATE_REASON_AUDIO_SETUP_FAILED,
		Transferred = MM_CALL_STATE_REASON_TRANSFERRED,
		Deflected = MM_CALL_STATE_REASON_DEFLECTED
	};
private:
	Call(QDBusObjectPath path);
	~Call();
public:
	static Call *get(QDBusObjectPath path);
public:
	bool sendDtmf(QString const &dtmf);
	Direction direction() const;
	State state() const;
	QString number() const;
public Q_SLOTS:
	bool accept();
	bool hangup();
	bool start();
	bool deflect(QString const &number);
protected Q_SLOTS:
	void dtmfHandler(QString dtmf);
	void stateChangeHandler(int old, int New, unsigned int reason);
Q_SIGNALS:
	void dtmfReceived(QString dtmf);
	void stateChanged(int old, int New, unsigned int reason);
private:
	static QMap<QString,Call*> _calls;
};

std::ostream &operator <<(std::ostream &o, Call::State const &s);
std::ostream &operator <<(std::ostream &o, Call::Direction const &d);
std::ostream &operator <<(std::ostream &o, Call::Reason const &r);
