#include "PhoneKeyboard.h"

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
       _digits[1] = new RTPushButton(Key("1"), this, true);
       _layout.addWidget(_digits[1], 0, 0);
       _digits[2] = new RTPushButton(Key("2", "ABC"), this, true);
       _layout.addWidget(_digits[2], 0, 1);
       _digits[3] = new RTPushButton(Key("3", "DEF"), this, true);
       _layout.addWidget(_digits[3], 0, 2);

       _digits[4] = new RTPushButton(Key("4", "GHI"), this, true);
       _layout.addWidget(_digits[4], 1, 0);
       _digits[5] = new RTPushButton(Key("5", "JKL"), this, true);
       _layout.addWidget(_digits[5], 1, 1);
       _digits[6] = new RTPushButton(Key("6", "MNO"), this, true);
       _layout.addWidget(_digits[6], 1, 2);

       _digits[7] = new RTPushButton(Key("7", "PQRS"), this, true);
       _layout.addWidget(_digits[7], 2, 0);
       _digits[8] = new RTPushButton(Key("8", "TUV"), this, true);
       _layout.addWidget(_digits[8], 2, 1);
       _digits[9] = new RTPushButton(Key("9", "WXYZ"), this, true);
       _layout.addWidget(_digits[9], 2, 2);

       _star = new RTPushButton(Key("*"), this, true);
       _layout.addWidget(_star, 3, 0);
       _digits[0] = new RTPushButton(Key("0"), this, true);
       _layout.addWidget(_digits[0], 3, 1);
       _number = new RTPushButton(Key("#"), this, true);
       _layout.addWidget(_number, 3, 2);
}
