#include "qqduckpixmapitem.h"

#include <qmath.h>

QQDuckPixmapItem::QQDuckPixmapItem(QString srcBouchot, QString postId, QObject *parent)
	: QQHuntPixmapItem(srcBouchot, postId, parent)
{
	m_maxAnimPixmapNumInc = 3;
}

#if !defined(M_PI_6)
#define M_PI_6 (M_PI_2 / 3)
#endif

QQHuntPixmapItem::QQPixmapProp QQDuckPixmapItem::animPixmapProp(float angle)
{
	QQPixmapProp pixmapProp;
	if((0 <= angle) && (angle < M_PI_6))
		pixmapProp.m_pixmapName = "duck_hor/duck_hor_%1.png";
	else if((M_PI_6 <= angle) && (angle < 2 * M_PI_6))
	{
		pixmapProp.m_pixmapName = "duck_diag/duck_diag_%1.png";
		if(m_speedVec(Y_VALUE) > 0)
			pixmapProp.m_rotationAngle = M_PI_2;
	}
	else if((2 * M_PI_6 <= angle) && (angle < 4 * M_PI_6))
	{
		pixmapProp.m_pixmapName = "duck_vert/duck_vert_%1.png";
		if(m_speedVec(Y_VALUE) > 0)
			pixmapProp.m_mirrorY = true;
	}
	else if((4 * M_PI_6 <= angle) && (angle < 5 * M_PI_6))
	{
		pixmapProp.m_pixmapName = "duck_diag/duck_diag_%1.png";
		pixmapProp.m_mirrorX = true;
		if(m_speedVec(Y_VALUE) > 0)
			pixmapProp.m_rotationAngle = M_PI_2;
	}
	else
	{
		pixmapProp.m_pixmapName = "duck_hor/duck_hor_%1.png";
		pixmapProp.m_mirrorX = true;
	}

	return pixmapProp;
}

QQHuntPixmapItem::QQPixmapProp QQDuckPixmapItem::fallPixmapProp()
{
	QQPixmapProp pixmapProp;
	pixmapProp.m_pixmapName = "duck_kill/duck_fall.png";

	return pixmapProp;

}

QQHuntPixmapItem::QQPixmapProp QQDuckPixmapItem::killPixmapProp()
{
	QQPixmapProp pixmapProp;
	pixmapProp.m_pixmapName = "duck_kill/duck_kill.png";

	return pixmapProp;
}
