#include "qqtsvparser.h"

#include <QtDebug>
#include <QBuffer>

#define CHAR_SEP '\t'

QQTsvParser::QQTsvParser(QObject *parent) : QQBackendParser(parent)
{

}



bool QQTsvParser::parseBackend(const QByteArray &data)
{
	QBuffer f;

	f.setData(data);
	f.open(QIODevice::ReadOnly);

	while(f.canReadLine())
	{
		QByteArray l = f.readLine();

		QList<QByteArray> fields = l.split(CHAR_SEP);
		if(fields.length() == 5)
		{
			bool ok = true;
			qlonglong id = fields.at(0).toLongLong(&ok);
			if(!ok || id <= m_lastId)
				continue;
			if(id > m_maxId)
				m_maxId = id;

			m_currentPost.reset();
			m_currentPost.setId(fields.at(0));
			m_currentPost.setNorloge(fields.at(1));
			m_currentPost.setUA(fields.at(2));
			m_currentPost.setLogin(fields.at(3));
			m_currentPost.setMessage(fields.at(4).simplified());

			emit newPostReady(m_currentPost);
		}
	}

	m_lastId = m_maxId;

	f.close();
	emit finished();

	return true;
}
