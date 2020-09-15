#pragma once

#include <QPushButton>
#include <QTextDocument>
#include <QUrl>

#include "HapticFeedback.h"

class RTPushButton:public QPushButton {
	Q_OBJECT
public:
	RTPushButton(QUrl const &url, QWidget *parent = nullptr, bool hapticFeedback=false);
	RTPushButton(QString const &text, QWidget *parent = nullptr, bool hapticFeedback=false);
	QSize sizeHint() const override;
protected:
	void mousePressEvent(QMouseEvent *e) override;
	void resizeEvent(QResizeEvent *) override;
	void paintEvent(QPaintEvent *) override;
protected:
	QTextDocument		_text;
	bool			_hapticFeedback;
};
