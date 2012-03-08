#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    QQPalmipede * getPalmi() {return palmi;}

public slots:
    void displayOptions();
    void doPostMessage(const QString & bouchot, const QString & message);

protected:
    void closeEvent(QCloseEvent *event);

private:

    Ui::MainWindow *ui;
    QQPalmipede *palmi;
    QQPinipede *pini;
    QQSettings *settings;
};

#endif // MAINWINDOW_H
