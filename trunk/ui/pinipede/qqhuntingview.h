#ifndef QQHUNTINGVIEW_H
#define QQHUNTINGVIEW_H

#include "qqduckpixmapitem.h"

#include <QGraphicsView>
#include <QObject>
#include <QList>

class QQHuntingView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit QQHuntingView(QWidget *parent = 0);
	~QQHuntingView();
signals:
	void duckKilled(QString srcBouchot, QString postId);

public slots:
	void launchDuck(QString srcBouchot, QString postId, bool selfDuck);
	void killDuck(bool forceSilent);

protected:
	virtual void resizeEvent(QResizeEvent *event);

private:

	QList<QQDuckPixmapItem *> m_duckList;
};

#endif // QQHUNTINGVIEW_H
