#include "qqpinioverlay.h"

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QGraphicsVideoItem>
#include <QMediaPlaylist>
#else
#include <phonon/AudioOutput>
#include <phonon/VideoPlayer>
#include <phonon/MediaObject>
#endif

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
	explicit OverlayPlayer() {}
	virtual ~OverlayPlayer() {}
	enum PlayerType { GenericOverlayPlayer, TypeVideoPlayer, TypeTotozPlayer, TypeImagePlayer };

	virtual PlayerType playerType() { return GenericOverlayPlayer; }
	virtual void show() {}
	virtual void hide() {}
};

class ImagePlayer : public OverlayPlayer
{
public:
	explicit ImagePlayer(QGraphicsProxyWidget *gpw, QQImageViewer *imgV) :
		m_gpw(gpw), m_imgV(imgV) {}
	virtual ~ImagePlayer()
	{
		if(m_gpw != NULL)
		{
			m_gpw->hide();
			m_gpw->deleteLater();
		}
		if(m_imgV != NULL)
			m_imgV->deleteLater();
	}

	virtual PlayerType playerType() { return TypeImagePlayer; }
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
	explicit TotozPlayer(QGraphicsProxyWidget *gpw, QQTotozViewer *ttV) :
		m_gpw(gpw), m_ttV(ttV) {}
	virtual ~TotozPlayer()
	{
		if(m_gpw != NULL)
		{
			m_gpw->hide();
			m_gpw->deleteLater();
		}
		if(m_ttV != NULL)
			m_ttV->deleteLater();
	}

	virtual PlayerType playerType() { return TypeTotozPlayer; }
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
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	explicit VideoPlayer(QGraphicsVideoItem *item, QMediaPlayer *player, QMediaPlaylist *mediaList) :
		m_gObj(item), m_player(player), m_media(mediaList) {}
#else
	explicit VideoPlayer(QGraphicsProxyWidget *gpw, Phonon::VideoPlayer *player, Phonon::MediaObject *media) :
		m_gObj(gpw), m_player(player), m_media(media) {}
#endif
	virtual ~VideoPlayer()
	{
		if(m_player != NULL)
			m_player->deleteLater();
		if(m_media != NULL)
			m_media->deleteLater();
		if(m_gObj != NULL)
		{
			m_gObj->hide();
			m_gObj->deleteLater();
		}
	}

	virtual PlayerType playerType() { return TypeVideoPlayer; }
	virtual void show() { m_gObj->show(); }
	virtual void hide() { m_gObj->hide(); }

	QString errorString() { return m_media->errorString(); }

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QGraphicsVideoItem *m_gObj;
	QMediaPlayer *m_player;
	QMediaPlaylist *m_media;
#else
	QGraphicsProxyWidget *m_gObj;
	Phonon::VideoPlayer *m_player;
	Phonon::MediaObject *m_media;
#endif
};

