#include "qqtmrequester.h"

#include "core/qqsettings.h"
#include "ui/totozmanager/qqtmxmlparser.h"

#include <QtDebug>
#include <QNetworkReply>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

QQTMRequester::QQTMRequester(QObject * parent) :
	QQNetworkAccessor(parent)
{
	m_currKey.clear();
	m_netReply = nullptr;

	m_xmlParser = new QQTMXmlParser(this);
	connect(m_xmlParser, SIGNAL(finished()), this, SLOT(parsingFinished()));
}

QQTMRequester::~QQTMRequester()
{
	if(m_netReply != nullptr)
		m_netReply->abort();
	delete m_xmlParser;
}

void QQTMRequester::requestFinishedSlot(QNetworkReply * reply)
{
	if(reply->error() == QNetworkReply::NoError)
	{
		QByteArray data = reply->readAll();

		QXmlSimpleReader xmlReader;
		QXmlInputSource xmlSource;

		xmlSource.setData(data);
		xmlReader.setContentHandler(m_xmlParser);
		xmlReader.setErrorHandler(m_xmlParser);
		xmlReader.parse(xmlSource);
	}
	else
	{
		qDebug() << Q_FUNC_INFO << "reply->error() =" << reply->error() << ", " << reply->errorString();
		emit requestFinished();
	}

	m_netReply = nullptr;
	reply->deleteLater();
}

void QQTMRequester::searchTotoz(const QString & key)
{
	cancel();
	m_totozes.clear();
	m_currKey = key;
	searchTotoz(key, 0);
}

void QQTMRequester::cancel()
{
	if(m_netReply != nullptr)
		m_netReply->abort();
}

void QQTMRequester::searchTotoz(const QString & key, int offset)
{
	QQSettings settings;

	QString searchPattern = settings.value(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN,
										   DEFAULT_TOTOZ_SERVER_QUERY_PATTERN).toString();
	searchPattern.replace("%t", key.toLatin1().toPercentEncoding());
	searchPattern.replace("%o", QString::number(offset));


	QString urlString = settings.value(SETTINGS_TOTOZ_SERVER_URL, DEFAULT_TOTOZ_SERVER_URL).toString();
	urlString.append("/").append(searchPattern);

	QUrl url(urlString);

	QNetworkRequest request(url);
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						 QNetworkRequest::PreferCache);

	m_netReply = httpGet(request);
}

void QQTMRequester::parsingFinished()
{
	m_totozes.append(m_xmlParser->totozes());

	if(m_xmlParser->numResults() > 0 &&
			m_totozes.size() < m_xmlParser->numResults())
		searchTotoz(m_currKey, m_totozes.size());
	else
		emit requestFinished();
}
