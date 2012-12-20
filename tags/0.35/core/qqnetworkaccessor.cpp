#include "qqnetworkaccessor.h"

#include <QNetworkProxyFactory>

QQNetworkAccessor::QQNetworkAccessor( QObject * parent, QObject * proxyAuthMgr ) :
	QObject(parent)
{
	m_qnam = createQNAM();

	if(proxyAuthMgr == 0)
		proxyAuthMgr = parent;

	connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(requestFinishedSlot(QNetworkReply*)));

	connect(m_qnam, SIGNAL(proxyAuthenticationRequired(QNetworkProxy, QAuthenticator*)),
			proxyAuthMgr, SLOT(proxyAuthenticationRequired(QNetworkProxy, QAuthenticator*)));
}

QNetworkAccessManager * QQNetworkAccessor::createQNAM()
{
	QNetworkAccessManager * qnam = new QNetworkAccessManager(this);

	qnam->proxyFactory()->setUseSystemConfiguration(true);

	return qnam;
}

QNetworkReply * QQNetworkAccessor::httpGet(const QNetworkRequest & request)
{
	return m_qnam->get(request);
}

QNetworkReply * QQNetworkAccessor::httpPost(const QNetworkRequest & request, const QByteArray &postData)
{
	return m_qnam->post(request, postData);
}
