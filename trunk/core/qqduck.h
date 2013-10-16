#ifndef QQDUCK_H
#define QQDUCK_H

#include "qqduckpixmapitem.h"

#include <QObject>
#include <QString>

class QQDuckPixmapItem;

class QQDuck : public QObject
{
	Q_OBJECT
public:
	explicit QQDuck(QObject *parent = 0);
	QQDuck(QString tribuneName, QString postId, QObject *parent = 0);

	~QQDuck();

	QString tribuneName() const { return m_tribuneName; }
	QString postId() const { return m_postId; }

	void setPixmapItem(QQDuckPixmapItem *pixmapItem) { m_pixmapItem = pixmapItem; }
	QQDuckPixmapItem *pixmapItem() const { return m_pixmapItem; }

public slots:
	void kill();
	void launch();

private:
	QString m_tribuneName;
	QString m_postId;
	QQDuckPixmapItem *m_pixmapItem;
};

#endif // QQDUCK_H
