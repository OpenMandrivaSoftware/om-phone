#include "CallUi.h"
#include "AudioManager.h"

static inline QString ButtonHtml(char const * const symbol, QString const &explanation=QString()) {
	QString html=QString::fromLatin1("<center><font size=\"800\"><b>") + QString::fromLatin1(symbol) + QStringLiteral("</b></font><br/>");
	if(explanation.isEmpty())
		html += QStringLiteral("<br/><small>&nbsp;</small>");
	else
		html += QStringLiteral("<br/><small>") + explanation + QStringLiteral("</small>");
	html += QStringLiteral("</center>");
	return html;
}

CallUi::CallUi(QWidget *parent):QWidget(parent),_layout(this) {
	_explanation=new QLabel(this);
	_layout.addWidget(_explanation, 0, 0, 1, 2);
	_hangup=new RTPushButton(ButtonHtml("&#x2718;", tr("Hang up")), this, true);
	connect(_hangup, &RTPushButton::clicked, this, &CallUi::hangup);
	_layout.addWidget(_hangup, 1, 0);
	_redirect=new RTPushButton(ButtonHtml("&#x27a1;", tr("Redirect")), this, true);
	_layout.addWidget(_redirect, 1, 1);
	_speaker=new RTPushButton(ButtonHtml("&#x269f;", tr("Speaker On")), this, true);
	_layout.addWidget(_speaker, 2, 0);
	_mute=new RTPushButton(ButtonHtml("&#x266b;", tr("Mute")), this, true);
	_layout.addWidget(_mute, 2, 1);
	connect(_speaker, &RTPushButton::clicked, this, &CallUi::speakerClicked);
	connect(_mute, &RTPushButton::clicked, this, &CallUi::muteClicked);
}

void CallUi::incomingCall(Call* call) {
	_explanation->setText(tr("Call with %1").arg(call->formattedNumber()));
}

void CallUi::speakerClicked() {
	_speaker->setDown(!_speaker->isDown());
	if(_speaker->isDown())
		AudioManager::get()->enableSpeaker();
	else
		AudioManager::get()->enableEarpiece();
	// May need to be re-enabled to ensure routing between both
	// active components
	AudioManager::get()->enableRouting();
}

void CallUi::muteClicked() {
	_mute->setDown(!_mute->isDown());
	if(_mute->isDown())
		AudioManager::get()->muteMicrophone();
	else
		AudioManager::get()->enableMicrophone();
	// May need to be re-enabled to ensure routing between both
	// active components
	AudioManager::get()->enableRouting();
}
