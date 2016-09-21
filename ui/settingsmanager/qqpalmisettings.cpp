#include "qqpalmisettings.h"
#include "ui_qqpalmisettings.h"

#include <QtDebug>
#include <QPushButton>

////
/// \brief QQPalmiSettings::QQPalmiSettings
/// \param parent
///
QQPalmiSettings::QQPalmiSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQPalmiSettings)
{
	ui->setupUi(this);
}

////
/// \brief QQPalmiSettings::~QQPalmiSettings
///
QQPalmiSettings::~QQPalmiSettings()
{
	delete ui;
}

////
/// \brief QQPalmiSettings::setStaticShortcuts
/// \param list
///
void QQPalmiSettings::setStaticShortcuts(QList< QPair<QChar, QString> > list)
{
	for(int i = 0; i < list.size(); i++)
	{
		ui->shortcutsTable->appendStaticRow(list.at(i).first, list.at(i).second);
	}
	ui->shortcutsTable->resizeColumnsToContents();
}

////
/// \brief QQPalmiSettings::getUserShortcuts
/// \return
///
QList<QPair<QChar, QString> > QQPalmiSettings::getUserShortcuts()
{
	return ui->shortcutsTable->getUserShotcuts();
}

////
/// \brief QQPalmiSettings::setUserShortcuts
/// \param list
///
void QQPalmiSettings::setUserShortcuts(QList<QPair<QChar, QString> > list)
{
	for(int i = 0; i < list.size(); i++)
	{
		ui->shortcutsTable->appendUserRow(list.at(i).first, list.at(i).second);
	}
	ui->shortcutsTable->resizeColumnsToContents();
	ui->shortcutsTable->initLastLine();
}

////
/// \brief QQPalmiSettings::isPalmiMinimized
/// \return
///
bool QQPalmiSettings::isPalmiMinimized() const
{
	return ui->dmMinimizedRB->isChecked();
}

/////
/// \brief QQPalmiSettings::setPalmiMinimized
/// \param isPalmiMini
///
void QQPalmiSettings::setPalmiMinimized(const bool &isPalmiMini)
{
	ui->dmExtendedRB->setChecked(! isPalmiMini);
	ui->dmMinimizedRB->setChecked(isPalmiMini);
}

////
/// \brief QQPalmiSettings::isPalmiDocked
/// \return
///
bool QQPalmiSettings::isPalmiDocked() const
{
	return ui->dmDockRB->isChecked();
}

///
/// \brief QQPalmiSettings::setPalmiDocked
/// \param isPalmiDocked
///
void QQPalmiSettings::setPalmiDocked(const bool &isPalmiDocked)
{
	ui->dmFixedRB->setChecked(! isPalmiDocked);
	ui->dmDockRB->setChecked(isPalmiDocked);
}

///
/// \brief QQPalmiSettings::getFileSharingService
/// \return
///
QString QQPalmiSettings::getFileSharingService() const
{
	return ui->fsServiceProvider->currentText();
}

///
/// \brief QQPalmiSettings::setFileSharingServices
/// \param fileSharingServices
///
void QQPalmiSettings::setFileSharingServices(const QStringList &fileSharingServices)
{
	foreach (QString service, fileSharingServices) {
		ui->fsServiceProvider->addItem(service);
	}
}

///
/// \brief QQPalmiSettings::setFileSharingService
/// \param fileSharingService
///
void QQPalmiSettings::setFileSharingService(const QString &fileSharingService)
{
	ui->fsServiceProvider->setCurrentText(fileSharingService);
}
