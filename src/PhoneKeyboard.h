#pragma once

#include <QGridLayout>
#include <QLabel>
#include "RTPushButton.h"

#ifdef USE_LIBPHONENUMBER
#include <string>
#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/asyoutypeformatter.h>
#endif

class PhoneKeyboard:public QWidget {
	Q_OBJECT
public:
	PhoneKeyboard(QWidget *parent=nullptr);
protected Q_SLOTS:
	void keyClicked();
	void bsClicked();
	void callClicked();
protected:
	QGridLayout	_layout;
	QLabel		*_phoneNumber;
	RTPushButton	*_digits[10];
	RTPushButton	*_star;
	RTPushButton	*_number;
	RTPushButton	*_backspace;
	RTPushButton	*_plus;
	RTPushButton	*_dial;
#ifdef USE_LIBPHONENUMBER
	std::string _formattedNumber;
	i18n::phonenumbers::PhoneNumberUtil* _phoneNumberUtil;
	i18n::phonenumbers::AsYouTypeFormatter* _phoneNumberFormatter;
#endif
};
