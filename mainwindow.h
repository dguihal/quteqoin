#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSystemTrayIcon;

class QQBoardsInfo;
class QQBouchot;
class QQPalmipede;
class QQPinipede;
class QQPiniSearchWidget;
class QQSettings;
class QQTotozManager;


namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QQPalmipede * getPalmi() {return m_palmi;}

public slots:
	void displayOptions();
	void doPostMessage(const QString &bouchot, const QString &message);
	void doTriggerMaxiPalmi();
	void doTriggerMiniPalmi();
	void palmiVisibilityChanged(bool visible);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void closeEvent(QCloseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

protected slots:
	void bouchotDestroyed(QQBouchot *bouchot);
	void bouchotGroupChanged(QQBouchot *bouchot, QString oldGroupName);
	void doFullRepaint();
	void initBouchot(QQBouchot *bouchot);

private:
	void initBouchots();
	void minimizePalmi(bool isPalmiMini);

	Ui::MainWindow *m_ui;
	QQBoardsInfo *m_boardsInfo;
	QQPalmipede *m_palmi;
	QQPinipede *m_pini;
	QQPiniSearchWidget *m_pSearchW;
	QQTotozManager *m_totozManager;

	QSystemTrayIcon *m_trayIcon;
};

#endif // MAINWINDOW_H
