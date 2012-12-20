#include "qqmixedpini.h"
#include "ui_qqmixedpini.h"

QQMixedPini::QQMixedPini(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::QQMixedPini)
{
    ui->setupUi(this);
}

QQMixedPini::~QQMixedPini()
{
    delete ui;
}
