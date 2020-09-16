#include "PhoneKeyboard.h"
#include "Phone.h"
#include <QLocale>
#include <iostream>

static inline QString Key(char const * const digit, char const * const letters=nullptr) {
	QString html=QString::fromLatin1("<center><big><b>") + QString::fromLatin1(digit) + QStringLiteral("</b></big>");
	if(letters)
		html += QStringLiteral("<br/><small>") + QString::fromLatin1(letters) + QStringLiteral("</small>");
	else
		html += QStringLiteral("<br/><small>&nbsp;</small>");
	html += QStringLiteral("</center>");
	return html;
}

PhoneKeyboard::PhoneKeyboard(QWidget *parent):QWidget(parent),_layout(this) {
#ifdef USE_LIBPHONENUMBER
	_phoneNumberUtil = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
	_phoneNumberFormatter = _phoneNumberUtil->PhoneNumberUtil::GetAsYouTypeFormatter(static_cast<Phone*>(QCoreApplication::instance())->phoneLocale());
#endif
	_phoneNumber = new QLabel(this);
	_layout.addWidget(_phoneNumber, 0, 0, 1, 3);

       _digits[1] = new RTPushButton(Key("1"), this, true);
       _layout.addWidget(_digits[1], 1, 0);
       _digits[2] = new RTPushButton(Key("2", "ABC"), this, true);
       _layout.addWidget(_digits[2], 1, 1);
       _digits[3] = new RTPushButton(Key("3", "DEF"), this, true);
       _layout.addWidget(_digits[3], 1, 2);

       _digits[4] = new RTPushButton(Key("4", "GHI"), this, true);
       _layout.addWidget(_digits[4], 2, 0);
       _digits[5] = new RTPushButton(Key("5", "JKL"), this, true);
       _layout.addWidget(_digits[5], 2, 1);
       _digits[6] = new RTPushButton(Key("6", "MNO"), this, true);
       _layout.addWidget(_digits[6], 2, 2);

       _digits[7] = new RTPushButton(Key("7", "PQRS"), this, true);
       _layout.addWidget(_digits[7], 3, 0);
       _digits[8] = new RTPushButton(Key("8", "TUV"), this, true);
       _layout.addWidget(_digits[8], 3, 1);
       _digits[9] = new RTPushButton(Key("9", "WXYZ"), this, true);
       _layout.addWidget(_digits[9], 3, 2);

	_star = new RTPushButton(Key("*"), this, true);
	_layout.addWidget(_star, 4, 0);
	_digits[0] = new RTPushButton(Key("0"), this, true);
	_layout.addWidget(_digits[0], 4, 1);
	_number = new RTPushButton(Key("#"), this, true);
	_layout.addWidget(_number, 4, 2);

	_backspace = new RTPushButton(Key("&#x232b;"), this, true); // 2190
	_layout.addWidget(_backspace, 5, 0);
	_plus = new RTPushButton(Key("+"), this, true);
	_layout.addWidget(_plus, 5, 1);
	_dial = new RTPushButton(Key("&#x260e;"), this, true); // 2713
	_layout.addWidget(_dial, 5, 2);

	for(int i=0; i<10; i++)
		connect(_digits[i], &RTPushButton::clicked, this, &PhoneKeyboard::keyClicked);
	connect(_star, &RTPushButton::clicked, this, &PhoneKeyboard::keyClicked);
	connect(_number, &RTPushButton::clicked, this, &PhoneKeyboard::keyClicked);
	connect(_backspace, &RTPushButton::clicked, this, &PhoneKeyboard::bsClicked);
	connect(_plus, &RTPushButton::clicked, this, &PhoneKeyboard::keyClicked);
	connect(_dial, &RTPushButton::clicked, this, &PhoneKeyboard::call);
}

void PhoneKeyboard::keyClicked() {
	char digit = 0;
	for(uint8_t i=0; i<10; i++) {
		if(sender() == _digits[i]) {
			digit = '0'+i;
			break;
		}
	}
	if(!digit) {
		if(sender() == _star)
			digit = '*';
		else if(sender() == _number)
			digit = '#';
		else if(sender() == _plus)
			digit = '+';
	}
#ifdef USE_LIBPHONENUMBER
	_phoneNumberFormatter->InputDigit(digit, &_formattedNumber);
	_phoneNumber->setText(QString::fromStdString(_formattedNumber));
#else
	_phoneNumber->setText(_phoneNumber->text() + QChar(digit));
#endif
}

void PhoneKeyboard::bsClicked() {
	QString const t=_phoneNumber->text();
	setNumber(t.left(t.length()-1));
}

void PhoneKeyboard::setNumber(QString const &number) {
#ifdef USE_LIBPHONENUMBER
	std::string s=number.toStdString();
	_phoneNumberUtil->NormalizeDiallableCharsOnly(&s);
	_formattedNumber.clear();
	_phoneNumberFormatter->Clear();
	for(int i=0; i<s.length(); i++)
		_phoneNumberFormatter->InputDigit(s[i], &_formattedNumber);
	_phoneNumber->setText(QString::fromStdString(_formattedNumber));
#else
	_phoneNumber->setText(number);
#endif
}

void PhoneKeyboard::call() {
#ifdef USE_LIBPHONENUMBER
	std::string n = _formattedNumber;
	_phoneNumberUtil->NormalizeDiallableCharsOnly(&n);
	emit callRequested(QString::fromStdString(n));
#else
	emit callRequested(_phoneNumber->text());
#endif
}
