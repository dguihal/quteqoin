#include "qqnetworksettings.h"
#include "ui_qqnetworksettings.h"

QQNetworkSettings::QQNetworkSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQNetworkSettings)
{
	ui->setupUi(this);

	connect(ui->noProxyRB, SIGNAL(toggled(bool)), this, SLOT(onNetworkModeChanged()));
	connect(ui->systProxyRB, SIGNAL(toggled(bool)), this, SLOT(onNetworkModeChanged()));
	connect(ui->customProxyRB, SIGNAL(toggled(bool)), this, SLOT(onNetworkModeChanged()));
	connect(ui->proxyPortLE, SIGNAL(textChanged(QString)), this, SLOT(onProxyPortValuechanged(QString)));
}

QQNetworkSettings::~QQNetworkSettings()
{
	delete ui;
}

void QQNetworkSettings::setNetworkSettingsMode(const Mode &mode)
{
	switch(mode)
	{
	case DIRECT:
		ui->noProxyRB->setChecked(true);
		break;
	case SYSTEM:
		ui->systProxyRB->setChecked(true);
		break;
	default:
		ui->customProxyRB->setChecked(true);
		break;
	}
	onNetworkModeChanged();
}

QQNetworkSettings::Mode QQNetworkSettings::networkSettingsMode() const
{
	if(ui->noProxyRB->isChecked())
		return DIRECT;
	else if(ui->systProxyRB->isChecked())
		return SYSTEM;
	else //if(ui->customProxyRB->isChecked())
		return MANUAL;
}

void QQNetworkSettings::setProxySettings(const QString &host, const QString &port)
{
	ui->proxyHostLE->setText(host);
	ui->proxyPortLE->setText(port);
}

QString QQNetworkSettings::proxySettingsHost() const
{
	if(! ui->customProxyRB->isChecked())
		return QString();
	else
		return ui->proxyHostLE->text();
}

QString QQNetworkSettings::proxySettingsPort() const
{
	if(! ui->customProxyRB->isChecked())
		return QString();
	else
		return ui->proxyPortLE->text();
}

void QQNetworkSettings::setProxyIsHTTP(const bool &proxyIsHttp)
{
	if(proxyIsHttp)
		ui->proxyIsHTTPRB->setChecked(true);
	else
		ui->proxyIsSocks5RB->setChecked(true);
}

bool QQNetworkSettings::proxyIsHTTP() const
{
	return ui->proxyIsHTTPRB->isChecked();
}

void QQNetworkSettings::onNetworkModeChanged()
{
	if(ui->customProxyRB->isChecked())
	{
		ui->proxyHostLE->setEnabled(true);
		ui->proxyPortLE->setEnabled(true);
		ui->proxyIsHTTPRB->setEnabled(true);
		ui->proxyIsSocks5RB->setEnabled(true);
	}
	else
	{
		ui->proxyHostLE->setDisabled(true);
		ui->proxyPortLE->setDisabled(true);
		ui->proxyIsHTTPRB->setDisabled(true);
		ui->proxyIsSocks5RB->setDisabled(true);
	}
}

void QQNetworkSettings::onProxyPortValuechanged(QString text)
{
	bool isNumber;
	int portNum = text.toUInt(&isNumber);
	if(! isNumber)
		ui->proxyPortLE->setStyleSheet("QLineEdit{background: red;}");
	else if(portNum == 0 || portNum > 65535)
		ui->proxyPortLE->setStyleSheet("QLineEdit{background: red;}");
	else
		ui->proxyPortLE->setStyleSheet("");

}
