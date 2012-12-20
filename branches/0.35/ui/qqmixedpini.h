#ifndef QQMIXEDPINI_H
#define QQMIXEDPINI_H

#include <QScrollArea>

namespace Ui {
    class QQMixedPini;
}

class QQMixedPini : public QScrollArea
{
    Q_OBJECT

public:
    explicit QQMixedPini(QWidget *parent = 0);
    ~QQMixedPini();

private:
    Ui::QQMixedPini *ui;
};

#endif // QQMIXEDPINI_H
