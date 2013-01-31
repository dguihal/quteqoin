#ifndef QQTMLABEL_H
#define QQTMLABEL_H

#include <QBuffer>
#include <QLabel>
#include <QString>

class QQTotozDownloader;

class QMovie;

class QQTotozViewer : public QLabel
{
	Q_OBJECT

public:
	QQTotozViewer(QWidget * parent = 0) : QLabel(parent) { init(""); }
	QQTotozViewer(const QString & totozId = "", QWidget * parent = 0) : QLabel(parent) { init(totozId); }
	~QQTotozViewer();

	void enableBookmarksMenu(const bool enableBMenu = true) { m_bookmarkMenuEnabled = enableBMenu; }
	void setTotozId(const QString & totozId);
	void setTotozDownloader(QQTotozDownloader * downloader) { if(downloader != NULL) m_downloader = downloader; }

public slots:
	void totozAvailable(QString & totozId, bool success);

signals:
	void totozClicked(QString anchor);
	void bookmarkingAsked(QString anchor);

protected:
	void updateImg();
	virtual void mousePressEvent(QMouseEvent * ev);
	virtual void mouseReleaseEvent(QMouseEvent * ev);
	virtual void enterEvent(QEvent * event);
	virtual void leaveEvent(QEvent * event);
	virtual void contextMenuEvent(QContextMenuEvent * ev);

private:
	void displayText(QString text);
	void displayMovie();
	void displayImage(QImage & image);
	QString getAnchor();
	void init(const QString & totozId);

	QString m_totozId;

	QMovie * m_totozMovie;
	QBuffer * m_totozDataBuffer;

	QQTotozDownloader * m_downloader;

	bool m_mousePressOK;
	bool m_bookmarkMenuEnabled;
	bool m_hasfailed;
};

#endif // QQTOTOTZMANAGER_H
