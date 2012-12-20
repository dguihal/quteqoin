#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QQBouchot;
class QQPalmipede;
class QQPinipede;
class QQSettings;

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
	void doPostMessage(const QString & bouchot, const QString & message);
	void doTriggerMiniPalmi();
	void doTriggerMaxiPalmi();

protected:
	virtual void closeEvent(QCloseEvent * event);
	virtual void resizeEvent(QResizeEvent * event);


private:
	void initBouchot(QQBouchot * bouchot);

	Ui::MainWindow *m_ui;
	QQPalmipede *m_palmi;
	QQPinipede *m_pini;
	QQSettings *m_settings;
};

#endif // MAINWINDOW_H
