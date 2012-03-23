#ifndef QQSYNTAXHIGHLIGHTER_H
#define QQSYNTAXHIGHLIGHTER_H

#include "ui/qqtextbrowser.h"

#include <QString>
#include <QSyntaxHighlighter>

class QQSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    QQSyntaxHighlighter(QQTextBrowser *parent = 0);

signals:

public slots:

protected:
    virtual void highlightBlock(const QString &text);

private:
    void highlightDuck(const QString &text);
    void highlightNorloge(const QString &text);
    void highlightTableVolante(const QString &text);
    void highlightTotoz(const QString &text);
};

#endif // QQSYNTAXHIGHLIGHTER_H
