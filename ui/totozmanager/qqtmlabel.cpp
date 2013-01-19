#include "qqtmlabel.h"

#include "core/qqtotoz.h"

#include <QtDebug>
#include <QImage>
#include <QMenu>
#include <QMovie>
#include <QMouseEvent>
#include <QPixmap>
#include <QPoint>
#include <QSize>
#include <QTimer>

#define TIMER_INTERVAL 1000

QQTMLabel::QQTMLabel(const QString & totozId, QWidget * parent) :
	QLabel(parent)

{
	m_totozId = totozId;
	m_mousePressOK = false;
	m_bookmarkMenuEnabled = true;

	setToolTip(getAnchor());
	setWordWrap(true);

	loadFailed = false;
	m_totozMovie = NULL;

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));

	update();
}

QQTMLabel::~QQTMLabel()
{
	if(m_totozMovie != NULL)
		delete m_totozMovie;
}

void QQTMLabel::update()
{
	m_timer->stop();

	if(m_totozMovie != NULL)
		delete m_totozMovie;
	m_totozMovie = new QMovie(QQTotoz::getPath(m_totozId));

	if(m_totozMovie->isValid())
	{
		displayMovie(m_totozMovie);
	}
	else
	{
		delete m_totozMovie;
		m_totozMovie = NULL;
		QImage image;
		if(image.load(QQTotoz::getPath(m_totozId)))
		{
			displayImage(image);
		}
		else
		{
			m_timer->start(TIMER_INTERVAL);

			m_totozMovie = new QMovie(":/img/totoz-loader.gif");
			displayMovie(m_totozMovie);
			loadFailed = true;
		}
	}
}

void QQTMLabel::mousePressEvent(QMouseEvent * ev)
{
	QLabel::mousePressEvent(ev);
	if(ev->button() == Qt::LeftButton)
		m_mousePressOK = true;
}

void QQTMLabel::mouseReleaseEvent(QMouseEvent * ev)
{
	QLabel::mouseReleaseEvent(ev);
	if(ev->button() == Qt::LeftButton && m_mousePressOK)
		emit totozClicked(getAnchor());
}

void QQTMLabel::enterEvent(QEvent * event)
{
	QLabel::enterEvent(event);
	m_mousePressOK = false;
}

void QQTMLabel::leaveEvent(QEvent * event)
{
	QLabel::leaveEvent(event);
	m_mousePressOK = false;
}

void QQTMLabel::contextMenuEvent(QContextMenuEvent * ev)
{
	QLabel::contextMenuEvent(ev);

	QMenu menu(this);
	QString actBookmStr("Bookmark");
	if(m_bookmarkMenuEnabled)
		menu.addAction(actBookmStr);

	QString actSendPalmiStr("Send to palmi");
	menu.addAction(actSendPalmiStr);

	QAction * actionTriggered = NULL;
	if( (actionTriggered = menu.exec(ev->globalPos())) != NULL )
	{
		QString text = actionTriggered->text();
		if(text == actBookmStr)
			emit bookmarkingAsked(getAnchor());
		else if(text == actSendPalmiStr)
			emit totozClicked(getAnchor());
		else
			qWarning() << "Unknown action triggered";
	}
}

void QQTMLabel::displayMovie(QMovie * movie)
{
	movie->setCacheMode(QMovie::CacheNone);
	movie->jumpToFrame(0);
	setMovie(movie);
	setMinimumSize(movie->frameRect().size());
	setMaximumSize(movie->frameRect().size());
	adjustSize();
	movie->start();
}

void QQTMLabel::displayImage(QImage & image)
{
	QPixmap pixmap = QPixmap::fromImage(image);
	setPixmap(pixmap);
	setMinimumSize(pixmap.width(), pixmap.height());
	setMaximumSize(pixmap.width(), pixmap.height());
	adjustSize();
}

QString QQTMLabel::getAnchor()
{
	return QString("[:").append(m_totozId).append("]");
}
