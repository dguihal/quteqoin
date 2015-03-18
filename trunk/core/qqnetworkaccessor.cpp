#include "qqnetworkaccessor.h"
#include "ui/qqproxyauthdialog.h"

#include <core/qqsettings.h>

#include <QtDebug>
#include <QApplication>
#include <QAuthenticator>
#include <QNetworkProxyFactory>
#include <QTimer>

#define NETWORK_REQUEST_TIMEOUT_MS 60000

QQNetworkAccessor::QQNetworkAccessor(QObject *parent) :
	QObject(parent)
{
	m_qnam = new QNetworkAccessManager(this);

	connect(m_qnam, SIGNAL(finished(QNetworkReply *)),
			this, SLOT(requestFinishedSlot(QNetworkReply *)));

	connect(m_qnam, SIGNAL(proxyAuthenticationRequired(QNetworkProxy, QAuthenticator *)),
			this, SLOT(onProxyAuthenticationRequired(QNetworkProxy, QAuthenticator *)));
}

void QQNetworkAccessor::updateProxySettings()
{
	QQSettings settings;

	QString nMode = settings.value(SETTINGS_NETWORK_MODE, DEFAULT_NETWORK_MODE).toString();
	if(! SETTINGS_NETWORK_MODES.contains(nMode))
	{
		nMode = DEFAULT_NETWORK_MODE;
		settings.remove(SETTINGS_NETWORK_MODE);
	}

	if(nMode == SETTINGS_NETWORK_MODE_SYSTEM)
	{
		QNetworkProxyFactory::setUseSystemConfiguration(true);
	}
	else
	{
		QNetworkProxyFactory::setUseSystemConfiguration(false);
		QNetworkProxy p;
		if(nMode == SETTINGS_NETWORK_MODE_DIRECT)
		{
			p.setType(QNetworkProxy::NoProxy);
		}
		else //if(Mode == SETTINGS_NETWORK_MODE_MANUAL)
		{
			QString host = settings.value(SETTINGS_NETWORK_PROXY_HOST, DEFAULT_NETWORK_PROXY_HOST).toString();
			quint16 port = settings.value(SETTINGS_NETWORK_PROXY_PORT, DEFAULT_NETWORK_PROXY_PORT).toUInt();
			bool isHttp = settings.value(SETTINGS_NETWORK_PROXY_IS_HTTP, DEFAULT_NETWORK_PROXY_IS_HTTP).toUInt();

			p.setType(isHttp ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy);
			p.setHostName(host);
			p.setPort(port);

		}
		QNetworkProxy::setApplicationProxy(p);
	}
}

// Fast month string to int conversion. This code
// assumes that the Month name is correct and that
// the string is at least three chars long.
int QQNetworkAccessor::name_to_month(QByteArray month_str)
{
	switch (month_str[0]) {
	case 'J':
		switch (month_str[1]) {
		case 'a':
			return 1;
			break;
		case 'u':
			switch (month_str[2] ) {
			case 'n':
				return 6;
				break;
			case 'l':
				return 7;
				break;
			}
		}
		break;
	case 'F':
		return 2;
		break;
	case 'M':
		switch (month_str[2] ) {
		case 'r':
			return 3;
			break;
		case 'y':
			return 5;
			break;
		}
		break;
	case 'A':
		switch (month_str[1]) {
		case 'p':
			return 4;
			break;
		case 'u':
			return 8;
			break;
		}
		break;
	case 'O':
		return 10;
		break;
	case 'S':
		return 9;
		break;
	case 'N':
		return 11;
		break;
	case 'D':
		return 12;
		break;
	}
	return 0;
}

// Parse : "Tue, 09 Apr 2013 11:22:22 +0200"
QDateTime QQNetworkAccessor::parseRC822(QString string)
{
	int pos = string.indexOf(',') + 1;
	QString minString = string.right(string.length() - pos);
	QStringList fields = minString.split(" ", QString::SkipEmptyParts);
	QDate date(fields[2].toInt(), name_to_month(fields[1].toLatin1()), fields[0].toInt());
	QTime time = QTime::fromString(fields[3], "hh:mm:ss");
	if(fields[4].startsWith('+'))
	{
		QString offset = fields[4].right(4);
		time.addSecs(offset.left(2).toInt() * 3600 + offset.right(2).toInt() * 60);
	}
	else if(fields[4].startsWith('-'))
	{
		QString offset = fields[4].right(4);
		time.addSecs(0 - (offset.left(2).toInt() * 3600 + offset.right(2).toInt() * 60));
	}
	QDateTime datetime(date, time, Qt::UTC);
	return datetime;
}

QNetworkReply * QQNetworkAccessor::httpGet(const QNetworkRequest &request)
{
	QTimer *replyTimer = new QTimer(this);
	replyTimer->setSingleShot(true);

	QNetworkReply *reply = m_qnam->get(request);
	connect(replyTimer, SIGNAL(timeout()), this, SLOT(onRequestTimeout()));
	connect(reply, SIGNAL(finished()), replyTimer, SLOT(stop()));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)), replyTimer, SLOT(start()));
	connect(reply, SIGNAL(destroyed(QObject*)), this, SLOT(onRemoveTimer(QObject*)));
	replyTimer->start(NETWORK_REQUEST_TIMEOUT_MS);
	m_replyTimers.insert(reply, replyTimer);

	return reply;
}

QNetworkReply * QQNetworkAccessor::httpHead(const QNetworkRequest &request)
{
	return m_qnam->head(request);
}

QNetworkReply * QQNetworkAccessor::httpPost(const QNetworkRequest &request, const QByteArray &postData)
{
	return m_qnam->post(request, postData);
}

QNetworkReply * QQNetworkAccessor::httpPost(const QNetworkRequest &request, QHttpMultiPart *multiPart)
{
	return m_qnam->post(request, multiPart);
}

// Gestion du proxy
QMutex QQNetworkAccessor::m_proxyPopupMutex;
QString QQNetworkAccessor::m_proxyUser;
QString QQNetworkAccessor::m_proxyPasswd;

void QQNetworkAccessor::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
	Q_UNUSED(proxy)

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
			QQProxyAuthDialog* proxyDialog = new QQProxyAuthDialog();
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

void QQNetworkAccessor::onRemoveTimer(QObject *obj)
{
	QTimer *timer = m_replyTimers.take((QNetworkReply *) obj);
	if(timer != NULL)
		delete timer;
}

void QQNetworkAccessor::onRequestTimeout()
{
	foreach(QNetworkReply *reply, m_replyTimers.keys())
	{
		QTimer *timer = m_replyTimers.value(reply);
		if(! timer->isActive())
		{
			reply->abort();
			qDebug() << Q_FUNC_INFO << reply->url() << "aborted";
			m_replyTimers.remove(reply);
			reply->deleteLater();
			timer->deleteLater();
		}
	}
}


