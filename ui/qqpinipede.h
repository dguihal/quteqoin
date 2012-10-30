#ifndef QQPINIPEDE_H
#define QQPINIPEDE_H

#include "core/qqnorloge.h"
#include "core/qqnorlogeref.h"
#include "core/qqtotoz.h"

#include <QBuffer>
#include <QHash>
#include <QMutex>
#include <QPointer>
#include <QRegExp>
#include <QString>
#include <QTabWidget>
#include <QTextFormat>

class QQBouchot;
class QQSyntaxHighlighter;
class QQSettings;
class QQTextBrowser;
class QTextCursor;
class QTextTable;
class QQPost;
class QQTotozManager;

class QLabel;
class QMovie;
class QTextBrowser;
class QTextDocument;

class QQPinipede : public QTabWidget
{
	Q_OBJECT

public:
	explicit QQPinipede(QQSettings * settings, QWidget *parent = 0);
	~QQPinipede();

	void addPiniTab(const QString &);
	void createPiniTabs(const QList<QString> &groups);
	void removePiniTab(const QString &);

	void purgePiniTab(const QString &groupName, const QString &bouchotName);
	void purgePinitab(const QString &groupName, const QString &bouchotName, unsigned int max);

	QQPost * getPostForGroup(QString &groupName, int numPost);

public slots:
	void norlogeClicked(QQNorloge norloge);
	void norlogeRefHovered(QQNorlogeRef norlogeRef);
	void unHighlight();
	void showTotozSlot(QQTotoz & totoz);
	void hideTotozSlot();
	void loginClicked(QString);
	void newPostsAvailable( QQBouchot * );

signals:
	void insertTextPalmi(QString text);

private:
	void createQTextTableRows( QQTextBrowser*, int, int );
	void printPostAtCursor( QTextCursor &, QQPost * );
	unsigned int insertPostToList(QList<QQPost *> *, QQPost *, unsigned int);
	void createQTextTable( QQTextBrowser* textBrowser, int numRow );

	QQSettings * m_settings;
	QQTotozManager * m_totozManager;
	QQTextBrowser * m_tBrowserHighlighted;
	QLabel * m_hiddenPostViewerLabel;
	QLabel * m_totozLabel;
	QMovie * m_totozMovie;
	QBuffer m_totozData;
	QString m_hiddenPostViewerLabelSSheet;

	QHash<QString, QList<QQPost *> *> m_listPostsTabMap;
	QHash<QString, QQTextBrowser *> m_textBrowserHash;

	QMutex newPostsAvailableMutex;

	int norlogeMatchLength;
	int duckMatchLength;
	int totozMatchLength;

};

#endif // QQPINIPEDE_H