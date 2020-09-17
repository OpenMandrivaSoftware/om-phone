#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "RTPushButton.h"
#include "Call.h"

class CallUi:public QWidget {
	Q_OBJECT
public:
	CallUi(QWidget *parent=nullptr);
protected Q_SLOTS:
	void incomingCall(Call* call);
	void speakerClicked();
	void muteClicked();
protected:
	QGridLayout	_layout;
	QLabel *	_explanation;
	RTPushButton *	_hangup;
	RTPushButton *	_redirect;
	RTPushButton *	_speaker;
	RTPushButton *	_mute;
};
