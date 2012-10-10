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
