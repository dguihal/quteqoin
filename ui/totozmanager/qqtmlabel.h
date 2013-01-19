#ifndef QQTMLABEL_H
#define QQTMLABEL_H

#include <QLabel>
#include <QString>

class QTimer;

class QQTMLabel : public QLabel
{
	Q_OBJECT

public:
	explicit QQTMLabel(const QString & totozId ="", QWidget * parent = 0 );
	~QQTMLabel();

	void enableBookmarksMenu(const bool enableBMenu = true) { m_bookmarkMenuEnabled = enableBMenu; }

public slots:
	void update();

signals:
	void totozClicked(QString anchor);
	void bookmarkingAsked(QString anchor);

protected:
	virtual void mousePressEvent(QMouseEvent * ev);
	virtual void mouseReleaseEvent(QMouseEvent * ev);
	virtual void enterEvent(QEvent * event);
	virtual void leaveEvent(QEvent * event);
	virtual void contextMenuEvent(QContextMenuEvent * ev);

private:
	void displayMovie(QMovie * movie);
	void displayImage(QImage & image);
	QString getAnchor();

	QTimer * m_timer;
	QString m_totozId;

	QMovie * m_totozMovie;
	bool loadFailed;

	bool m_mousePressOK;
	bool m_bookmarkMenuEnabled;
};

#endif // QQTOTOTZMANAGER_H
