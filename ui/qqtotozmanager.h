#ifndef QQTOTOZMANAGER_H
#define QQTOTOZMANAGER_H

#include "core/qqtotoz.h"
#include "core/qqsettings.h"

#include <QDockWidget>
#include <QString>

namespace Ui {
class QQTotozManager;
}

class QLabel;
class QQSettings;
class QQTimer;
class QQTotozDownloader;
class QQTMRequester;

class QScrollArea;
class QWidgetItem;


class QQTotozManager : public QDockWidget
{
	Q_OBJECT

public:
	explicit QQTotozManager(QWidget * parent = 0);
	~QQTotozManager();

public slots:
	virtual void setVisible(bool visible);
	void tabChanged(int tabIndex);
	void totozSearchFinished();
	void totozSearchCanceled();
	void totozSearchEnabled(bool enabled);

	static QStringList bookmarkedTotozIds();
	static void setBookmarkedTotozIds(QStringList newList);

signals:
	void totozClicked(QString anchor);

protected:
	virtual void focusInEvent(QFocusEvent *event);
	virtual void showEvent(QShowEvent *ev);

protected slots:
	void emojiSelected();
	void handleSearchTextChanged(QString text);
	void searchTotoz();
	void totozBookmarkDo(QString anchor, QQTotoz::TotozBookmarkAction action);

private:
	void updateTotozViewer();
	void updateEmojiViewer(const QList<QQEmojiDef> &emojis);

	Ui::QQTotozManager *m_ui;

	QQTMRequester *m_requester;
	QQTotozDownloader *m_totozDownloader;
	QWidget *m_oldFocusWidget;
	bool m_totozSearchEnabled;
	QTimer *m_searchQueryTemperer;

	static QStringList m_tTZBookmarkListCache;
	QStringList m_filteredTTZBookmarkList;
	QStringList m_searchResultList;
	QList<QQEmojiCat> emojis;

	QLabel *m_bookmarkHeaderW;
	QWidget *m_bookmarkW;
	QLabel *m_searchHeaderW;
	QWidget *m_searchW;
	QList<QWidgetItem *> m_displayedItems;
};

#endif // QQTOTOZMANAGER_H
