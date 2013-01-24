#include "qqtmrequester.h"

#include "core/qqsettings.h"
#include "xml/totozmanager/qqtmxmlparser.h"

#include <QtDebug>
#include <QNetworkReply>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#define TOTOZ_URL "http://totoz.sauf.ca"


QQTMRequester::QQTMRequester(QObject * parent, QQSettings * settings) :
	QQNetworkAccessor(parent, settings)
{
	m_settings = settings;
	m_currKey.clear();
	m_netReply = NULL;

	m_xmlParser = new QQTMXmlParser(this);
	connect(m_xmlParser, SIGNAL(finished()), this, SLOT(parsingFinished()));
}

QQTMRequester::~QQTMRequester()
{
	if(m_netReply != NULL)
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
		qDebug() << "QQTMRequester::requestFinishedSlot : reply->error()=" << reply->error() << ", " << reply->errorString();
		emit requestFinished();
	}

	m_netReply = NULL;
	reply->deleteLater();
}

void QQTMRequester::searchTotoz(const QString & key)
{
	m_totozes.clear();
	m_currKey = key;
	searchTotoz(key, 0);
}

void QQTMRequester::cancel()
{
	if(m_netReply != NULL)
		m_netReply->abort();
}

void QQTMRequester::searchTotoz(const QString & key, int offset)
{
	QString searchPattern = m_settings->totozQueryPattern();
	searchPattern.replace("%t", key.toAscii().toPercentEncoding());
	searchPattern.replace("%o", QString::number(offset));

	QString urlString = QString::fromAscii(TOTOZ_URL)
						.append("/")
						.append(searchPattern);

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
