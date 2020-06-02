#ifndef QQDUCKPIXMAPITEM_H
#define QQDUCKPIXMAPITEM_H

#include "qqhuntpixmapitem.h"

class QQDuckPixmapItem : public QQHuntPixmapItem
{
	Q_OBJECT

public:
	explicit QQDuckPixmapItem(QString srcBouchot, QString postId, bool selfDuck, QObject *parent = 0);
	~QQDuckPixmapItem() {}

protected:
	virtual QQHuntPixmapItem::QQPixmapProp animPixmapProp(float angle);
	virtual QQHuntPixmapItem::QQPixmapProp fallPixmapProp();
	virtual QQHuntPixmapItem::QQPixmapProp killPixmapProp();
};

#endif // QQDUCKPIXMAPITEM_H
