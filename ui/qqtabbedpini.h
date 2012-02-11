#ifndef QQTABBEDPINI_H
#define QQTABBEDPINI_H

#include <QTabWidget>

namespace Ui {
    class QQTabbedPini;
}

class QQTabbedPini : public QTabWidget
{
    Q_OBJECT

public:
    explicit QQTabbedPini(QWidget *parent = 0);
    ~QQTabbedPini();

private:
    Ui::QQTabbedPini *ui;
	QWidget *realPini;
};

#endif // QQTABBEDPINI_H
