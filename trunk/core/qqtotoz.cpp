#include "qqtotoz.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QtDebug>
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
	return QFile::exists(QQTotoz::getPath(m_id));
}

QString QQTotoz::getPath(QString id)
{
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));

	if(! dirCache.exists())
		dirCache.mkpath(dirCache.path());

	QString totozIdMd5 = QString(QCryptographicHash::hash((id.toLower().toAscii()),
														  QCryptographicHash::Md5).toHex());
	return dirCache.filePath(totozIdMd5);
}

void QQTotoz::load()
{
	//qDebug() << "QQTotoz::load id: " << m_id << ", path:" << getPath(m_id);
	QFile file(getPath(m_id));
	file.open(QIODevice::ReadOnly);

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
				m_expireDate.fromString(xml.readElementText(), Qt::ISODate);
			}
			else if(currentElement == "imgdata" || currentElement == "totoz")
			{
			}
			else
				qDebug() << "QQTotoz::load unknown element :" << currentElement;

			break;
		case QXmlStreamReader::Characters:
			if(xml.isCDATA())
				m_totozData = QByteArray::fromBase64(xml.text().toAscii());
			else
				qDebug() << "QQTotoz::load unknown Characters :" << xml.text();
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

	buffer.close();
	file.close();

	if(xml.error() != QXmlStreamReader::NoError || m_totozData.size() == 0)
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
	xml.writeCharacters(m_expireDate.toString(Qt::ISODate));
	xml.writeEndElement(); //expiredate

	xml.writeStartElement("imgdata");
	xml.writeCDATA(m_totozData.toBase64());
	xml.writeEndElement(); //imgdata

	xml.writeEndElement(); //totoz

	buffer.close();

	QByteArray compressedData = qCompress(buffer.data());
	file.write(compressedData);
	file.close();
}