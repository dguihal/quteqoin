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
class QQTotozManager;
class QTextCursor;
class QQPost;
class QQPostParser;
class QQTotozDownloader;
class QQTotozViewer;

class QLabel;
class QMovie;
class QTextBrowser;
class QTextDocument;
class QToolButton;

class QQPinipede : public QTabWidget
{
	Q_OBJECT

public:
	explicit QQPinipede(QWidget *parent = 0);
	~QQPinipede();

	void setToolButton(QToolButton *toolButton);

	void addPiniTab(const QString &);
	void createPiniTabs(const QList<QString> &groups);
	void removePiniTab(const QString &);

	void purgePiniTab(const QString &groupName, const QString &bouchotName);
	void purgePinitab(const QString &groupName, const QString &bouchotName, unsigned int max);

	QQPost *getPostForGroup(QString &groupName, int numPost);

	void setTotozManager(QQTotozManager *ttManager);

	void purgePinitabHistory(const QString &groupName);

	virtual bool event(QEvent *e);

public slots:
	void norlogeClicked(QString srcBouchot, QQNorloge norloge);
	void norlogeRefClicked(QString srcBouchot, QQNorlogeRef nRef);
	void norlogeRefHovered(QQNorlogeRef norlogeRef);
	void unHighlight(QQTextBrowser *tBrowser);
	void showTotozViewer(QString &totozId);
	void hideTotozViewer();
	void loginClicked(QString bouchot, QString login);
	void newPostsAcknowledged(QString groupName);
	void notify();

signals:
	void insertTextPalmi(QString bouchot, QString text);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *ev);

private:
	bool applyPostDisplayFilters(QQPost *post);
	unsigned int insertPostToList(QList<QQPost *> *listPosts, QQPost *post, unsigned int indexStart);
	void newPostsAvailable(QString groupName);
	bool printPostAtCursor(QTextCursor &cursor, QQPost *post);

	QQTotozDownloader *m_totozDownloader;
	QQTotozManager *m_totozManager;
	QQTotozViewer *m_totozViewer;
	QLabel *m_hiddenPostViewerLabel;
	QBuffer m_totozData;
	QString m_hiddenPostViewerLabelSSheet;

	QHash<QString, QList<QQPost *> *> m_listPostsTabMap;
	QHash<QString, QQTextBrowser *> m_textBrowserHash;

	QQPostParser *m_postparser;

	QMutex newPostsAvailableMutex;

	int norlogeMatchLength;
	int duckMatchLength;
	int totozMatchLength;

};

#endif // QQPINIPEDE_H
