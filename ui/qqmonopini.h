#ifndef QQMONOPINI_H
#define QQMONOPINI_H

#include <QWidget>

namespace Ui {
    class QQMonoPini;
}

class QQMonoPini : public QWidget
{
    Q_OBJECT

public:
    explicit QQMonoPini(QWidget *parent = 0);
    ~QQMonoPini();

private:
    Ui::QQMonoPini *ui;
};

#endif // QQMONOPINI_H
