#include "qqhuntingview.h"

#include "core/qqsettings.h"

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
	setStyleSheet("background: transparent; border-style: none;");
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setCacheMode(QGraphicsView::CacheBackground);
	setFrameStyle(QFrame::NoFrame);

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
	QQSettings settings;

	if(settings.value(SETTINGS_HUNT_MODE, DEFAULT_HUNT_MODE).toInt() == QuteQoin::Disabled ||
	   settings.value(SETTINGS_HUNT_MAX_ITEMS, DEFAULT_HUNT_MAX_ITEMS).toInt() <= m_duckList.size())
		return;

	QQDuckPixmapItem *duck = new QQDuckPixmapItem(srcBouchot, postId);
	duck->setPos(mapFromGlobal(QCursor::pos()));
	scene()->addItem(duck);

	m_duckList << duck;

	duck->animate();
}

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::killDuck
/// \param forceSilent
///
void QQHuntingView::killDuck(bool forceSilent)
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
			QQSettings settings;
			if(! (forceSilent || settings.value(SETTINGS_HUNT_SILENT_ENABLED, DEFAULT_HUNT_SILENT_ENABLED).toBool()))
				emit duckKilled(duck->bouchotName(), duck->postId());

			m_duckList.removeOne(duck);
			duck->animateKill();
			break;
		}
	}
}
