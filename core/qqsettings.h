#ifndef QQSETTINGS_H
#define QQSETTINGS_H

#include <QObject>
#include <QString>
#include <QUrl>

class QQBouchot;
class QQPinipede;

class QSettings;
class QString;

class QQSettings : public QObject
{
	Q_OBJECT

public:
	enum TotozMode { Inline_Mode = 0, Popup_Mode = 1, Bald_Mode = 2 };

	explicit QQSettings(QObject *parent = 0);
	~QQSettings();

	void setMaxHistoryLength(uint);
	uint maxHistoryLength();

	void setDefaultUA(const QString&);
	QString defaultUA();

	void setTotozServerUrl(const QUrl&);
	QUrl totozServerUrl();

	void setTotozMode(TotozMode);
	TotozMode totozMode();

	void setDefaultLogin(const QString&);
	QString defaultLogin();

	void setListBouchots(const QList<QQBouchot *>&);
	QList<QQBouchot *> listBouchots();
	bool hasBouchot(QQBouchot *);
	void addBouchot(QQBouchot *);
	void addBouchots(const QList<QQBouchot *>&);
	void removeBouchots(const QList<QQBouchot *>&);
	void startBouchots();
	void startBouchot(QString &);
	void stopBouchots();
	void stopBouchot(QString &);

	QList<QString> getListTabs();

	void setPinipede( QQPinipede * );

public slots:
	bool readSettings();
	bool saveSettings();
	bool maybeSave();

private:
	uint m_maxHistoryLength;
	QString m_defaultUA;
	QString m_defaultLogin;
	QUrl m_totozServerUrl;
	TotozMode m_totozMode;

	QList<QQBouchot *> m_listBouchots;
	bool dirty;
};

#endif // QQSETTINGS_H
