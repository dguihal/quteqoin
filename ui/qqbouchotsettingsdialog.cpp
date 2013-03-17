#include "qqbouchotsettingsdialog.h"
#include "ui_qqbouchotsettingsdialog.h"

#include <QtDebug>
#include <QColorDialog>
#include <QMessageBox>
#include <QRegExp>

QQBouchotSettingsDialog::QQBouchotSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQBouchotSettingsDialog)
{
	ui->setupUi(this);
	connect(ui->presetPushButton, SIGNAL(clicked()), this, SLOT(usePresetPressed()));
	connect(ui->colorSelPushButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
	connect(ui->colorLineEdit, SIGNAL(textChanged(QString)), this, SLOT(colorChanged(QString)));
	connect(ui->nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));

	QRegExp reg("^#[0-9a-fA-F]{0,6}$");
	ui->colorLineEdit->setValidator(new QRegExpValidator(reg, this));
	ui->presetComboBox->addItems(QQBouchot::getBouchotDefNameList());

}

QQBouchotSettingsDialog::QQBouchotSettingsDialog(QString bouchotName, QQBouchot::QQBouchotSettings bouchotSettings, QWidget * parent):
	QDialog(parent),
	ui(new Ui::QQBouchotSettingsDialog)
{
	ui->setupUi(this);
	connect(ui->presetPushButton, SIGNAL(clicked()), this, SLOT(usePresetPressed()));
	connect(ui->colorSelPushButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
	connect(ui->colorLineEdit, SIGNAL(textChanged(QString)), this, SLOT(colorChanged(QString)));
	connect(ui->nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));

	QRegExp reg("^#[0-9a-fA-F]{0,6}$");
	ui->colorLineEdit->setValidator(new QRegExpValidator(reg, this));
	ui->presetComboBox->addItems(QQBouchot::getBouchotDefNameList());

	m_bouchotSettings = bouchotSettings;
	m_bouchotName = bouchotName;
	setBouchot();
	ui->nameLineEdit->setReadOnly(true);
}

QQBouchotSettingsDialog::~QQBouchotSettingsDialog()
{
}

void QQBouchotSettingsDialog::setGroups(QStringList listGroups)
{
	ui->groupSelComboBox->addItems(listGroups);
	int index = ui->groupSelComboBox->findText(m_bouchotSettings.group(), Qt::MatchExactly | Qt::MatchCaseSensitive);
	ui->groupSelComboBox->setCurrentIndex(index);
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

void QQBouchotSettingsDialog::showColorDialog()
{
	QColor color(ui->colorLineEdit->text());
	color = QColorDialog::getColor(color, this);
	if (color.isValid())
	{
		QString colorName = color.name();
		ui->colorLineEdit->setText(colorName);
		ui->colorLineEdit->setStyleSheet("QLineEdit{background-color: " + colorName + ";}");
		ui->colorSelPushButton->setStyleSheet("QPushButton{background-color: " + colorName + ";}");
	}
}

void QQBouchotSettingsDialog::colorChanged(QString text)
{
	QColor color(text);
	if(color.isValid())
	{
		ui->colorLineEdit->setStyleSheet("QLineEdit{background-color: " + text + ";}");
		ui->colorSelPushButton->setStyleSheet("QPushButton{background-color: " + text + ";}");
		ui->colorSelPushButton->setText("");
	}
	else
	{
		ui->colorLineEdit->setStyleSheet("QLineEdit{}");
		ui->colorSelPushButton->setStyleSheet("QPushButton{color: red;}");
		ui->colorSelPushButton->setText(tr("Invalid"));
	}
}

void QQBouchotSettingsDialog::nameChanged(QString text)
{
	if(m_listNames.contains(text))
	{
		ui->nameLineEdit->setStyleSheet("QLineEdit{color: red;}");
	}
}

void QQBouchotSettingsDialog::accept()
{
	if(! m_listNames.contains(ui->nameLineEdit->text()))
	{
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
		QDialog::accept();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText(tr("This board name already exists, it must be unique."));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}
