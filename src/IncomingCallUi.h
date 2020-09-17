#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "RTPushButton.h"
#include "Call.h"

class IncomingCallUi:public QWidget {
	Q_OBJECT
public:
	IncomingCallUi(QWidget *parent=nullptr);
Q_SIGNALS:
	void accept();
	void reject();
	void redirect();
public Q_SLOTS:
	void incomingCall(Call* call);
protected:
	QGridLayout	_layout;
	QLabel *	_explanation;
	RTPushButton *	_accept;
	RTPushButton *	_reject;
	RTPushButton *	_redirect;
	Call *		_activeCall;
};
