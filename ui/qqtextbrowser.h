#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "ui/qqpinipede.h"

#include <QTextEdit>

class QQTextBrowser : public QTextEdit
{
    Q_OBJECT

public:
    explicit QQTextBrowser(QString groupName, QQPinipede *parent = 0);
    ~QQTextBrowser();

    QString bouchotUnderCursor();
    QString messageUnderCursor();

signals:
    void norlogeClicked(QQNorloge norloge);
    void loginClicked(QString groupName);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    int mouseRowNum;
    int mouseColNum;

    bool mousePressed;

    QString m_groupName;

    QString m_currBouchot;
    QString m_message;

    QQPinipede *m_parent;
};

#endif // QQTEXTBROWSER_H
