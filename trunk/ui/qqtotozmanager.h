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
	void tabChanged(int tabIndex);
	void searchTotoz();
	void totozSearchFinished();
	void totozSearchCanceled();
	void totozSearchEnabled(bool enabled);

signals:
	void totozClicked(QString anchor);

protected:
	virtual void focusInEvent(QFocusEvent *event);

protected slots:
	void totozSelected(QString anchor);
	void totozBookmarkDo(QString anchor, QQTotoz::TotozBookmarkAction action);
	void searchBookmarks(QString text);

private:
	void fillBookmarks();
	void createViewer(QScrollArea * dest, const QStringList & ids, QQTotoz::TotozBookmarkAction action);

	Ui::QQTotozManager *ui;

	QQTMRequester *m_requester;
	QQTotozDownloader *m_totozDownloader;
	QWidget *m_totozServerSearchWidget;
};

#endif // QQTOTOZMANAGER_H
