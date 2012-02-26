#ifndef QQPINIPEDE_H
#define QQPINIPEDE_H

#include "core/qqnorloge.h"

#include <QHash>
#include <QRegExp>
#include <QString>
#include <QTabWidget>
#include <QTextFormat>

class QQBouchot;
class QQTextBrowser;
class QTextCursor;
class QTextFrame;
class QQPost;

class QTextBrowser;
class QTextDocument;

class QQPinipede : public QTabWidget
{
    Q_OBJECT

public:
    explicit QQPinipede(QWidget *parent = 0);
    ~QQPinipede();

    void addPiniTab(const QString &);
    void createPiniTabs(const QList<QString> &);
    void removePiniTab(const QString &);

    void printPostsAtEnd(QList<QQPost *> &);
    void printPostAt(QQPost *, int);

    QQPost * getPostForGroup(QString &groupName, int numPost);

public slots:
    void norlogeClicked(QQNorloge norloge);
    void loginClicked(QString);
    void newPostsAvailable( QQBouchot * );

signals:
    void insertTextPalmi(QString text);

private:
    void createQTextTableRows( QQTextBrowser*, int, int );
    void printPostAtCursor( QTextCursor &, QQPost * );
    unsigned int insertPostToList(QList<QQPost *> *, QQPost *, unsigned int);

    QHash<QString, QList<QQPost *> *> m_listPostsTabMap;

    QTextDocument * document(const QString &);
    int getNextDuck(QString &);
    int getNextNorloge(QString &);
    int getNextTotoz(QString &);

    QHash<QString, QQTextBrowser *> m_textBrowserHash;

    int norlogeMatchLength;
    int duckMatchLength;
    int totozMatchLength;
};

#endif // QQPINIPEDE_H
