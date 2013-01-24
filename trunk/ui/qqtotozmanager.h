#ifndef QQTOTOZMANAGER_H
#define QQTOTOZMANAGER_H

#include <QDockWidget>
#include <QPoint>
#include <QString>

namespace Ui {
class QQTotozManager;
}

class QQSettings;
class QQTotozDownloader;
class QQTMRequester;

class QQTotozManager : public QDockWidget
{
	Q_OBJECT

public:
	explicit QQTotozManager(QQSettings * settings, QWidget * parent = 0);
	~QQTotozManager();

	virtual void closeEvent(QCloseEvent * e);
	virtual void showEvent(QShowEvent *e);

public slots:
	void tabChanged(int tabIndex);
	void searchTotoz();
	void totozSearchFinished();
	void totozSearchCanceled();

signals:
	void totozClicked(QString anchor);

protected slots:
	void totozSelected(QString anchor);

private:
	Ui::QQTotozManager *ui;

	QQSettings * m_settings;
	QQTMRequester * m_requester;
	QQTotozDownloader * m_totozDownloader;

	bool m_positionInitialized;
	QPoint m_position;

	QString searchLineEditSS;
};

#endif // QQTOTOZMANAGER_H
