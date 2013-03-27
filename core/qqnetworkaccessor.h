#ifndef QQNETWORKACCESSOR_H
#define QQNETWORKACCESSOR_H

#include <QObject>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QQNetworkAccessor : public QObject
{
	Q_OBJECT

public:
	QQNetworkAccessor(QObject * parent, QObject * proxyAuthMgr = 0);

public slots:
	virtual void requestFinishedSlot(QNetworkReply * reply) = 0;

protected:
	QNetworkReply * httpGet(const QNetworkRequest & request);
	QNetworkReply * httpPost(const QNetworkRequest & request,
													 const QByteArray & postData);

private:
	QNetworkAccessManager * createQNAM();

	QPointer<QNetworkAccessManager> m_qnam;
};

#endif // QQNETWORKACCESSOR_H
