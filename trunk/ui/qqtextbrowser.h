#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
#include "core/qqtotoz.h"
#include "ui/qqpinipede.h"

#include <QPoint>
#include <QTextBrowser>
#include <QUrl>

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
	void duckClicked(QString board, QString postId, bool isSelfPost);
	void shotDuck(bool forceSilent);
	void norlogeClicked(QString board, QQNorloge norloge);
	void norlogeRefClicked(QString board, QQNorlogeRef nRef);
	void norlogeRefHovered(QQNorlogeRef norloge);
	void unHighlight(QQTextBrowser *);
	void displayTotoz(const QString &totozId);
	void displayWebImage(const QUrl &url);
	void hideViewers();
	void displayTotozContextMenu(QPoint &pos);
	void loginClicked(QString board, QString login);
	void newPostsAcknowledged(QString groupName);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *ev);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void wheelEvent(QWheelEvent *event);

protected slots:
	void onAnchorClicked(const QUrl &link);
	void onAnchorHighlighted(const QUrl &link);
	void onContentTypeAvailable(QUrl &url, QString &contentType);
	void webSearchActionTriggered();

private:
	void clearViewers();
	bool displayPreview(const QUrl &url);
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

	qreal m_timeUAAreaWidthPx;

	QString m_highlightedNorlogeRef;
	QString m_displayedTotozId;
	QUrl m_shownUrl;

	QString m_groupName;
};

#endif // QQTEXTBROWSER_H
