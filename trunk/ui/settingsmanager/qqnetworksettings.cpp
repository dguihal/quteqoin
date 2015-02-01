#include "qqnetworksettings.h"
#include "ui_qqnetworksettings.h"

QQNetworkSettings::QQNetworkSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QQNetworkSettings)
{
	ui->setupUi(this);
}

QQNetworkSettings::~QQNetworkSettings()
{
	delete ui;
}
