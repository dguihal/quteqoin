/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __QQXMLPARSER_H
#define __QQXMLPARSER_H
//

#include <QObject>
#include <QStack>
#include <QString>
#include <QXmlAttributes>
#include <QXmlDefaultHandler>

#include "core/qqpost.h"
#include "core/qqbouchot.h"

//
class QQXmlParser : public QObject, public QXmlDefaultHandler
{
	Q_OBJECT

public:
	QQXmlParser();
	virtual ~QQXmlParser();

	QString errorString () const;
	void setLastId(int lastId) { this->m_lastId = lastId; }
	void setTypeSlip(QQBouchot::TypeSlip typeSlip) { this->m_typeSlip = typeSlip; }

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
	bool skippedEntity(const QString & name);

	// Methodes héritées de QXmlLexicalHandler
	bool startCDATA() { m_isInCData = true; return true; }
	//bool startDTD();
	bool endDATA() { m_isInCData = false; return true; }
	//bool endDTD();

	int maxId() { return m_maxId; }

signals:
	void newPostReady(QQPost & newPost);
	void finished();

private:
	QStack<QString>	m_elementNames;
	QString			m_tmpString;
	QString			m_errorString;
	QQPost			m_currentPost;

	int m_lastId;
	int m_maxId;
	QQBouchot::TypeSlip m_typeSlip;

	bool m_isInCData;
};
#endif
