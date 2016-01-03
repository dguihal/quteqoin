#ifndef QQPINIOVERLAY_H
#define QQPINIOVERLAY_H

#include <QGraphicsView>
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QMimeType>
#include <QMediaPlayer>
#else
class QMediaPlayer
{
public:
	enum State { VOID_S };
	enum Error { VOID_E };
};
#endif
#include <QUrl>

class OverlayPlayer;

class QQDuckPixmapItem;
class QQImageViewer;
class QQTotozViewer;
class QQWebDownloader;
class QQWebImageViewer;

class QGraphicsObject;
class QGraphicsVideoItem;
class QMediaPlaylist;
class QTemporaryFile;

class QQPiniOverlay : public QGraphicsView
{
	Q_OBJECT
public:
	explicit QQPiniOverlay(QWidget *parent = 0);
	~QQPiniOverlay();

signals:
	void duckKilled(QString srcBouchot, QString postId);

public slots:
	void launchDuck(QString srcBouchot, QString postId, bool selfDuck);
	void killDuck(bool forceSilent);

	void showTotoz(const QString &totozId);
	void showUrl(const QUrl &url, QString &contentType);
	void clearOverview();

protected:
	virtual void resizeEvent(QResizeEvent *event);

protected slots:
	void dlReady();
	void doVideoStateChanged(QMediaPlayer::State newState);
	void handleVideoError(QMediaPlayer::Error error);

private:
	void moveToMousePos(QGraphicsObject *gpw, const QSize &s);
	void showVideo(const QUrl &url);
	void showWaitAnim();

	QQWebDownloader *m_downloader;
	QGraphicsPixmapItem *m_img;
	QList<QQDuckPixmapItem *> m_duckList;
	QList<QTemporaryFile *> m_tmpFiles;

	OverlayPlayer *m_currentPlayer;
	OverlayPlayer *m_pendingPlayer;

};

#endif // QQPINIOVERLAY_H
