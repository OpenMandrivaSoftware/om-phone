#include "IncomingCallUi.h"

static inline QString ButtonHtml(char const * const symbol, QString const explanation=QString()) {
	QString html=QString::fromLatin1("<center><font size=\"800\"><b>") + QString::fromLatin1(symbol) + QStringLiteral("</b></font><br/>");
	if(explanation.isEmpty())
		html += QStringLiteral("<br/><small>&nbsp;</small>");
	else
		html += QStringLiteral("<br/><small>") + explanation + QStringLiteral("</small>");
	html += QStringLiteral("</center>");
	return html;
}

IncomingCallUi::IncomingCallUi(QWidget *parent):QWidget(parent),_layout(this),_activeCall(nullptr) {
	_explanation=new QLabel(this);
	_layout.addWidget(_explanation, 0, 0, 1, 2);
	_accept=new RTPushButton(ButtonHtml("&#x2713;", tr("Accept")), this, true);
	connect(_accept, &RTPushButton::clicked, this, &IncomingCallUi::accept);
	_layout.addWidget(_accept, 1, 0);
	_reject=new RTPushButton(ButtonHtml("&#x2718;", tr("Hang up")), this, true);
	connect(_reject, &RTPushButton::clicked, this, &IncomingCallUi::reject);
	_layout.addWidget(_reject, 1, 1);
	_redirect=new RTPushButton(ButtonHtml("&#x27a1;", tr("Redirect")), this, true);
	connect(_redirect, &RTPushButton::clicked, this, &IncomingCallUi::redirect);
	_layout.addWidget(_redirect, 1, 2);
}

void IncomingCallUi::incomingCall(Call* call) {
	_explanation->setText(tr("Incoming call from %1").arg(call->formattedNumber()));
}
