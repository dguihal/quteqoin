#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
#include "core/qqtotoz.h"
#include "ui/qqpinipede.h"

#include <QTextEdit>
#include <QPoint>

class QQMessageBlockUserData;

class QQTextBrowser : public QTextEdit
{
	Q_OBJECT

public:
	explicit QQTextBrowser(QString groupName, QQPinipede *parent = 0);
	~QQTextBrowser();

	int notifAreaWidth();
	void notifAreaPaintEvent(QPaintEvent * event);

public slots:
	void updateNotifArea(int);

signals:
	void norlogeClicked(QQNorloge norloge);
	void norlogeRefHovered(QQNorlogeRef norloge);
	void unHighlight();
	void displayTotoz(QString & totozId);
	void concealTotoz();
	void loginClicked(QString groupName);

protected:
	void leaveEvent(QEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent * event);



private:

	void hideTotoz();
	void highlightNorloge(QQNorlogeRef nRef);
	void showTotoz(QString & totozId);
	void unHighlightNorloge();

	class QQNotifArea : public QWidget
	{
	public:
		QQNotifArea(QQTextBrowser * qqtb) : QWidget(qqtb) { m_qqtb = qqtb; }
		QSize sizeHint() const { return QSize(m_qqtb->notifAreaWidth(), 0); }

	protected:
		void paintEvent(QPaintEvent * event) { m_qqtb->notifAreaPaintEvent(event); }

	private:
		QQTextBrowser * m_qqtb;
	};
	QWidget * m_notifArea;

	bool m_mouseClick;
	QPoint m_lastPoint;

	bool m_highlightAsked;
	QQNorlogeRef m_highlightedNRef;
	QString m_displayedTotozId;

	QString m_groupName;
};

#endif // QQTEXTBROWSER_H
