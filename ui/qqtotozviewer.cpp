#include "qqtotozviewer.h"

#include "core/qqtotozdownloader.h"

#include <QtDebug>
#include <QImage>
#include <QMenu>
#include <QMovie>
#include <QMouseEvent>
#include <QPixmap>
#include <QPoint>
#include <QSize>

#define TIMER_INTERVAL 1000

void QQTotozViewer::init(const QString & totozId)
{
	m_mousePressOK = false;
	m_bookmarkAddEnabled = false;
	m_bookmarkRemEnabled = false;
	m_totozMovie = NULL;
	m_downloader = NULL;
	m_totozDataBuffer = new QBuffer(this);

	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0); color : black; }");
	setTextFormat(Qt::PlainText);
	setTextInteractionFlags(Qt::NoTextInteraction);
	setWordWrap(false);

	setTotozId(totozId);
}

void QQTotozViewer::setTotozId(const QString & totozId)
{
	if(totozId.startsWith("[:") && totozId.endsWith("]"))
		m_totozId = totozId.mid(2, totozId.length() - 3);
	else
		m_totozId = totozId;

	setToolTip(getAnchor());

	if(totozId.length() > 0)
	{
		m_hasfailed = false;
		updateImg();
	}
}

QQTotozViewer::~QQTotozViewer()
{
}

void QQTotozViewer::updateImg()
{
	clear();
	if(m_totozMovie != NULL)
		delete m_totozMovie;

	QQTotoz totoz(m_totozId);
	QByteArray totozData = totoz.data();

	m_totozDataBuffer->close();
	m_totozDataBuffer->setData(totozData);
	m_totozDataBuffer->open(QIODevice::ReadOnly);

	m_totozMovie = new QMovie(this);
	m_totozMovie->setDevice(m_totozDataBuffer);

	if(m_totozMovie->isValid())
		displayMovie();
	else
	{
		delete m_totozMovie;
		m_totozMovie = NULL;

		QImage image;
		if(image.loadFromData(totozData))
			displayImage(image);
		else
		{
			m_totozDataBuffer->close();
			QQTotoz::invalidateCache(m_totozId);
			m_totozMovie = new QMovie(":/img/totoz-loader.gif");

			displayMovie();

			if(m_downloader != NULL && ! m_hasfailed)
			{
				m_hasfailed = true; // to prevent infinite loops

				connect(m_downloader, SIGNAL(fetchTotozFinished(QString &, bool)),
						this, SLOT(totozAvailable(QString &, bool)));
				m_downloader->fetchTotoz(m_totozId);
			}
			else
				displayText("Totoz not found");
		}
	}
}

void QQTotozViewer::displayContextMenu(QPoint & pos)
{
	QMenu menu(this);
	QString actBookmAddStr("Add to bookmarks");
	QString actBookmRemStr("Remove from bookmarks");
	if(m_bookmarkAddEnabled)
		menu.addAction(actBookmAddStr);
	if(m_bookmarkRemEnabled)
		menu.addAction(actBookmRemStr);

	QString actSendPalmiStr("Send to palmi");
	menu.addAction(actSendPalmiStr);

	QAction * actionTriggered = NULL;
	if( (actionTriggered = menu.exec(pos)) != NULL )
	{
		QString text = actionTriggered->text();
		if(text == actBookmAddStr)
			emit totozBookmarkAct(m_totozId, QQTotoz::ADD);
		else if(text == actBookmRemStr)
			emit totozBookmarkAct(m_totozId, QQTotoz::REMOVE);
		else if(text == actSendPalmiStr)
			emit totozClicked(getAnchor());
		else
			qWarning() << "Unknown action triggered";
	}

}

void QQTotozViewer::totozAvailable(QString & totozId, bool success)
{
	if(totozId == m_totozId)
	{
		disconnect(m_downloader);
		if(success)
			updateImg();
		else
			displayText("Totoz not found");
	}
}

void QQTotozViewer::mousePressEvent(QMouseEvent * ev)
{
	QLabel::mousePressEvent(ev);
	if(ev->button() == Qt::LeftButton)
		m_mousePressOK = true;
}

void QQTotozViewer::mouseReleaseEvent(QMouseEvent * ev)
{
	QLabel::mouseReleaseEvent(ev);
	if(ev->button() == Qt::LeftButton && m_mousePressOK)
		emit totozClicked(getAnchor());
}

void QQTotozViewer::enterEvent(QEvent * event)
{
	QLabel::enterEvent(event);
	m_mousePressOK = false;
}

void QQTotozViewer::leaveEvent(QEvent * event)
{
	QLabel::leaveEvent(event);
	m_mousePressOK = false;
}

void QQTotozViewer::contextMenuEvent(QContextMenuEvent * ev)
{
	//QLabel::contextMenuEvent(ev);

	ev->accept();
	QPoint pos = ev->globalPos();

	displayContextMenu(pos);
}

void QQTotozViewer::displayText(QString text)
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 200); color : black; }");
	QFontMetrics fm(font());
	QSize txtSize = fm.size(Qt::TextSingleLine, text);
	setMinimumSize(txtSize);
	setMaximumSize(txtSize);
	setText(text);
}

void QQTotozViewer::displayMovie()
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0); color : black; }");
	m_totozMovie->setCacheMode(QMovie::CacheNone);
	m_totozMovie->jumpToFrame(0);
	setMovie(m_totozMovie);
	setMinimumSize(m_totozMovie->frameRect().size());
	setMaximumSize(m_totozMovie->frameRect().size());
	adjustSize();
	m_totozMovie->start();
}

void QQTotozViewer::displayImage(QImage & image)
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0); color : black; }");
	QPixmap pixmap = QPixmap::fromImage(image);
	setPixmap(pixmap);
	setMinimumSize(pixmap.width(), pixmap.height());
	setMaximumSize(pixmap.width(), pixmap.height());
	adjustSize();
}

QString QQTotozViewer::getAnchor()
{
	return QString("[:").append(m_totozId).append("]");
}
