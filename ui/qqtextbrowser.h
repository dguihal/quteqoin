#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "qqpinipede.h"

#include <QTextEdit>

class QQTextBrowser : public QTextEdit
{
	Q_OBJECT

public:
    explicit QQTextBrowser(QString groupName, QQPinipede *parent = 0);
	~QQTextBrowser();

signals:
	void norlogeClicked(int rowNum);
	void loginClicked(int rowNums);

protected:
	void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	int mouseRowNum;
	int mouseColNum;

    bool mousePressed;

    QString m_groupName;

    QQPinipede *m_parent;
};

#endif // QQTEXTBROWSER_H
