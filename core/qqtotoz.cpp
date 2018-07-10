#include "qqtotoz.h"
#include "core/qqsettingsparams.h"

#include <QtDebug>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

QQTotoz::QQTotoz() {}

QQTotoz::QQTotoz(const QString & totozId)
{
	if(totozId.startsWith("[:") && totozId.endsWith("]"))
		m_id = totozId.mid(2, totozId.length() - 3);
	else
		m_id = totozId;
	load();
}

QQTotoz::QQTotoz(const QQTotoz & totoz)
{
	m_id = totoz.m_id;
	load();
}

QQTotoz::~QQTotoz()
{
	if(m_wasmodfied)
		save();
}

bool QQTotoz::cacheExists()
{
	QFile file(QQTotoz::getPath(m_id));
	if(!file.exists(QQTotoz::getPath(m_id)))
		return false;

	//Invalidation systematique au bout de 60j
	QFileInfo info(file);
	QDateTime dateFile = info.created();
	if(dateFile.daysTo(QDateTime::currentDateTime()) > MAX_CACHE_AGE_DAYS)
	{
		file.remove();
		return false;
	}

	return true;
}

bool QQTotoz::isCacheExpired()
{
	bool cacheExpired = false;
	QDateTime now = QDateTime::currentDateTime();

	cacheExpired |= (! cacheExists());
	cacheExpired |= (! m_cacheExpireDate.isValid());
	cacheExpired |= (! (m_cacheExpireDate.secsTo(now) <= 0));

	return cacheExpired;
}

QString QQTotoz::getPath(QString id)
{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QDir dirCache(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
#else
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
#endif

	if(! dirCache.exists())
		dirCache.mkpath(dirCache.path());

	QString totozIdB64 = QString(id.toLower().toLatin1().toBase64()).left(255);
	totozIdB64.replace('/', '_');
	return dirCache.filePath(totozIdB64);
}

void QQTotoz::load()
{
	//qDebug() << "QQTotoz::load id: " << m_id << ", path:" << getPath(m_id);
	QFile file(getPath(m_id));
	if(file.open(QIODevice::ReadOnly))
	{
		QByteArray compressedData = file.readAll();

		m_wasmodfied = false;
		if(compressedData.size() == 0)
			return;

		QByteArray data = qUncompress(compressedData);
		QBuffer buffer(&data);
		buffer.open(QIODevice::ReadOnly);

		QXmlStreamReader xml;
		xml.setDevice(&buffer);

		QString currentElement;
		while (!xml.atEnd())
		{
			xml.readNext();
			switch (xml.tokenType())
			{
			case QXmlStreamReader::StartElement:
				currentElement = xml.name().toString();

				if(currentElement == "nsfw")
				{
					QString data = xml.readElementText();
					m_isNSFW = (data != "0");
				}
				else if(currentElement == "tags")
				{
					m_tags.clear();
				}
				else if(currentElement == "tag")
				{
					m_tags.append(xml.readElementText());
				}
				else if(currentElement == "expiredate")
				{
					m_cacheExpireDate = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
				}
				else if(currentElement == "totozdata"
				        || currentElement == "totozdatacontenttype"
				        || currentElement == "totoz"
				        || currentElement == "imgdata" ) // TODO: legacy to be removed one day
				{
				}
				else
					qDebug() << Q_FUNC_INFO << "unknown element :" << currentElement;

				break;
			case QXmlStreamReader::Characters:
				if(xml.isCDATA())
				{
					auto content = xml.text().toLatin1();
					if(currentElement == "totozdata")
						m_totozData = QByteArray::fromBase64(content);
					else if(currentElement == "totozdatacontenttype")
						m_totozDataContentType = content;
				}
				else
					qDebug() << Q_FUNC_INFO << "unknown Characters :" << xml.text();
				break;
			case QXmlStreamReader::NoToken:
			case QXmlStreamReader::Invalid:
			case QXmlStreamReader::StartDocument:
			case QXmlStreamReader::EndDocument:
			case QXmlStreamReader::EndElement:
			case QXmlStreamReader::Comment:
			case QXmlStreamReader::DTD:
			case QXmlStreamReader::EntityReference:
			case QXmlStreamReader::ProcessingInstruction:
				//Nothing
				break;

			}
		}

		if (xml.error() != QXmlStreamReader::NoError)
			m_totozData.clear();

		buffer.close();
		file.close();
	}

	if(m_totozData.size() == 0)
		file.remove();
}

void QQTotoz::save()
{
	//qDebug() << "QQTotoz::save id: " << m_id << ", path:" << getPath(m_id);
	QFile file(getPath(m_id));
	file.open(QIODevice::WriteOnly);

	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);

	QXmlStreamWriter xml;
	xml.setDevice(&buffer);

	xml.writeDTD("<!DOCTYPE totoz>");
	xml.writeStartElement("totoz");
	xml.writeAttribute("version", "1.0");

	xml.writeStartElement("nsfw");
	m_isNSFW ? xml.writeCharacters("1") : xml.writeCharacters("0");
	xml.writeEndElement(); //nsfw

	xml.writeStartElement("tags");
	for(int i = 0; i < m_tags.size(); i++)
	{
		xml.writeStartElement("tag");
		xml.writeCharacters(m_tags.at(i));
		xml.writeEndElement(); //tag
	}
	xml.writeEndElement(); //tags

	xml.writeStartElement("expiredate");
	xml.writeCharacters(m_cacheExpireDate.toString(Qt::ISODate));
	xml.writeEndElement(); //expiredate

	xml.writeStartElement("totozdatacontenttype");
	xml.writeCDATA(m_totozDataContentType);
	xml.writeEndElement(); //totozdata

	xml.writeStartElement("totozdata");
	xml.writeCDATA(m_totozData.toBase64());
	xml.writeEndElement(); //totozdata

	xml.writeEndElement(); //totoz

	buffer.close();

	QByteArray compressedData = qCompress(buffer.data());
	file.write(compressedData);
	file.close();
}
