#include "qqnetworkaccessor.h"
#include "ui/qqproxyauthdialog.h"

#include <QtDebug>
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
	QDate date(fields[2].toInt(), name_to_month(fields[1].toAscii()), fields[0].toInt());
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

QNetworkReply * QQNetworkAccessor::httpGet(const QNetworkRequest & request)
{
	return m_qnam->get(request);
}

QNetworkReply * QQNetworkAccessor::httpPost(const QNetworkRequest & request, const QByteArray &postData)
{
	return m_qnam->post(request, postData);
}


