#include "qqhuntingview.h"

#include "qqduckpixmapitem.h"

#include <QtDebug>
#include <QCursor>
#include <QPropertyAnimation>
#include <QResizeEvent>

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::QQHuntingView
/// \param parent
///
QQHuntingView::QQHuntingView(QWidget *parent) :
	QGraphicsView(parent)
{
	setRenderHint(QPainter::Antialiasing);
	setPalette(Qt::transparent);
	setStyleSheet("background: transparent");
	setAttribute(Qt::WA_TransparentForMouseEvents);

	QGraphicsScene *scene = new QGraphicsScene();
	scene->setSceneRect(rect());
	setScene(scene);

	resize(size());
}

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::~QQHuntingView
///
QQHuntingView::~QQHuntingView()
{
	foreach (QQDuck *duck, m_duckList) {
		delete duck;
	}
	m_duckList.clear();

	QGraphicsScene *currScene = scene();
	currScene->clear();
	delete currScene;
}

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::resizeEvent
/// \param event
///
void QQHuntingView::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);
	if(scene())
		scene()->setSceneRect(0, 0, event->size().width(), event->size().height());
	event->accept();
}

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::launch
/// \param srcBouchot
/// \param postId
///
void QQHuntingView::launch(QString srcBouchot, QString postId)
{
	qDebug() << srcBouchot << postId;
	QQDuck *duck = new QQDuck(srcBouchot, postId, this);

	QQDuckPixmapItem *item = new QQDuckPixmapItem(srcBouchot, postId);
	item->setPos(mapFromGlobal(QCursor::pos()));
	scene()->addItem(item);

	duck->setPixmapItem(item);
	m_duckList << duck;

	duck->launch();
}

void QQHuntingView::killDuck()
{
	QPoint shotPoint = mapFromGlobal(QCursor::pos());
	QPointF shotPointF(shotPoint);

	foreach(QQDuck *duck, m_duckList)
	{
		QQDuckPixmapItem *pixmapItem = duck->pixmapItem();
		QPointF itemPos = pixmapItem->pos();
		QRectF itemRect = pixmapItem->boundingRect();
		if(shotPointF.x() >= itemPos.x() && shotPointF.x() <= (itemPos.x() + itemRect.width()) &&
		   shotPointF.y() >= itemPos.y() && shotPointF.y() <= (itemPos.y() + itemRect.height()))
		{
			emit duckKilled(duck->tribuneName(), duck->postId());
			m_duckList.removeOne(duck);
			duck->kill();
			break;
		}
	}
}
