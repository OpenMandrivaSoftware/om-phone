#pragma once

#include <QGridLayout>
#include "RTPushButton.h"

class PhoneKeyboard:public QWidget {
	Q_OBJECT
public:
	PhoneKeyboard(QWidget *parent=nullptr);
protected:
	QGridLayout	_layout;
	RTPushButton	*_digits[10];
	RTPushButton	*_star;
	RTPushButton	*_number;
};
