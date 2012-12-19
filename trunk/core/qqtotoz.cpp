#include "qqtotoz.h"

#include <QCryptographicHash>

QQTotoz::QQTotoz() {}

QQTotoz::QQTotoz(const QString & totozId, int posInMessage)
{
	m_posInMessage = posInMessage;
	m_id = totozId.mid(2, totozId.length() - 3);
}

QQTotoz::QQTotoz(const QQTotoz & totoz)
{
	m_posInMessage = totoz.m_posInMessage;
	m_id = totoz.m_id;
}

QQTotoz::~QQTotoz()
{
}

QString QQTotoz::getPath(QString id)
{
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
	if(! dirCache.cd("QuteQoin"))
	{
		dirCache.mkpath(dirCache.path());
		dirCache.cd("QuteQoin");
	}


	QString totozIdMd5 = QString(QCryptographicHash::hash((id.toAscii()),
														  QCryptographicHash::Md5).toHex());

	return dirCache.filePath(id);
}
