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
	QString IMEI() const { return property("Imei", QStringLiteral("Modem3gpp")); }
	QString manufacturer() const { return property("Manufacturer"); }
	QString model() const { return property("Model"); }

	QDBusObjectPath SIM() const;

protected:
	QString property(char const * const p, QString const &subInterface=QString()) const;

public Q_SLOTS:
	void messageAdded(QDBusObjectPath path, bool received);
	void voiceCallAdded(QDBusObjectPath path);

	bool sendSMS(QString const &recipient, QString const &text);

protected:
	QDBusObjectPath	_dbusPath;
};
