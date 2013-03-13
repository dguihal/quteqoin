#include "qqnetworkaccessor.h"
#include "ui/qqproxyauthdialog.h"

#include <QApplication>
#include <QAuthenticator>
#include <QNetworkProxyFactory>

QQNetworkAccessor::QQNetworkAccessor(QObject * parent) :
	QObject(parent)
{
	m_qnam = createQNAM();

	connect(m_qnam, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(requestFinishedSlot(QNetworkReply*)));

	connect(m_qnam, SIGNAL(proxyAuthenticationRequired(QNetworkProxy, QAuthenticator*)),
			this, SLOT(proxyAuthenticationRequired(QNetworkProxy, QAuthenticator*)));
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

// Gestion du proxy
QMutex QQNetworkAccessor::m_proxyPopupMutex;
QString QQNetworkAccessor::m_proxyUser;
QString QQNetworkAccessor::m_proxyPasswd;

void QQNetworkAccessor::proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
{
	Q_UNUSED(proxy)

	qDebug() << "QQSettings::proxyAuthenticationRequired";
	//Premier echec
	if(QQNetworkAccessor::m_proxyUser.size() != 0 &&
	   authenticator->user() != QQNetworkAccessor::m_proxyUser)
	{
		authenticator->setUser(QQNetworkAccessor::m_proxyUser);
		authenticator->setPassword(QQNetworkAccessor::m_proxyPasswd);
	}
	else // Echec ulterieur, ou pas de conf proxy, on redemande
	{
		if(QQNetworkAccessor::m_proxyPopupMutex.tryLock())
		{
			QQProxyAuthDialog * proxyDialog = new QQProxyAuthDialog();
			proxyDialog->setLogin(QQNetworkAccessor::m_proxyUser);
			proxyDialog->setPasswd(QQNetworkAccessor::m_proxyPasswd);
			if(proxyDialog->exec() == QDialog::Accepted)
			{
				QQNetworkAccessor::m_proxyUser = proxyDialog->login();
				QQNetworkAccessor::m_proxyPasswd = proxyDialog->passwd();

				authenticator->setUser(QQNetworkAccessor::m_proxyUser);
				authenticator->setPassword(QQNetworkAccessor::m_proxyPasswd);
			}
			QQNetworkAccessor::m_proxyPopupMutex.unlock();
			//startBouchots();
		}
		else
		{
			QApplication::processEvents();
		}
	}
}
