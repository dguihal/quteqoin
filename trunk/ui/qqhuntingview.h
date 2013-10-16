#ifndef QQHUNTINGVIEW_H
#define QQHUNTINGVIEW_H

#include "core/qqduck.h"

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
	void launch(QString srcBouchot, QString postId);
	void killDuck();

protected:
	virtual void resizeEvent(QResizeEvent *event);

private:

	QList<QQDuck *> m_duckList;
};

#endif // QQHUNTINGVIEW_H
