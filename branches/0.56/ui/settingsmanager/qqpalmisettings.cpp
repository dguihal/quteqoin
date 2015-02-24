#include "qqpalmisettings.h"
#include "ui_qqpalmisettings.h"

#include <QtDebug>
#include <QPushButton>

QQPalmiSettings::QQPalmiSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQPalmiSettings)
{
	ui->setupUi(this);
}

QQPalmiSettings::~QQPalmiSettings()
{
	delete ui;
}

void QQPalmiSettings::setStaticShortcuts(QList< QPair<QChar, QString> > list)
{
	for(int i = 0; i < list.size(); i++)
	{
		ui->shortcutsTable->appendStaticRow(list.at(i).first, list.at(i).second);
	}
	ui->shortcutsTable->resizeColumnsToContents();
}

QList<QPair<QChar, QString> > QQPalmiSettings::getUserShortcuts()
{
	return ui->shortcutsTable->getUserShotcuts();
}

void QQPalmiSettings::setUserShortcuts(QList<QPair<QChar, QString> > list)
{
	for(int i = 0; i < list.size(); i++)
	{
		ui->shortcutsTable->appendUserRow(list.at(i).first, list.at(i).second);
	}
	ui->shortcutsTable->resizeColumnsToContents();
	ui->shortcutsTable->initLastLine();
}

bool QQPalmiSettings::isPalmiMinimized() const
{
	return ui->dmMinimizedRB->isChecked();
}

void QQPalmiSettings::setPalmiMinimized(const bool &isPalmiMini)
{
	ui->dmExtendedRB->setChecked(! isPalmiMini);
	ui->dmMinimizedRB->setChecked(isPalmiMini);
}

bool QQPalmiSettings::isPalmiDocked() const
{
	return ui->dmDockRB->isChecked();
}

void QQPalmiSettings::setPalmiDocked(const bool &isPalmiDocked)
{
	ui->dmFixedRB->setChecked(! isPalmiDocked);
	ui->dmDockRB->setChecked(isPalmiDocked);
}
