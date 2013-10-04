#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
#include "core/qqtotoz.h"
#include "ui/qqpinipede.h"

#include <QTextBrowser>
#include <QPoint>

class QQMessageBlockUserData;
class QNetworkReply;
class QQPiniUrlHelper;

class QQTextBrowser : public QTextBrowser
{
	Q_OBJECT

public:
	explicit QQTextBrowser(QString groupName, QQPinipede *parent = 0);
	~QQTextBrowser();

	int notifAreaWidth();
	qreal timeUAAreaWidth() { return m_timeUAAreaWidthPx; }
	void notifAreaPaintEvent(QPaintEvent *event);

public slots:
	void updateNotifArea(int);

signals:
	void duckClicked(QString srcBouchot, QString postId);
	void shotDuck();
	void norlogeClicked(QString srcBouchot, QQNorloge norloge);
	void norlogeRefClicked(QString srcBouchot, QQNorlogeRef nRef);
	void norlogeRefHovered(QQNorlogeRef norloge);
	void unHighlight(QQTextBrowser *);
	void displayTotoz(QString &totozId);
	void concealTotoz();
	void displayTotozContextMenu(QPoint &pos);
	void loginClicked(QString bouchot, QString login);
	void newPostsAcknowledged(QString groupName);

protected:
	virtual void leaveEvent(QEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void contextMenuEvent(QContextMenuEvent *ev);

protected slots:
	void webSearchActionTriggered();
	void handleContentTypeAvailable(QUrl &url, QString &contentType);

private:
	void hideTotoz();
	void highlightNorloge(QQNorlogeRef nRef);
	void showTotoz(QString &totozId);
	void unHighlightNorloge();

	class QQNotifArea : public QWidget
	{
	public:
		QQNotifArea(QQTextBrowser *qqtb) : QWidget(qqtb) { m_qqtb = qqtb; }
		QSize sizeHint() const { return QSize(m_qqtb->notifAreaWidth(), 0); }

	protected:
		void paintEvent(QPaintEvent *event) { m_qqtb->notifAreaPaintEvent(event); }

	private:
		QQTextBrowser *m_qqtb;
	};
	QWidget *m_notifArea;

	QQPiniUrlHelper *m_urlHelper;

	bool m_mouseClick;
	QPoint m_lastPoint;

	qreal m_timeUAAreaWidthPx;

	QString m_highlightedNorlogeRef;
	QString m_displayedTotozId;

	QString m_groupName;
};

#endif // QQTEXTBROWSER_H
