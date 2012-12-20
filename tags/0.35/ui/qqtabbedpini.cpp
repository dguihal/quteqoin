#include "qqtabbedpini.h"
#include "ui_qqtabbedpini.h"

QQTabbedPini::QQTabbedPini(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::QQTabbedPini)
{
    ui->setupUi(this);
}

QQTabbedPini::~QQTabbedPini()
{
    delete ui;
}
