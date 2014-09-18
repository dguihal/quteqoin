#ifndef QQPINIPEDE_H
#define QQPINIPEDE_H

#include "ui/qqhuntingview.h"

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
class QQPiniSearchWidget;
class QQPost;
class QQPostDisplayFilter;
class QQPostParser;
class QQTotozDownloader;
class QQTotozViewer;
class QQWebImageViewer;

class QLabel;
class QLineEdit;
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

	void setToolButton(QWidget *toolButton);

	void addPiniTab(const QString &groupName);
	void clearPiniTab(const QString &groupName);
	void removePiniTab(const QString &groupName);
	void repaintPiniTab(const QString &groupName);

	void purgePiniTab(const QString &groupName, const QString &bouchotName);
	void purgePinitab(const QString &groupName, const QString &bouchotName, unsigned int max);

	QQPost *getPostForGroup(QString &groupName, int numPost);

	void setTotozManager(QQTotozManager *ttManager);

	void purgePinitabHistory(const QString &groupName);

	virtual bool event(QEvent *e);

public slots:
	void bouchotVisibilityChanged(QString board);
	void duckKilled(QString board,QString postId);
	void norlogeClicked(QString srcBoard, QQNorloge norloge);
	void norlogeRefClicked(QString srcBoard, QQNorlogeRef nRef);
	void norlogeRefHovered(QQNorlogeRef norlogeRef);
	void unHighlight(QQTextBrowser *tBrowser);
	void showWebImageViewer(const QUrl &url);
	void showTotozViewer(const QString &totozId);
	void hideViewers();
	void loginClicked(QString board, QString login);
	void tabEventsAcknowledged(const QString& groupName);
	void bigorNotify(QString &srcBoard, QString &poster, bool global);
	void searchText(const QString &text, bool forward = true);

signals:
	void insertTextPalmi(QString bouchot, QString text);
	void boardGroupEventsAcknowledged(const QString& boardName);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *ev);
	virtual void resizeEvent(QResizeEvent *event);

private:
	bool applyPostDisplayFilters(QQPost *post);
	unsigned int insertPostToList(QQListPostPtr *listPosts, QQPost *post, unsigned int indexStart);
	void newPostsAvailable(QString groupName);
	bool printPostAtCursor(QTextCursor &cursor, QQPost *post);
	QColor getDynHighlightColor(const QColor &bgColor);

	QQTotozDownloader *m_totozDownloader;
	QQTotozManager *m_totozManager;
	QQTotozViewer *m_totozViewer;
	QQWebImageViewer *m_webImageViewer;
	QLabel *m_hiddenPostViewerLabel;
	QBuffer m_totozData;
	QString m_hiddenPostViewerLabelSSheet;

	QHash<QString, QQListPostPtr *> m_listPostsTabMap;
	QHash<QString, QQTextBrowser *> m_textBrowserHash;
	QList<QQPostDisplayFilter *> m_listpostDisplayFilters;

	QQPostParser *m_postparser;

	QMutex newPostsAvailableMutex;

	QQHuntingView *m_huntingView;
	bool m_duckAutolaunchEnabled;
};

#endif // QQPINIPEDE_H