#ifndef QQSETTINGS_H
#define QQSETTINGS_H

#include <QObject>
#include <QMutex>
#include <QNetworkProxy>
#include <QString>
#include <QUrl>

class QQBouchot;
class QQPinipede;

class QAuthenticator;
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
	uint maxHistoryLength() { return m_maxHistoryLength; }

	void setDefaultUA(const QString&);
	QString defaultUA() { return m_defaultUA; }

	void setTotozServerUrl(const QUrl&);
	QUrl totozServerUrl() { return m_totozServerUrl; }

	void setTotozMode(TotozMode);
	TotozMode totozMode() { return m_totozMode; }

	void setDefaultLogin(const QString&);
	QString defaultLogin() { return m_defaultLogin; }

	void setProxyUser(const QString & proxyUser) { m_proxyUser = proxyUser; }
	QString ProxyUser() { return m_proxyUser; }

	void setProxyPasswd(const QString & proxyPasswd) { m_proxyUser = proxyPasswd; }
	QString ProxyPasswd() { return m_proxyPasswd; }

	void setListBouchots(const QList<QQBouchot *>&);
	QList<QQBouchot *> listBouchots() { return m_listBouchots; }
	bool hasBouchot(QString bouchotName) { return bouchot(bouchotName) != NULL; }
	QQBouchot * bouchot(QString bouchotName);
	void addBouchot(QQBouchot *);
	void addBouchots(const QList<QQBouchot *>&);
	void removeBouchot(const QString bouchotName);
	void startBouchots();
	void startBouchot(QString &);
	void stopBouchots();
	void stopBouchot(QString &);

	QList<QString> listTabs();

	void setPalmiMinimized(bool palmiMini);
	bool palmiMinimized() { return m_palmiMini; }

	void setDirty() { m_dirty = true; }

public slots:
	bool readSettings();
	bool saveSettings();
	bool maybeSave();
	void proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);

private:
	uint m_maxHistoryLength;
	QString m_defaultUA;
	QString m_defaultLogin;
	QUrl m_totozServerUrl;
	TotozMode m_totozMode;
	bool m_palmiMini;

	QString m_proxyUser;
	QString m_proxyPasswd;

	QList<QQBouchot *> m_listBouchots;
	bool m_dirty;

	QMutex m_proxyPopupMutex;
};

#endif // QQSETTINGS_H
