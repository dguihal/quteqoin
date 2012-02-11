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

//
class QQXmlParser : public QObject, public QXmlDefaultHandler
{
	Q_OBJECT

	public:
		QQXmlParser();
		virtual ~QQXmlParser();

		QString errorString () const;
		void setLastId(int);

		// Methodes héritées de QXmlErrorHandler
//		bool error ( const QXmlParseException & );
//		bool fatalError ( const QXmlParseException & );
//		bool warning ( const QXmlParseException & );

		// Methodes héritées de QXmlContentHandler
		bool characters ( const QString & );
//		bool ignorableWhitespace ( const QString & );
//		bool processingInstruction ( const QString &, const QString & );
//		void setDocumentLocator ( QXmlLocator * );
		bool skippedEntity ( const QString & );
//		bool startDocument ();
//		bool endDocument ();
		bool startElement ( const QString &, const QString &, const QString &, const QXmlAttributes & );
		bool endElement ( const QString &, const QString &, const QString & );
//		bool startPrefixMapping ( const QString &, const QString & );
//		bool endPrefixMapping ( const QString & );

	signals:
		void newPostReady ( QQPost &newPost );
		
	private:
		QStack<QString>	m_elementNames;
		QString			m_tmpString;
		QString			m_errorString;
		QQPost			m_currentPost;

		int m_lastId;
};
#endif