//////////////////////////////////////////////////////////////
/// \brief QQPiniOverlay::QQPiniOverlay
/// \param parent
///
QQPiniOverlay::QQPiniOverlay(QWidget *parent) :
	QGraphicsView(parent),
	m_downloader(new QQWebDownloader(this)),
	m_img(NULL),
	m_currentPlayer(NULL),
	m_pendingPlayer(NULL)
{
	setRenderHint(QPainter::Antialiasing);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("background: transparent;");
	setFrameStyle(QFrame::NoFrame);

	QGraphicsScene *scene = new QGraphicsScene();
	scene->setSceneRect(rect());
	setScene(scene);

	connect(m_downloader, SIGNAL(ready()), this, SLOT(dlReady()));
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
void QQPiniOverlay::dlReady()
{
	QQSettings settings;
	int maxSize = settings.value(SETTINGS_WEB_IMAGE_PREVIEW_SIZE, DEFAULT_WEB_IMAGE_PREVIEW_SIZE).toInt();

	QString contentType = m_downloader->dataContentType();
	if(contentType.startsWith("image/"))
	{
		QQImageViewer *imgV = new QQImageViewer();
		imgV->updateImg(m_downloader->imgData(), QSize(maxSize, maxSize));
		QGraphicsProxyWidget *gpw = scene()->addWidget(imgV, Qt::Widget);
		moveToMousePos(gpw, imgV->size());

		if(m_currentPlayer != NULL)
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
		if(m_currentPlayer != NULL)
			delete m_currentPlayer;

		m_currentPlayer = m_pendingPlayer;
		m_pendingPlayer = NULL;

		if(m_currentPlayer != NULL)
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
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	qDebug() << Q_FUNC_INFO << error;
	if(error != QMediaPlayer::NoError)
	{
		QString errString = ((VideoPlayer *) m_currentPlayer)->errorString();
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

		if(m_currentPlayer != NULL)
			delete m_currentPlayer;
		m_currentPlayer = new ImagePlayer(gpw, v);
		m_currentPlayer->show();
	}
#else
	Q_UNUSED(error)
#endif
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

	QQTotozViewer *v = new QQTotozViewer(totozId, NULL);
	QGraphicsProxyWidget *gpw = scene()->addWidget(v, Qt::Widget);
	moveToMousePos(gpw, v->size());

	if(m_currentPlayer != NULL)
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
	showWaitAnim();

	if(contentType.startsWith("video/") ||
			contentType.startsWith("image/"))
		m_downloader->getURL(url);
}

//////////////////////////////////////////////////////////////
/// \brief clearOverview
///
void QQPiniOverlay::clearOverview()
{
	if(m_currentPlayer != NULL)
	{
		delete m_currentPlayer;
		m_currentPlayer = NULL;
	}

	while(! m_tmpFiles.isEmpty())
	{
		QTemporaryFile *f = m_tmpFiles.takeFirst();

		if(f != NULL)
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

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QGraphicsVideoItem *i = new QGraphicsVideoItem();
	i->setAspectRatioMode(Qt::KeepAspectRatio);
	i->setSize(s);
	scene()->addItem(i);

	QMediaPlayer *player = new QMediaPlayer();
	player->setVideoOutput(i);
	player->setMuted(settings.value(SETTINGS_GENERAL_STEALTH_MODE, DEFAULT_GENERAL_STEALTH_MODE).toBool());

	QMediaPlaylist *l = new QMediaPlaylist();
	l->addMedia(url);
	l->setPlaybackMode(QMediaPlaylist::Loop);

	moveToMousePos(i, s);

	m_pendingPlayer = new VideoPlayer(i, player, l);
	if(m_currentPlayer != NULL)
		m_currentPlayer->hide();

	connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
			this, SLOT(doVideoStateChanged(QMediaPlayer::State)));

	connect(player, SIGNAL(error(QMediaPlayer::Error)),
			this, SLOT(handleVideoError(QMediaPlayer::Error)));

	player->setPlaylist(l);
	player->play();
#else
	clearOverview();
	Phonon::MediaObject *media = new Phonon::MediaObject();
	media->setCurrentSource(Phonon::MediaSource(url));

	Phonon::VideoPlayer *player = new Phonon::VideoPlayer(Phonon::VideoCategory);
	connect(media, SIGNAL(finished()), player, SLOT(play()));
	player->setFixedSize(s);
	player->audioOutput()->setMuted(settings.value(SETTINGS_GENERAL_STEALTH_MODE, DEFAULT_GENERAL_STEALTH_MODE).toBool());

	QGraphicsProxyWidget *gpw = scene()->addWidget(player, Qt::Widget);
	moveToMousePos(gpw, s);

	if(m_currentPlayer != NULL)
		delete m_currentPlayer;
	m_currentPlayer = new VideoPlayer(gpw, player, media);
	m_currentPlayer->show();

	player->play(media->currentSource());
#endif
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

	if(m_currentPlayer != NULL)
		delete m_currentPlayer;
	m_currentPlayer = new ImagePlayer(gpw, imgV);
	m_currentPlayer->show();

}
