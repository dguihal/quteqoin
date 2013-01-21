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

#include <QTextDocument>

//
QQXmlParser::QQXmlParser()
	: QXmlDefaultHandler()
{
	m_currentPost.reset();
	this->m_lastId = 0;
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
	if((m_elementNames.top() == "info") ||
		 (m_elementNames.top() == "message") ||
		 (m_elementNames.top() == "login"))
	{
		QString tmp = ch;
		if (m_typeSlip == QQBouchot::SlipTagsRaw)
			tmp.replace("&","&amp;").replace(">","&gt;").replace("<","&lt;");
		m_tmpString.append(tmp);
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
/*
//
bool QQXmlParser::startDocument ()
{
	qDebug() << "QQXmlParser::startDocument" << endl;
	return true;
}
//
bool QQXmlParser::endDocument ()
{
	qDebug() << "QQXmlParser::endDocument" << endl;
	return true;
}
*/
//
bool QQXmlParser::startElement(const QString &namespaceURI, const QString &localName,
								const QString &qName, const QXmlAttributes &atts)
{
	// Pour éviter le warning
	(void) namespaceURI;
	(void) qName;

	//qDebug() << "QQXmlParser::startElement" << namespaceURI << " ;; " << localName << " ;; " << qName << " ;; " << atts.count() << endl;
	if (localName == "post")
	{
		int i;
		for (i = 0; i < atts.count(); i++)
		{
			//qDebug() << "QQXmlParser::startElement, Attributes : " << atts.localName(i) << " = " << atts.value(i) << endl;
			if(atts.localName(i) == "id")
			{
				if(atts.value(i).toInt() <= this->m_lastId)
				{
					m_errorString = QString::fromUtf8("Id déjà connu, arrêt du parser xml");
					return false;
				}

				m_currentPost.setId(atts.value(i));
			}
			else if(atts.localName(i) == "time") m_currentPost.setNorloge(atts.value(i));
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
			m_currentPost.setUA(Qt::escape(m_tmpString));
		else if (localName == "message")
			m_currentPost.setMessage(m_tmpString);
		else if (localName == "login")
			m_currentPost.setLogin(Qt::escape(m_tmpString));
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
