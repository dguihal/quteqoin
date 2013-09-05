#ifndef QQTMLABEL_H
#define QQTMLABEL_H

#include "core/qqtotoz.h"

#include <QBuffer>
#include <QLabel>
#include <QMovie>
#include <QString>

class QQTotozDownloader;

class QMovie;

class QQTotozViewer : public QLabel
{
	Q_OBJECT

public:
	QQTotozViewer(QWidget *parent = 0) : QLabel(parent) { init(""); }
	QQTotozViewer(const QString &totozId = "", QWidget *parent = 0) : QLabel(parent) { init(totozId); }
	~QQTotozViewer();

	void enableBookmarksAdd(const bool enableBmAdd = true) { m_bookmarkAddEnabled = enableBmAdd; }
	void enableBookmarksRem(const bool enableBmRem = true) { m_bookmarkRemEnabled = enableBmRem; }
	void setTotozId(const QString &totozId);
	void setTotozDownloader(QQTotozDownloader *downloader) { if(downloader != NULL) m_downloader = downloader; }

public slots:
	void displayContextMenu(QPoint &pos);
	void totozAvailable(QString &totozId, bool success, QString &errMsg);

signals:
	void totozClicked(QString anchor);
	void totozBookmarkAct(QString anchor, QQTotoz::TotozBookmarkAction);

protected:
	void updateImg();
	virtual void contextMenuEvent(QContextMenuEvent *ev);
	virtual void enterEvent(QEvent *event);
	virtual void hideEvent(QHideEvent *event);
	virtual void leaveEvent(QEvent *event);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
	virtual void showEvent(QShowEvent *event);

private:
	void displayText(QString text);
	void displayMovie();
	void displayImage(QImage &image);
	QString getAnchor();
	void init(const QString &totozId);
	void setPos();

	QString m_totozId;

	QMovie m_totozMovie;
	QBuffer m_totozDataBuffer;

	QQTotozDownloader *m_downloader;

	bool m_mousePressOK;
	bool m_bookmarkAddEnabled;
	bool m_bookmarkRemEnabled;
	bool m_hasfailed;
};

#endif // QQTOTOTZMANAGER_H
