#include "qqmonopini.h"
#include "ui_qqmonopini.h"

QQMonoPini::QQMonoPini(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QQMonoPini)
{
    ui->setupUi(this);
}

QQMonoPini::~QQMonoPini()
{
    delete ui;
}
