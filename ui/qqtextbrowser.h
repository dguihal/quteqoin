#ifndef QQTEXTBROWSER_H
#define QQTEXTBROWSER_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
#include "ui/qqpinipede.h"

#include <QTextEdit>

class QQTextBrowser : public QTextEdit
{
    Q_OBJECT

public:
    explicit QQTextBrowser(QString groupName, QQPinipede *parent = 0);
    ~QQTextBrowser();

signals:
    void norlogeClicked(QQNorloge norloge);
    void norlogeRefHovered(QQNorlogeRef norloge);
    void loginClicked(QString groupName);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    bool mousePressed;

    QString m_groupName;

    QQPinipede *m_parent;
};

#endif // QQTEXTBROWSER_H
