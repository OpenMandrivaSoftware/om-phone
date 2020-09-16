#pragma once

#include <QStackedWidget>
#include "PhoneKeyboard.h"

class PhoneWidget:public QStackedWidget {
	Q_OBJECT
public:
	PhoneWidget(QWidget *parent=nullptr);
protected:
//	void closeEvent(QCloseEvent *event) override;
Q_SIGNALS:
	void callRequested(QString const &number);
public Q_SLOTS:
	void setNumber(QString const &number) { _kbd->setNumber(number); }
	void call() { _kbd->call(); }
protected:
	PhoneKeyboard*	_kbd;
};
