#ifndef QQTMXMLPARSER_H
#define QQTMXMLPARSER_H

#include <QList>
#include <QObject>
#include <QStack>
#include <QXmlDefaultHandler>

class QQTMXmlParser : public QObject, public QXmlDefaultHandler
{
	Q_OBJECT
public:
	explicit QQTMXmlParser(QObject *parent = 0);
	virtual ~QQTMXmlParser();

	// Methodes héritées de QXmlErrorHandler
	// bool warning(const QXmlParseException & exception);
	// bool error(const QXmlParseException & exception);
	// bool fatalError(const QXmlParseException & exception);

	// Methodes héritées de QXmlContentHandler
	// void setDocumentLocator(QXmlLocator * locator);
	bool startDocument();
	bool endDocument();
	// bool startPrefixMapping(const QString & prefix, const QString & uri);
	// bool endPrefixMapping(const QString & prefix);
	bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
	bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);
	bool characters(const QString & ch);
	// bool ignorableWhitespace(const QString & ch);
	// bool processingInstruction(const QString & target, const QString & data);
	// bool skippedEntity(const QString & name);

	int numResults() { return m_numResults; }
	QList<QString> totozes() { return m_listTotozes; }

signals:
	void finished();

public slots:

private:
	QStack<QString>	m_elementNames;
	QList<QString> m_listTotozes;

	int m_numResults;
};

#endif // QQTMXMLPARSER_H
