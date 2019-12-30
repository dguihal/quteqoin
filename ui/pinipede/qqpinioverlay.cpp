#include "qqpinioverlay.h"

#include <QGraphicsVideoItem>
#include <QMediaPlaylist>
#include <QMediaPlayer>

#include "core/qqsettings.h"
#include "core/qqwebdownloader.h"
#include "ui/qqtotozviewer.h"
#include "ui/pinipede/qqduckpixmapitem.h"
#include "ui/pinipede/qqwebimageviewer.h"

#include <QCoreApplication>
#include <QGraphicsProxyWidget>
#include <QResizeEvent>
#include <QTemporaryFile>
#include <QToolTip>

//////////////////////////////////////////////////////////////
/// \brief The OverlayPlayer class
///
class OverlayPlayer
{
public:
	virtual ~OverlayPlayer() {}

	virtual void show() {}
	virtual void hide() {}
};

class ImagePlayer : public OverlayPlayer
{
public:
	ImagePlayer(QGraphicsProxyWidget *gpw, QQImageViewer *imgV) :
	    m_gpw(gpw), m_imgV(imgV) {}
	virtual ~ImagePlayer()
	{
		if(m_gpw != nullptr)
		{
			m_gpw->hide();
			m_gpw->deleteLater();
		}
		if(m_imgV != nullptr)
			m_imgV->deleteLater();
	}

	virtual void show() { m_gpw->show(); }
	virtual void hide() { m_gpw->hide(); }

	QGraphicsProxyWidget *m_gpw;
	QQImageViewer *m_imgV;
};

//////////////////////////////////////////////////////////////
/// \brief The TotozPlayer class
///
class TotozPlayer : public OverlayPlayer
{
public:
	TotozPlayer(QGraphicsProxyWidget *gpw, QQTotozViewer *ttV) :
	    m_gpw(gpw), m_ttV(ttV) {}
	virtual ~TotozPlayer()
	{
		if(m_gpw != nullptr)
		{
			m_gpw->hide();
			m_gpw->deleteLater();
		}
		if(m_ttV != nullptr)
			m_ttV->deleteLater();
	}

	virtual void show() { m_gpw->show(); }
	virtual void hide() { m_gpw->hide(); }

	QGraphicsProxyWidget *m_gpw;
	QQTotozViewer *m_ttV;
};

//////////////////////////////////////////////////////////////
/// \brief The VideoPlayer class
///
class VideoPlayer : public OverlayPlayer
{
public:
	VideoPlayer(QGraphicsVideoItem *item, QMediaPlayer *player, QMediaPlaylist *mediaList) :
	    m_gObj(item), m_player(player), m_media(mediaList) {}
	virtual ~VideoPlayer()
	{
		if(m_player != nullptr)
			m_player->deleteLater();
		if(m_media != nullptr)
			m_media->deleteLater();
		if(m_gObj != nullptr)
		{
			m_gObj->hide();
			m_gObj->deleteLater();
		}
	}

	virtual void show() { m_gObj->show(); }
	virtual void hide() { m_gObj->hide(); }
	void stop() { m_player->stop(); }

	QString errorString() { return m_media->errorString(); }

