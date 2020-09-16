#include "PhoneWidget.h"

PhoneWidget::PhoneWidget(QWidget *parent):QStackedWidget(parent) {
	_kbd = new PhoneKeyboard(this);
	connect(_kbd, &PhoneKeyboard::callRequested, this, &PhoneWidget::callRequested);
	addWidget(_kbd);
}

/*
void PhoneWidget::closeEvent(QCloseEvent *e) {
	hide();
	e->ignore();
	return false;
}*/
