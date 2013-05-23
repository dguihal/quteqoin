#ifndef QQNETWORKACCESSOR_H
#define QQNETWORKACCESSOR_H

#include <QDateTime>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>

class QQNetworkAccessor : public QObject
{
	Q_OBJECT

public:
	QQNetworkAccessor(QObject * parent);

public slots:
	virtual void requestFinishedSlot(QNetworkReply * reply) = 0;

protected:
	int name_to_month(QByteArray month_str);
	QDateTime parseRC822(QString string);

protected slots:
	void proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);

protected:
	QNetworkReply * httpGet(const QNetworkRequest & request);
	QNetworkReply * httpHead(const QNetworkRequest & request);
	QNetworkReply * httpPost(const QNetworkRequest & request,
													 const QByteArray & postData);

private:
	QNetworkAccessManager * createQNAM();

	QPointer<QNetworkAccessManager> m_qnam;

	// Gestion du proxy
	static QMutex m_proxyPopupMutex;
	static QString m_proxyUser;
	static QString m_proxyPasswd;
};

#endif // QQNETWORKACCESSOR_H
