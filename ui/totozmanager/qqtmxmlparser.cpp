#include "qqtmxmlparser.h"

#include <QtDebug>

QQTMXmlParser::QQTMXmlParser(QObject *parent) :
	QObject(parent)
{
	m_elementNames.clear();
	m_listTotozes.clear();
	m_numResults = 0;
}

//
QQTMXmlParser::~QQTMXmlParser()
{
	m_elementNames.clear();
	m_listTotozes.clear();
}

bool QQTMXmlParser::characters(const QString & ch)
{
	if(m_elementNames.last() == "name")
	{
		m_listTotozes.append(ch);
	}

	return true;
}

bool QQTMXmlParser::startDocument()
{
	m_listTotozes.clear();
	m_numResults = 0;
	return true;
}

bool QQTMXmlParser::endDocument()
{
	emit finished();
	return true;
}

bool QQTMXmlParser::startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts)
{
	(void) namespaceURI;
	(void) qName;
	(void) atts;

	m_elementNames.push(localName);

	if(localName == "totozes")
	{
		QString resCount = atts.value("results");
		bool ok = false;
		m_numResults = resCount.toInt(&ok, 10);
		if(!ok)
			m_numResults = 0;
	}

	return true;
}

bool QQTMXmlParser::endElement(const QString & namespaceURI, const QString & localName, const QString & qName)
{
	(void) namespaceURI;
	(void) qName;

	QString storedName = m_elementNames.pop();

	if(storedName != localName)
	{
		qWarning() << "QQTMXmlParser::endElement : malformed xml," << storedName << "expected," << localName << "found";
		return false;
	}

	return true;
}
