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
class QQTextBrowser;
class QQTotozManager;
class QQPiniSearchWidget;
class QQPost;
class QQPostDisplayFilter;
class QQPostParser;
class QQTotozDownloader;
class QQTotozViewer;
class QQPiniOverlay;

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
	explicit QQPinipede(QWidget *parent = nullptr);
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
	void norlogeClicked(QString srcBouchot, QQNorloge norloge);
	void norlogeRefClicked(QString srcBouchot, QQNorlogeRef nRef);
	void norlogeRefHovered(QQNorlogeRef norlogeRef);
	void unHighlight(QQTextBrowser *tBrowser);
	void loginClicked(QString bouchot, QString login);
	void tabEventsAcknowledged(const QString& groupName);
	void bigorNotify(QString &srcBouchot, QString &poster, bool global);
	void searchText(const QString &text, bool forward = true);

signals:
	void insertTextPalmi(QString bouchot, QString text);
	void boardGroupEventsAcknowledged(const QString& boardName);

protected:
	virtual void resizeEvent(QResizeEvent *event);

private:
	bool applyPostDisplayFilters(QQPost *post);
	QColor getDynHighlightColor(const QColor &bgColor);
	unsigned int insertPostToList(QQListPostPtr *listPosts, QQPost *post, unsigned int indexStart);
	void newPostsAvailable(QString groupName);
	bool printPostAtCursor(QTextCursor &cursor, QQPost *post);
	void updatePiniDisplaySettings(QTextDocument *doc);

	QQTotozDownloader *m_totozDownloader;
	QQTotozManager *m_totozManager;
	QLabel *m_hiddenPostViewerLabel;
	QBuffer m_totozData;
	QString m_hiddenPostViewerLabelSSheet;

	QHash<QString, QQListPostPtr *> m_listPostsTabMap;
	QHash<QString, QQTextBrowser *> m_textBrowserHash;
	QList<QQPostDisplayFilter *> m_listpostDisplayFilters;

	QQPostParser *m_postparser;

	QMutex m_newPostsAvailableMutex;

	QQPiniOverlay *m_overlay;

	bool m_duckAutolaunchEnabled;
	QChar m_fieldSep;
	int m_maxHistorySize;
	bool m_stealthModeEnabled;

	/**
	 * @brief ascii_login when true, the displayed login/ua replace non ascii characters by '-'
	 */
	bool asciiLogin;
};

#endif // QQPINIPEDE_H
