#ifndef QQNETWORKACCESSOR_H
#define QQNETWORKACCESSOR_H

#include <QDateTime>
#include <QMap>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QTimer>

class QAbstractNetworkCache;

class QQNetworkAccessor : public QObject
{
	Q_OBJECT

public:
	QQNetworkAccessor(QObject *parent);

	static void updateProxySettings();

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply) = 0;

protected:
	static int name_to_month(QByteArray month_str);
	static QDateTime parseRC822(const QString& string);

protected:
	QNetworkReply *httpGet(const QNetworkRequest &request);
	QNetworkReply *httpHead(const QNetworkRequest &request);
	QNetworkReply *httpPost(const QNetworkRequest &request,
							 const QByteArray &postData);
	QNetworkReply *httpPost(const QNetworkRequest &request,
							 QHttpMultiPart *multiPart);
	QNetworkReply *httpPut(const QNetworkRequest &request,
						   QIODevice *data);

	void clearCookiesForUrl(const QUrl &url);
	void clearNetworkBackend();
	bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

	void setNetCacheManager(QAbstractNetworkCache *networkCache) { m_qnam->setCache(networkCache); }

protected slots:
	static void onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
	void onRemoveTimer(QObject *obj);
	void onRequestTimeout();

private:
	QPointer<QNetworkAccessManager> m_qnam;
	QMap<QNetworkReply *, QTimer *> m_replyTimers;

	// Gestion du proxy
	static QMutex m_proxyPopupMutex;
	static QString m_proxyUser;
	static QString m_proxyPasswd;
};

#endif // QQNETWORKACCESSOR_H
