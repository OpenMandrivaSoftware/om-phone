#include "RTPushButton.h"
#include <QFile>
#include <QPainter>
#include <QResizeEvent>

static HapticFeedback *hf = nullptr;

RTPushButton::RTPushButton(QUrl const &url, QWidget *parent, bool hapticFeedback):QPushButton(QString(), parent),_text(this),_hapticFeedback(hapticFeedback) {
	QFile f(url.toLocalFile());
	if(f.open(QFile::ReadOnly)) {
		_text.setBaseUrl(url);
		_text.setHtml(f.readAll());
		f.close();
	} else
		_text.setHtml(url.toString());
}

RTPushButton::RTPushButton(QString const &text, QWidget *parent, bool hapticFeedback):QPushButton(QString(), parent),_text(this),_hapticFeedback(hapticFeedback) {
	_text.setHtml(text);
}

QSize RTPushButton::sizeHint() const {
	return QSize(_text.size().width(), _text.size().height());
}

void RTPushButton::mousePressEvent(QMouseEvent *e) {
	if(_hapticFeedback) {
		if(!hf) hf=new HapticFeedback;
		if(hf) hf->start();
	}
	QPushButton::mousePressEvent(e);
}

void RTPushButton::resizeEvent(QResizeEvent *e) {
	QPushButton::resizeEvent(e);
	_text.setTextWidth(e->size().width());
}

void RTPushButton::paintEvent(QPaintEvent *e) {
	QPushButton::paintEvent(e);
	QPainter p(this);
	_text.drawContents(&p);
}
