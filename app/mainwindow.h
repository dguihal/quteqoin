#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QQBouchot;
class QQPalmipede;
class QQPinipede;
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
	void totozManagerVisibilityChanged(bool visible);

protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

protected slots:
	void bouchotDestroyed(QQBouchot *bouchot);
	void bouchotGroupChanged(QQBouchot *bouchot, QString oldGroupName);
	void initBouchot(QQBouchot *bouchot);

private:
	void initBouchots();
	void minimizePalmi(bool isPalmiMini);

	Ui::MainWindow *m_ui;
	QQPalmipede *m_palmi;
	QQPinipede *m_pini;
	QQTotozManager *m_totozManager;
};

#endif // MAINWINDOW_H
