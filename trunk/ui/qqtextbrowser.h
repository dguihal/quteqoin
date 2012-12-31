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

signals:
	void norlogeClicked(QQNorloge norloge);
	void norlogeRefHovered(QQNorlogeRef norloge);
	void unHighlight();
	void showTotozSig(QQTotoz & totoz);
	void hideTotozSig();
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
	void showTotoz(QQTotoz & totoz);
	void unHighlightNorloge();

	bool m_mouseClick;
	QPoint m_lastPoint;

	bool m_highlightAsked;
	QQNorlogeRef m_highlightedNRef;
	QString m_displayedTotozId;

	QString m_groupName;
};

#endif // QQTEXTBROWSER_H
