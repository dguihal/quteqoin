#ifndef QQTOTOZMANAGER_H
#define QQTOTOZMANAGER_H

#include "core/qqtotoz.h"

#include <QDockWidget>
#include <QString>

namespace Ui {
class QQTotozManager;
}

class QScrollArea;
class QQSettings;
class QQTotozDownloader;
class QQTMRequester;

class QQTotozManager : public QDockWidget
{
	Q_OBJECT

public:
	explicit QQTotozManager(QQSettings * settings, QWidget * parent = 0);
	~QQTotozManager();

public slots:
	void tabChanged(int tabIndex);
	void searchTotoz();
	void totozSearchFinished();
	void totozSearchCanceled();

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

	QQSettings * m_settings;
	QQTMRequester * m_requester;
	QQTotozDownloader * m_totozDownloader;
};

#endif // QQTOTOZMANAGER_H
