#pragma once

#include <QCoreApplication>
#include <QDBusObjectPath>
#include <QString>

class Modem:public QObject {
	Q_OBJECT
public:
	Modem(QDBusObjectPath const &obj);
	Modem(int index);

	QDateTime networkTime() const;
	QString IMEI() const;

public Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void voiceCallAdded(QDBusObjectPath path);

	bool sendSMS(QString const &recipient, QString const &text);

protected:
	QDBusObjectPath	_dbusPath;
};
