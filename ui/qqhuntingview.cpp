#include "qqhuntingview.h"

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
	setStyleSheet("background: transparent; border: transparent;");
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
/// \brief QQHuntingView::launchDuck
/// \param srcBouchot
/// \param postId
///
void QQHuntingView::launchDuck(QString srcBouchot, QString postId)
{
	qDebug() << srcBouchot << postId;

	QQDuckPixmapItem *duck = new QQDuckPixmapItem(srcBouchot, postId);
	duck->setPos(mapFromGlobal(QCursor::pos()));
	scene()->addItem(duck);

	m_duckList << duck;

	duck->animate();
}

void QQHuntingView::killDuck()
{
	QPoint shotPoint = mapFromGlobal(QCursor::pos());
	QPointF shotPointF(shotPoint);

	foreach(QQDuckPixmapItem *duck, m_duckList)
	{
		QPointF itemPos = duck->pos();
		QRectF itemRect = duck->boundingRect();
		if(shotPointF.x() >= itemPos.x() && shotPointF.x() <= (itemPos.x() + itemRect.width()) &&
		   shotPointF.y() >= itemPos.y() && shotPointF.y() <= (itemPos.y() + itemRect.height()))
		{
			emit duckKilled(duck->bouchotName(), duck->postId());
			m_duckList.removeOne(duck);
			duck->animateKill();
			break;
		}
	}
}
