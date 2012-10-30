#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
#include "core/qqtotoz.h"
#include "ui/qqpinipede.h"

#include <QTextEdit>

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
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent * event);

private:
	void highlightNorloge(QQNorlogeRef nRef);
	void unHighlightNorloge();
	void showTotoz(QQTotoz & totoz);
	void hideTotoz();

	bool mousePressed;

	bool m_highlightAsked;
	QQNorlogeRef m_highlightedNRef;
	QString m_displayedTotozId;

	QString m_groupName;
};

#endif // QQTEXTBROWSER_H