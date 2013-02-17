#ifndef QQSETTINGS_H
#define QQSETTINGS_H

#include <QMutex>
#include <QNetworkProxy>
#include <QSettings>
#include <QString>

class QQBouchot;
class QQPinipede;

class QAuthenticator;
class QSettings;
class QString;

#define SETTINGS_MAINWINDOW_STATE "mainwindow_state"
#define SETTINGS_MAINWINDOW_GEOMETRY "mainwindow_geometry"

#define SETTINGS_TOTOZ_BOOKMARKLIST "totoz_bookmarklist"

#define SETTINGS_TOTOZ_SERVER_URL "totoz_server_url"
#define DEFAULT_TOTOZ_SERVER_URL "http://totoz.eu"

#define SETTINGS_TOTOZ_SERVER_BASE_IMG "totoz_server_base_img"
#define DEFAULT_TOTOZ_SERVER_BASE_IMG "img"

#define SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH "totoz_serveur_allow_search"
#define DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH 1

#define SETTINGS_TOTOZ_SERVER_QUERY_PATTERN "totoz_query_pattern"
#define DEFAULT_TOTOZ_SERVER_QUERY_SEARCH "search.xml?terms=%t&offset=%o"

class QQSettings : public QSettings
{
	Q_OBJECT

public:
	enum TotozMode { Inline_Mode = 0, Popup_Mode = 1, Bald_Mode = 2 };

	explicit QQSettings(QObject * parent = 0);
	~QQSettings();

	void setMaxHistoryLength(unsigned int maxHistoryLength) { this->m_maxHistoryLength = maxHistoryLength; }
	unsigned int maxHistoryLength() { return m_maxHistoryLength; }

	void setDefaultUA(const QString & defaultUA) { m_defaultUA = defaultUA; }
	QString defaultUA();

	void setTotozMode(QQSettings::TotozMode totozMode) { m_totozMode = totozMode; }
	TotozMode totozMode() { return m_totozMode; }

	void setDefaultLogin(const QString & defaultLogin) { m_defaultLogin = defaultLogin; }
	QString defaultLogin() { return m_defaultLogin; }

	void setProxyUser(const QString & proxyUser) { m_proxyUser = proxyUser; }
	QString ProxyUser() { return m_proxyUser; }

	void setProxyPasswd(const QString & proxyPasswd) { m_proxyUser = proxyPasswd; }
	QString ProxyPasswd() { return m_proxyPasswd; }

	void setListBouchots(const QList<QQBouchot *>&);
	QList<QQBouchot *> listBouchots() { return m_listBouchots; }
	QList<QQBouchot *> listBouchots(QString group);
	bool hasBouchot(QString bouchotName) { return bouchot(bouchotName) != NULL; }
	QQBouchot * bouchot(QString bouchotName);
	void addBouchot(QQBouchot * bouchot) { m_listBouchots.append(bouchot); }
	void addBouchots(const QList<QQBouchot *>& newBouchots) { m_listBouchots << newBouchots; }
	void removeBouchot(QQBouchot * bouchot);
	void removeBouchot(const QString bouchotName);
	void startBouchots();

	void startBouchot(QString &);
	void stopBouchots();
	void stopBouchot(QString &);

	QList<QString> listTabs();

	void setPalmiMinimized(bool palmiMini) { m_palmiMini = palmiMini; }
	bool palmiMinimized() { return m_palmiMini; }

public slots:
	bool readSettings();
	bool saveSettings();
	void proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);

private:
	uint m_maxHistoryLength;
	QString m_defaultUA;
	QString m_defaultLogin;

	TotozMode m_totozMode;

	bool m_palmiMini;

	QString m_proxyUser;
	QString m_proxyPasswd;

	QList<QQBouchot *> m_listBouchots;

	QMutex m_proxyPopupMutex;
};

#endif // QQSETTINGS_H
