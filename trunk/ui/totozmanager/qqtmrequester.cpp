#include "qqtmrequester.h"

#include "core/qqsettings.h"
#include "xml/totozmanager/qqtmxmlparser.h"

#include <QDebug>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#define TOTOZ_URL "http://totoz.sauf.ca"
#define QUERY_URL "search.xml?terms="


QQTMRequester::QQTMRequester(QQSettings * settings) :
	QQNetworkAccessor(settings)
{
	m_settings = settings;
	m_currKey.clear();

	m_xmlParser = new QQTMXmlParser(this);
	connect(m_xmlParser, SIGNAL(finished()), this, SLOT(parsingFinished()));
}

QQTMRequester::~QQTMRequester()
{
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

	reply->deleteLater();
}

void QQTMRequester::searchTotoz(const QString & key)
{
	m_totozes.clear();
	m_currKey = key;
	searchTotoz(key, 0);
}

void QQTMRequester::searchTotoz(const QString & key, int offset)
{
	QString urlString = QString::fromAscii(TOTOZ_URL)
						.append("/")
						.append(QUERY_URL)
						.append(key.toAscii().toPercentEncoding())
						.append("&offset=")
						.append(QString::number(offset));

	QUrl url(urlString);

	QNetworkRequest request(url);
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						 QNetworkRequest::PreferCache);

	httpGet(request);
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
