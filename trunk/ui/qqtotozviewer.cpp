#include "qqtotozviewer.h"

#include "core/qqtotozdownloader.h"

#include <QtDebug>
#include <QMenu>
#include <QMouseEvent>

#define TIMER_INTERVAL 1000

QQTotozViewer::~QQTotozViewer()
{
}

void QQTotozViewer::setTotozId(const QString &totozId)
{
	if(totozId.startsWith("[:") && totozId.endsWith("]"))
		m_totozId = totozId.mid(2, totozId.length() - 3);
	else
		m_totozId = totozId;

	setToolTip(getAnchor());

	if(totozId.length() > 0)
	{
		m_hasfailed = false;
		updateTotoz();
	}
}

void QQTotozViewer::displayContextMenu(QPoint &pos)
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

	QAction *actionTriggered = NULL;
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
			qWarning() << Q_FUNC_INFO << "Unknown action triggered";
	}
}

void QQTotozViewer::totozAvailable(QString &totozId, bool success, QString &errMsg)
{
	if(totozId == m_totozId)
	{
		disconnect(m_downloader);
		if(success)
			updateTotoz();
		else
			displayText(errMsg);
	}
}

void QQTotozViewer::updateTotoz()
{
	clear();

	QQTotoz totoz(m_totozId);
	QByteArray totozData = totoz.data();
	if(! updateImg(totozData))
		handleInvalidTotozData();

}

void QQTotozViewer::contextMenuEvent(QContextMenuEvent *ev)
{
	ev->accept();
	QPoint pos = ev->globalPos();

	displayContextMenu(pos);
}

void QQTotozViewer::enterEvent(QEvent *event)
{
	QQImageViewer::enterEvent(event);
	m_mousePressOK = false;
}

void QQTotozViewer::leaveEvent(QEvent *event)
{
	QQImageViewer::leaveEvent(event);
	m_mousePressOK = false;
}

void QQTotozViewer::mousePressEvent(QMouseEvent *ev)
{
	QQImageViewer::mousePressEvent(ev);
	if(ev->button() == Qt::LeftButton)
		m_mousePressOK = true;
}

void QQTotozViewer::mouseReleaseEvent(QMouseEvent *ev)
{
	QQImageViewer::mouseReleaseEvent(ev);
	if(ev->button() == Qt::LeftButton && m_mousePressOK)
		emit totozClicked(getAnchor());
}

QString QQTotozViewer::getAnchor()
{
	return QString("[:").append(m_totozId).append("]");
}

void QQTotozViewer::handleInvalidTotozData()
{
	QQTotoz::invalidateCache(m_totozId);
	if(m_downloader != NULL && ! m_hasfailed)
	{
		m_hasfailed = true; // to prevent infinite loops

		connect(m_downloader, SIGNAL(fetchTotozFinished(QString &, bool, QString &)),
				this, SLOT(totozAvailable(QString &, bool, QString &)));
		m_downloader->fetchTotoz(m_totozId);
	}
	else
		displayText("Totoz not found");
}

void QQTotozViewer::init(const QString &totozId)
{
	m_mousePressOK = false;
	m_bookmarkAddEnabled = false;
	m_bookmarkRemEnabled = false;
	m_downloader = NULL;

	setTotozId(totozId);
}
