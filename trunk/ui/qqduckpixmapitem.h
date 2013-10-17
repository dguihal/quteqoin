#ifndef QQDUCKPIXMAPITEM_H
#define QQDUCKPIXMAPITEM_H

#include "core/qqhuntpixmapitem.h"

class QQDuckPixmapItem : public QQHuntPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	explicit QQDuckPixmapItem(QString srcBouchot, QString postId, QObject *parent = 0);
	~QQDuckPixmapItem() {}

protected:
	virtual QQHuntPixmapItem::QQPixmapProp animPixmapProp(float angle);
	virtual QQHuntPixmapItem::QQPixmapProp fallPixmapProp();
	virtual QQHuntPixmapItem::QQPixmapProp killPixmapProp();
};

#endif // QQDUCKPIXMAPITEM_H
