#ifndef QQTMLABEL_H
#define QQTMLABEL_H

#include "core/qqtotoz.h"
#include "qqimageviewer.h"

#include <QString>

class QQTotozDownloader;

class QMovie;

class QQTotozViewer : public QQImageViewer
{
	Q_OBJECT

public:
	QQTotozViewer(QWidget *parent = 0) : QQImageViewer(parent) { init(""); }
	QQTotozViewer(const QString &totozId = "", QWidget *parent = 0) : QQImageViewer(parent) { init(totozId); }
	~QQTotozViewer();

	void enableBookmarksAdd(const bool enableBmAdd = true) { m_bookmarkAddEnabled = enableBmAdd; }
	void enableBookmarksRem(const bool enableBmRem = true) { m_bookmarkRemEnabled = enableBmRem; }
	void setTotozId(const QString &totozId);
	void setTotozDownloader(QQTotozDownloader *downloader) { if(downloader != NULL) m_downloader = downloader; }

public slots:
	void displayContextMenu(QPoint &pos);

signals:
	void totozClicked(QString anchor);
	void totozBookmarkAct(QString anchor, QQTotoz::TotozBookmarkAction);

protected:
	void updateTotoz();
	virtual void contextMenuEvent(QContextMenuEvent *ev);
	virtual void enterEvent(QEnterEvent *event);
	virtual void leaveEvent(QEvent *event);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);

protected slots:
	void totozAvailable(QString &totozId, bool success, QString &errMsg);

private:
	QString getAnchor();
	void handleInvalidTotozData();
	void init(const QString &totozId);

	QString m_totozId;

	QQTotozDownloader *m_downloader;

	bool m_mousePressOK;
	bool m_bookmarkAddEnabled;
	bool m_bookmarkRemEnabled;
	bool m_hasfailed;
};

#endif // QQTOTOTZMANAGER_H
