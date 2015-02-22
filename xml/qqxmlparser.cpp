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

#include "qqxmlparser.h"

#include <QtDebug>
#include <QTextDocument>

//
QQXmlParser::QQXmlParser()
	: QXmlDefaultHandler()
{
	m_currentPost.reset();
	m_typeSlip = QQBouchot::SlipTagsEncoded;
	m_lastId = 0;
	m_isInCData = false;
}

//
QQXmlParser::~QQXmlParser()
{
}

QString QQXmlParser::errorString () const
{
	return m_errorString;
}

// Methodes héritées de QXmlErrorHandler
/*
bool QQXmlParser::error(const QXmlParseException & exception)
{
	qDebug() << "QQXmlParser::error ;; " << exception.message() << endl;
	return true;
}
//
bool QQXmlParser::fatalError(const QXmlParseException & exception)
{
	qDebug() << "QQXmlParser::fatalError ;; " << exception.message() << endl;
	return true;
}
//
bool QQXmlParser::warning(const QXmlParseException & exception)
{
	qDebug() << "QQXmlParser::warning ;; " << exception.message() << endl;
	return true;
}
*/

// Methodes héritées de QXmlContentHandler

//
bool QQXmlParser::characters(const QString & ch)
{
	//	qDebug() << "QQXmlParser::characters ;; " << ch << endl;
	if(m_isInCData)
	{
		m_tmpString.append(ch);
	}
	else if((m_elementNames.top() == "info") ||
	   (m_elementNames.top() == "message") ||
	   (m_elementNames.top() == "login"))
	{
		if (m_typeSlip == QQBouchot::SlipTagsRaw)
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			m_tmpString.append(ch.toHtmlEscaped());
#else
			m_tmpString.append(Qt::escape(ch));
#endif
		else
			m_tmpString.append(ch);
	}
	return true;
}
/*
//
bool QQXmlParser::ignorableWhitespace(const QString & ch)
{
	qDebug() << "QQXmlParser::ignorableWhitespace ;; " << ch << endl;
	return true;
}
//
bool QQXmlParser::processingInstruction(const QString & target, const QString & data)
{
	qDebug() << "QQXmlParser::processingInstruction ;; " << target << " ;; " << data << endl;
	return true;
}
//
void QQXmlParser::setDocumentLocator(QXmlLocator * locator)
{
	qDebug() << "QQXmlParser::setDocumentLocator ;; " << locator->lineNumber() << endl;
}
*/
//
bool QQXmlParser::skippedEntity(const QString & name)
{
	//	qDebug() << "QQXmlParser::skippedEntity ;; " << name << endl;
	if((m_elementNames.top() == "info") ||
	   (m_elementNames.top() == "message") ||
	   (m_elementNames.top() == "login"))
	{
		m_tmpString += "&";
		m_tmpString += name;
		m_tmpString += ";";
	}
	return true;
}

//
bool QQXmlParser::startDocument ()
{
	m_elementNames.clear();
	m_tmpString.clear();
	m_errorString.clear();
	m_currentPost.reset();
	m_maxId = 0;
	return true;
}
//
bool QQXmlParser::endDocument ()
{
	m_lastId = 0;
	emit finished();
	return true;
}

//
bool QQXmlParser::startElement(const QString &namespaceURI, const QString &localName,
							   const QString &qName, const QXmlAttributes &atts)
{
	// Pour éviter le warning
	(void) namespaceURI;
	(void) qName;

	if (localName == "post")
	{
		int i;
		for (i = 0; i < atts.count(); i++)
		{
			if(atts.localName(i) == "id")
			{
				qlonglong idValue = atts.value(i).toLongLong();
				m_maxId = idValue > m_maxId ? idValue : m_maxId;
				if(idValue <= this->m_lastId)
				{
					m_errorString = QString::fromUtf8("Id déjà connu, arrêt du parser xml");
					emit finished();
					return false;
				}

				m_currentPost.setId(atts.value(i));
			}
			else if(atts.localName(i) == "time")
				m_currentPost.setNorloge(atts.value(i));
		}
	}

	if (m_elementNames.size() > 0 &&
		((m_elementNames.top() == "info") ||
		 (m_elementNames.top() == "message") ||
		 (m_elementNames.top() == "login"))
		)
	{
		m_tmpString.append("<").append(localName);
		for(int i = 0; i < atts.length(); i++)
			m_tmpString.append(" ").append(atts.qName(i)).append("=\"").append(atts.value(i)).append("\"");
		m_tmpString.append(">");
	}
	else
		m_elementNames.push(localName);
	return true;
}
//
bool QQXmlParser::endElement(const QString &namespaceURI, const QString &localName,
							 const QString &qName)
{
	// Pour éviter le warning
	(void) namespaceURI;
	(void) qName;

	//qDebug() << "QQXmlParser::endElement" << namespaceURI << " ;; " << localName << " ;; " << qName << endl;
	if (localName != m_elementNames.top())
	{
		m_tmpString.append("</").append(localName).append(">");
		return true;
	}

	if (localName == "post")
	{
		emit newPostReady(m_currentPost);
		m_currentPost.reset();
	}
	else
	{
		if (localName == "info")
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			m_currentPost.setUA(m_tmpString.trimmed().toHtmlEscaped());
#else
			m_currentPost.setUA(Qt::escape(m_tmpString.trimmed()));
#endif
		else if (localName == "message")
			m_currentPost.setMessage(m_tmpString.trimmed());
		else if (localName == "login")
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			m_currentPost.setLogin(m_tmpString.trimmed().toHtmlEscaped());
#else
			m_currentPost.setLogin(Qt::escape(m_tmpString.trimmed()));
#endif
		m_tmpString.clear();
	}
	m_elementNames.pop();
	return true;
}
/*
//
bool QQXmlParser::startPrefixMapping(const QString & prefix, const QString & uri)
{
	qDebug() << "QQXmlParser::startPrefixMapping" << prefix << " ;; " << uri << endl;
	return true;
}
//
bool QQXmlParser::endPrefixMapping(const QString & prefix)
{
	qDebug() << "QQXmlParser::endPrefixMapping" << prefix << endl;
	return true;
}
//

}
*/
