#include "qqbouchotsettingsdialog.h"
#include "ui_qqbouchotsettingsdialog.h"

#include "core/qqbouchot.h"

QQBouchotSettingsDialog::QQBouchotSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QQBouchotSettingsDialog)
{
    ui->setupUi(this);
	connect(ui->presetPushButton, SIGNAL(clicked()), this, SLOT(usePresetPressed()));
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okPressed()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));

	newBouchot = NULL;
	ui->presetComboBox->addItems(QQBouchot::getBouchotDefNameList());
}

QQBouchotSettingsDialog::~QQBouchotSettingsDialog()
{
    delete ui;
	if( newBouchot != NULL )
		delete newBouchot;
}

QQBouchot * QQBouchotSettingsDialog::getConfiguredBouchot()
{
	QQBouchot * ret = newBouchot;
	newBouchot = NULL;
	return ret;
}

void QQBouchotSettingsDialog::okPressed()
{
	if(newBouchot == NULL)
		newBouchot = new QQBouchot();
	newBouchot->setName(ui->nameLineEdit->text());
	newBouchot->setColor(ui->colorLineEdit->text());
	newBouchot->setAliasesFromString(ui->aliasLineEdit->text());
	newBouchot->setRefreshFromString(ui->refreshRateLineEdit->text());
	newBouchot->setLogin(ui->loginLineEdit->text());
	newBouchot->setUa(ui->uaLineEdit->text());
	newBouchot->setCookie(ui->cookieLineEdit->text());
	newBouchot->setPostUrl(ui->urlPostLineEdit->text());
	newBouchot->setBackendUrl(ui->urlBackendLineEdit->text());
	newBouchot->setPostData(ui->postDataLineEdit->text());
	newBouchot->setSlipType((QQBouchot::TypeSlip) ui->slipTypeComboBox->currentIndex());
}

void QQBouchotSettingsDialog::cancelPressed()
{
	if( newBouchot != NULL )
		delete newBouchot;
}

void QQBouchotSettingsDialog::usePresetPressed()
{
	newBouchot = QQBouchot::getBouchotDef(ui->presetComboBox->currentText());
	ui->nameLineEdit->setText(newBouchot->name());
	ui->colorLineEdit->setText(newBouchot->colorToString());
	ui->aliasLineEdit->setText(newBouchot->aliasesToString());
	ui->refreshRateLineEdit->setText(newBouchot->refreshToString());
	ui->cookieLineEdit->setText(newBouchot->cookie());
	ui->urlPostLineEdit->setText(newBouchot->postUrl());
	ui->urlBackendLineEdit->setText(newBouchot->backendUrl());
	ui->postDataLineEdit->setText(newBouchot->postData());
	ui->slipTypeComboBox->setCurrentIndex(newBouchot->slipType());
}