	QGraphicsVideoItem *m_gObj;
	QMediaPlayer *m_player;
	QMediaPlaylist *m_media;
};

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::QQPiniOverlay
/// \param parent
///
QQPiniOverlay::QQPiniOverlay(QWidget *parent) :
    QGraphicsView(parent),
    m_downloader(new QQWebDownloader(this)),
    m_img(nullptr),
    m_currentPlayer(nullptr),
    m_pendingPlayer(nullptr)
{
	setRenderHint(QPainter::Antialiasing);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("background: transparent;");
	setFrameStyle(QFrame::NoFrame);

	/* QGraphicsScene *scene = new QGraphicsScene();
	scene->setSceneRect(rect());
	setScene(scene); */
	setScene(new QGraphicsScene());

	if(parent != nullptr)
		resize(parent->size());

	connect(m_downloader, SIGNAL(ready(QUrl &)), this, SLOT(dlReady(QUrl &)));
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::~QQPiniOverlay
///
QQPiniOverlay::~QQPiniOverlay()
{
	clearOverview();

	QGraphicsScene *s = scene();
	delete s;
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::focusOutEvent
/// \param event
///
void QQPiniOverlay::focusOutEvent(QFocusEvent *event)
{
	qDebug() << Q_FUNC_INFO << event->reason();
	QGraphicsView::focusOutEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::resizeEvent
/// \param event
///
void QQPiniOverlay::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);
	if(scene())
		scene()->setSceneRect(0, 0, event->size().width(), event->size().height());
	event->accept();
}

//////////////////////////////////////////////////////////////
/// \brief dlReady
///
void QQPiniOverlay::dlReady(QUrl &url)
{
	if (m_pendingURLs.isEmpty() || m_pendingURLs.top() != url)
		return;
	else
		m_pendingURLs.clear();

	QQSettings settings;
	int maxSize = settings.value(SETTINGS_WEB_IMAGE_PREVIEW_SIZE, DEFAULT_WEB_IMAGE_PREVIEW_SIZE).toInt();

	QString contentType = m_downloader->dataContentType();
	if(contentType.startsWith("image/"))
	{
		QQImageViewer *imgV = new QQImageViewer();
		imgV->updateImg(m_downloader->imgData(), QSize(maxSize, maxSize));
		QGraphicsProxyWidget *gpw = scene()->addWidget(imgV, Qt::Widget);
		moveToMousePos(gpw, imgV->size());

		if(m_currentPlayer != nullptr)
			delete m_currentPlayer;
		m_currentPlayer = new ImagePlayer(gpw, imgV);
		m_currentPlayer->show();
	}
	else if(contentType.startsWith("video/"))
	{
		QTemporaryFile *f = new QTemporaryFile();
		if (f->open()) {
			f->write(m_downloader->imgData());
			f->flush();
			m_tmpFiles.append(f);
			QUrl url = QUrl::fromLocalFile(f->fileName());
			showVideo(url);
		}
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::doVideoStateChanged
/// \param newState
///
void QQPiniOverlay::doVideoStateChanged(QMediaPlayer::State newState)
{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	if(newState == QMediaPlayer::PlayingState)
	{
		if(m_currentPlayer != nullptr)
			delete m_currentPlayer;

		m_currentPlayer = m_pendingPlayer;
		m_pendingPlayer = nullptr;

		if(m_currentPlayer != nullptr)
			m_currentPlayer->show();
	}
#else
	Q_UNUSED(newState)
#endif
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::handleVideoError
/// \param error
///
void QQPiniOverlay::handleVideoError(QMediaPlayer::Error error)
{
	qDebug() << Q_FUNC_INFO << error;
	if(error != QMediaPlayer::NoError)
	{
		QString errString;
		VideoPlayer * pVPlayer;
		if((pVPlayer = dynamic_cast<VideoPlayer *>(m_pendingPlayer)) != nullptr)
		{
			errString = pVPlayer->errorString();
			pVPlayer->stop();
			delete m_pendingPlayer;
		}
		else if((pVPlayer = dynamic_cast<VideoPlayer *>(m_currentPlayer)) != nullptr)
		{
			errString = pVPlayer->errorString();
			pVPlayer->stop();
			delete m_currentPlayer;
		}

		if(errString.length() == 0)
			switch(error)
			{
				case QMediaPlayer::ResourceError:
					errString = "Resource Error";
					break;
				case QMediaPlayer::FormatError:
					errString = "Format Error";
					break;
				case QMediaPlayer::NetworkError:
					errString = "Network Error";
					break;
				case QMediaPlayer::AccessDeniedError:
					errString = "Access Denied Error";
					break;
				case QMediaPlayer::ServiceMissingError:
					errString = "Service Missing Error";
					break;
				case QMediaPlayer::MediaIsPlaylist:
					errString = "Media Is Playlist Error";
					break;
				default:
					errString = QString("Unknown Error %1").arg(error);
			}

		QQImageViewer *v = new QQImageViewer();
		v->setText(QString("Error playing media : %1").arg(errString));
		QGraphicsProxyWidget *gpw = scene()->addWidget(v, Qt::Widget);
		moveToMousePos(gpw, v->size());

		m_currentPlayer = new ImagePlayer(gpw, v);
		m_currentPlayer->show();
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::launchDuck
/// \param srcBouchot
/// \param postId
///
void QQPiniOverlay::launchDuck(QString srcBouchot, QString postId, bool selfDuck)
{
	QQSettings settings;

	if(settings.value(SETTINGS_HUNT_MODE, DEFAULT_HUNT_MODE).toInt() == QuteQoin::HuntMode_Disabled ||
	   settings.value(SETTINGS_HUNT_MAX_ITEMS, DEFAULT_HUNT_MAX_ITEMS).toInt() <= m_duckList.size())
		return;

	QQDuckPixmapItem *duck = new QQDuckPixmapItem(srcBouchot, postId, selfDuck);
	duck->setPos(mapFromGlobal(QCursor::pos()));
	scene()->addItem(duck);

	m_duckList << duck;

	duck->animate();
}

//////////////////////////////////////////////////////////////
/// \brief QQHuntingView::killDuck
/// \param forceSilent
///
void QQPiniOverlay::killDuck(bool forceSilent)
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
			if(! (forceSilent
			      || settings.value(SETTINGS_HUNT_SILENT_ENABLED, DEFAULT_HUNT_SILENT_ENABLED).toBool()
			      || duck->isSelfItem()))
				emit duckKilled(duck->bouchotName(), duck->postId());

			m_duckList.removeOne(duck);
			duck->animateKill();
			break;
		}
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::showTotoz
/// \param totozId
///
void QQPiniOverlay::showTotoz(const QString &totozId)
{
	QQSettings settings;
	if(settings.value(SETTINGS_TOTOZ_VISUAL_MODE, DEFAULT_TOTOZ_VISUAL_MODE).toString() ==
	        TOTOZ_VISUAL_MODE_DISABLED)
		return;

	QQTotozViewer *v = new QQTotozViewer(totozId, nullptr);
	QGraphicsProxyWidget *gpw = scene()->addWidget(v, Qt::Widget);
	moveToMousePos(gpw, v->size());

	if(m_currentPlayer != nullptr)
		delete m_currentPlayer;
	m_currentPlayer = new TotozPlayer(gpw, v);
	m_currentPlayer->show();
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::showUrl
/// \param url
///
void QQPiniOverlay::showUrl(const QUrl &url, QString &contentType)
{

#ifdef WIN32
	if(contentType.startsWith("image/"))
#else
	showWaitAnim();

	if(contentType.startsWith("video/") ||
	        contentType.startsWith("image/"))
#endif
	{
		m_pendingURLs.push(url);
		if(contentType.startsWith("video/"))
			showVideo(url);
		else
			m_downloader->getURL(url);
	}
}

//////////////////////////////////////////////////////////////
/// \brief clearOverview
///
void QQPiniOverlay::clearOverview()
{
	m_pendingURLs.clear();
	if(m_currentPlayer != nullptr)
	{
		delete m_currentPlayer;
		m_currentPlayer = nullptr;
	}

	while(! m_tmpFiles.isEmpty())
	{
		QTemporaryFile *f = m_tmpFiles.takeFirst();

		if(f != nullptr)
			delete f;
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::setPos
///
void QQPiniOverlay::moveToMousePos(QGraphicsObject *gpw, const QSize &s)
{
	bool shouldMoveToolTip = true;
	QPoint cursorPos = mapFromGlobal(QCursor::pos());

	if(cursorPos.x() > (width() / 2)) // a droite
	{
		cursorPos.rx() -= s.width();
		shouldMoveToolTip = false;
	}
	if(cursorPos.y() > (height() / 2)) // en dessous
	{
		cursorPos.ry() -= s.height();
		shouldMoveToolTip = false;
	}

	gpw->setX(cursorPos.x());
	gpw->setY(cursorPos.y());

	if(shouldMoveToolTip && QToolTip::isVisible())
	{
		QString txt = QToolTip::text();
		QToolTip::hideText();

		QPoint ttPos = cursorPos;
		ttPos.rx() += s.width();

		QToolTip::hideText();
		QCoreApplication::processEvents();
		QToolTip::showText(mapToGlobal(ttPos), txt, parentWidget());
	}
}

//////////////////////////////////////////////////////////////
/// \brief showVideo
/// \param url
///
void QQPiniOverlay::showVideo(const QUrl &url)
{
	QQSettings settings;
	int maxSize = settings.value(SETTINGS_WEB_IMAGE_PREVIEW_SIZE, DEFAULT_WEB_IMAGE_PREVIEW_SIZE).toInt();
	QSize s(maxSize, maxSize);

	auto player = new QMediaPlayer();
	//auto player = new QMediaPlayer(this);
	player->setMuted(settings.value(SETTINGS_GENERAL_STEALTH_MODE, DEFAULT_GENERAL_STEALTH_MODE).toBool());

	/*
	auto i = new QGraphicsVideoItem();
	i->setAspectRatioMode(Qt::KeepAspectRatio);
	i->setSize(s);
	player->setVideoOutput(i);
	scene()->addItem(i);
	*/

	auto *v = new QVideoWidget;
	v->setAspectRatioMode(Qt::KeepAspectRatio);
	player->setVideoOutput(v);
	auto *p = new QGraphicsProxyWidget();
	p->setWidget(v);
	scene()->addItem(p);

	auto l = new QMediaPlaylist();
	l->addMedia(url);
	l->setPlaybackMode(QMediaPlaylist::Loop);

	moveToMousePos(p, s);

	m_pendingPlayer = nullptr; //new VideoPlayer(i, player, l);
	if(m_currentPlayer != nullptr)
		m_currentPlayer->hide();

	connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
	        this, SLOT(doVideoStateChanged(QMediaPlayer::State)));

	connect(player, SIGNAL(error(QMediaPlayer::Error)),
	        this, SLOT(handleVideoError(QMediaPlayer::Error)));

	player->setPlaylist(l);
	player->play();
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::showWaitAnim
///
void QQPiniOverlay::showWaitAnim()
{
	QQImageViewer *imgV = new QQImageViewer();
	imgV->displayWaitMovie();
	QGraphicsProxyWidget *gpw = scene()->addWidget(imgV, Qt::Widget);
	moveToMousePos(gpw, imgV->size());

	if(m_currentPlayer != nullptr)
		delete m_currentPlayer;
	m_currentPlayer = new ImagePlayer(gpw, imgV);
	m_currentPlayer->show();

}
