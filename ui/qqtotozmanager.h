#ifndef QQTOTOZMANAGER_H
#define QQTOTOZMANAGER_H

#include "core/qqtotoz.h"

#include <QDockWidget>
#include <QString>

namespace Ui {
class QQTotozManager;
}

class QQSettings;
class QQTotozDownloader;
class QQTMRequester;

class QScrollArea;


class QQTotozManager : public QDockWidget
{
	Q_OBJECT

public:
	explicit QQTotozManager(QWidget * parent = 0);
	~QQTotozManager();

public slots:
	virtual void setVisible(bool visible);
	void tabChanged(int tabIndex);
	void searchTotoz();
	void totozSearchFinished();
	void totozSearchCanceled();
	void totozSearchEnabled(bool enabled);

	static QStringList bookmarkedTotozIds();
	static void setBookmarkedTotozIds(QStringList newList);

signals:
	void totozClicked(QString anchor);

protected:
	virtual void focusInEvent(QFocusEvent *event);

protected slots:
	void emojiSelected();
	void totozSelected(QString anchor);
	void totozBookmarkDo(QString anchor, QQTotoz::TotozBookmarkAction action);
	void handleSearchTextChanged(QString text);

private:
	struct QQTmEmoji {
		QString charCode;
		QString description;
	};

	void fillBookmarks();
	void createTotozViewer(QScrollArea * dest, const QStringList & ids, QQTotoz::TotozBookmarkAction action);
	void createEmojiViewer(QScrollArea * dest, const QList<QQTmEmoji> &emojis);

	Ui::QQTotozManager *m_ui;

	QQTMRequester *m_requester;
	QQTotozDownloader *m_totozDownloader;
	QWidget *m_totozServerSearchWidget;
	QWidget *m_oldFocusWidget;

	static QStringList m_bookmarkListCache;
};

#endif // QQTOTOZMANAGER_H
