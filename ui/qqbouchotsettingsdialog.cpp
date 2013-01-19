#include "qqbouchotsettingsdialog.h"
#include "ui_qqbouchotsettingsdialog.h"

#include <QtDebug>

QQBouchotSettingsDialog::QQBouchotSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQBouchotSettingsDialog)
{
	ui->setupUi(this);
	connect(ui->presetPushButton, SIGNAL(clicked()), this, SLOT(usePresetPressed()));
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okPressed()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));

	ui->presetComboBox->addItems(QQBouchot::getBouchotDefNameList());

}

QQBouchotSettingsDialog::QQBouchotSettingsDialog(QString bouchotName, QQBouchot::QQBouchotSettings bouchotSettings,
												 QStringList m_listGroups, QWidget * parent):
	QDialog(parent),
	ui(new Ui::QQBouchotSettingsDialog)
{
	ui->setupUi(this);
	connect(ui->presetPushButton, SIGNAL(clicked()), this, SLOT(usePresetPressed()));
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okPressed()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));

	ui->presetComboBox->addItems(QQBouchot::getBouchotDefNameList());

	m_bouchotSettings = bouchotSettings;
	m_bouchotName = bouchotName;
	setGroups(m_listGroups);
	setBouchot();
}

QQBouchotSettingsDialog::~QQBouchotSettingsDialog()
{
	delete ui;
}

void QQBouchotSettingsDialog::setGroups(QStringList listGroups)
{
	ui->groupSelComboBox->addItems(listGroups);
	ui->groupSelComboBox->setCurrentIndex( -1 );
}

void QQBouchotSettingsDialog::setBouchot()
{
	qDebug() << "QQBouchotSettingsDialog::setBouchot";

	//On ne change pas un nom de bouchot
	ui->nameLineEdit->setText(m_bouchotName);
	ui->nameLineEdit->setReadOnly(true);
	ui->presetPushButton->setEnabled(false);

	ui->groupSelComboBox->setCurrentIndex(ui->groupSelComboBox->findText(m_bouchotSettings.group(), Qt::MatchExactly | Qt::MatchCaseSensitive ));
	ui->colorLineEdit->setText(m_bouchotSettings.colorToString());
	ui->aliasLineEdit->setText(m_bouchotSettings.aliasesToString());
	ui->refreshRateLineEdit->setText(m_bouchotSettings.refreshToString());
	ui->loginLineEdit->setText(m_bouchotSettings.login());
	ui->uaLineEdit->setText(m_bouchotSettings.ua());
	ui->cookieLineEdit->setText(m_bouchotSettings.cookie());
	ui->urlPostLineEdit->setText(m_bouchotSettings.postUrl());
	ui->urlBackendLineEdit->setText(m_bouchotSettings.backendUrl());
	ui->postDataLineEdit->setText(m_bouchotSettings.postData());
	ui->slipTypeComboBox->setCurrentIndex(m_bouchotSettings.slipType());
}

void QQBouchotSettingsDialog::okPressed()
{
	qDebug() << "QQBouchotSettingsDialog::okPressed";

	m_bouchotSettings.setGroup(ui->groupSelComboBox->currentText());
	m_bouchotSettings.setColor(ui->colorLineEdit->text());
	m_bouchotSettings.setAliasesFromString(ui->aliasLineEdit->text());
	m_bouchotSettings.setRefreshFromString(ui->refreshRateLineEdit->text());
	m_bouchotSettings.setLogin(ui->loginLineEdit->text());
	m_bouchotSettings.setUa(ui->uaLineEdit->text());
	m_bouchotSettings.setCookie(ui->cookieLineEdit->text());
	m_bouchotSettings.setPostUrl(ui->urlPostLineEdit->text());
	m_bouchotSettings.setBackendUrl(ui->urlBackendLineEdit->text());
	m_bouchotSettings.setPostData(ui->postDataLineEdit->text());
	m_bouchotSettings.setSlipType((QQBouchot::TypeSlip) ui->slipTypeComboBox->currentIndex());

	m_bouchotName = ui->nameLineEdit->text();
}

void QQBouchotSettingsDialog::cancelPressed()
{
	qDebug() << "QQBouchotSettingsDialog::cancelPressed";
}

void QQBouchotSettingsDialog::usePresetPressed()
{
	qDebug() << "QQBouchotSettingsDialog::usePresetPressed";

	m_bouchotName = ui->presetComboBox->currentText();
	m_bouchotSettings = QQBouchot::getBouchotDef(m_bouchotName);

	ui->nameLineEdit->setText(m_bouchotName);
	ui->colorLineEdit->setText(m_bouchotSettings.colorToString());
	ui->aliasLineEdit->setText(m_bouchotSettings.aliasesToString());
	ui->refreshRateLineEdit->setText(m_bouchotSettings.refreshToString());
	ui->cookieLineEdit->setText(m_bouchotSettings.cookie());
	ui->urlPostLineEdit->setText(m_bouchotSettings.postUrl());
	ui->urlBackendLineEdit->setText(m_bouchotSettings.backendUrl());
	ui->postDataLineEdit->setText(m_bouchotSettings.postData());
	ui->slipTypeComboBox->setCurrentIndex(m_bouchotSettings.slipType());
}
