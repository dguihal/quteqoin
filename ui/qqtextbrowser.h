#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
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
	void loginClicked(QString groupName);

protected:
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent * event);

private:
	bool highlightNorloge(QQMessageBlockUserData * blockData, QQNorlogeRef nRef);

	bool mousePressed;

	bool m_highlightAsked;
	QQMessageBlockUserData * m_highlightedBlockUserData;
	QQNorlogeRef m_highlightedNRef;

	QString m_groupName;

	QQPinipede *m_parent;
};

#endif // QQTEXTBROWSER_H
