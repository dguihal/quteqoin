#include "qqtotoz.h"


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
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation).append("QuteQoin"));

	if(! dirCache.exists())
		dirCache.mkpath(dirCache.path());

	return dirCache.filePath(id);
}
