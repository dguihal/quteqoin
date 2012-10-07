#include "qqtotoz.h"

QQTotoz::QQTotoz()
{
	m_typeTotoz = QQTOTOZ_NOTFOUND;
	m_totozStaticImgPtr.clear();
	m_totozDynImgPtr.clear();
}

QQTotoz::QQTotoz(const QString & totozId, int posInMessage)
{
	m_typeTotoz = QQTOTOZ_UNDEF;
	m_posInMessage = posInMessage;
	m_id = totozId.mid(2, totozId.length() - 3);
	m_totozStaticImgPtr.clear();
	m_totozDynImgPtr.clear();
}

QQTotoz::QQTotoz(const QQTotoz & totoz)
{
	m_typeTotoz = totoz.m_typeTotoz;
	m_posInMessage = totoz.m_posInMessage;
	m_id = totoz.m_id;
	m_totozStaticImgPtr = totoz.m_totozStaticImgPtr;
	m_totozDynImgPtr = totoz.m_totozDynImgPtr;
}

QQTotoz::~QQTotoz()
{
}
