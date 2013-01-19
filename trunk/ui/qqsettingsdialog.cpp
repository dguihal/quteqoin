#include "qqsettingsdialog.h"
#include "ui_qqsettingsdialog.h"

#include "core/qqbouchot.h"
#include "ui/qqbouchotsettingsdialog.h"

#include <QtDebug>
#include <QStringListModel>
#include <QPalette>
#include <QUrl>

QQSettingsDialog::QQSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQSettingsDialog)
{
	ui->setupUi(this);
	ui->bouchotListView->setModel(new QStringListModel());
	connect(ui->srvAllowSearch, SIGNAL(clicked(bool)), this, SLOT(setTotozAllowSearch(bool)));

	connect(ui->addBouchotButton, SIGNAL(clicked()), this, SLOT(addBouchot()));
	connect(ui->deleteBouchotButton, SIGNAL(clicked()), this, SLOT(deleteBouchot()));
	connect(ui->editBouchotButton, SIGNAL(clicked()), this, SLOT(editBouchot()));
}

QQSettingsDialog::~QQSettingsDialog()
{
	delete ui;
}

void QQSettingsDialog::setMaxHistoryLength(uint maxHistLen)
{
	ui->maxHistLineEdit->setText(QString::number(maxHistLen));
}

uint QQSettingsDialog::maxHistoryLength()
{
	bool ok;
	uint res = ui->maxHistLineEdit->text().toUInt(&ok, 10);
	return res;
}

void QQSettingsDialog::setDefaultUA(const QString& defaultUA)
{
	ui->defaultUALlineEdit->setText(defaultUA);
}

QString QQSettingsDialog::defaultUA()
{
	return ui->defaultLoginLineEdit->text();
}

void QQSettingsDialog::setTotozServerUrl(const QString & totozSrvUrl)
{
	ui->srvTotozLineEdit->setText(totozSrvUrl);
}

QString QQSettingsDialog::totozServerUrl()
{
	return ui->srvTotozLineEdit->text();
}

void QQSettingsDialog::setTotozAllowSearch(bool totozAllowSearch)
{
	if(totozAllowSearch != ui->srvAllowSearch->isChecked())
		ui->srvAllowSearch->setChecked(totozAllowSearch);

	ui->srvTotozQueryPatternLineEdit->setDisabled(! totozAllowSearch);
}

bool QQSettingsDialog::totozAllowSearch()
{
	return ui->srvAllowSearch->isChecked();
}

void QQSettingsDialog::setTotozQueryPattern(const QString & totozQueryPattern)
{
	return ui->srvTotozQueryPatternLineEdit->setText(totozQueryPattern);
}

QString QQSettingsDialog::totozQueryPattern()
{
	return ui->srvTotozLineEdit->text();
}

void QQSettingsDialog::setTotozMode(int totozMode)
{
	ui->totozModeComboBox->setCurrentIndex(totozMode);
}

int QQSettingsDialog::totozMode()
{
	return ui->totozModeComboBox->currentIndex();
}

void QQSettingsDialog::setDefaultLogin(const QString & defaultLogin)
{
	ui->defaultLoginLineEdit->setText(defaultLogin);
}

QString QQSettingsDialog::defaultLogin()
{
	return ui->defaultLoginLineEdit->text();
}

void QQSettingsDialog::setBouchots(const QMap<QString, QQBouchot::QQBouchotSettings> currentListBouchot)
{
	m_bouchots = currentListBouchot;

	QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
	model->removeRows(0, model->rowCount());
	model->insertRows(0, m_bouchots.size());

	QMapIterator<QString, QQBouchot::QQBouchotSettings> i(m_bouchots);
	int index = 0;
	while(i.hasNext())
	{
		i.next();
		QQBouchot::QQBouchotSettings settings = i.value();
		if(! m_listGroups.contains(settings.group()))
			m_listGroups.append(settings.group());

		model->setData(model->index(index++), QVariant(i.key()));
	}
}

void QQSettingsDialog::addBouchot()
{
	QQBouchotSettingsDialog bouchotSettingsDialog(this);
	bouchotSettingsDialog.setGroups(m_listGroups);

	if(bouchotSettingsDialog.exec() == QDialog::Accepted)
	{
		QQBouchot::QQBouchotSettings bSettings = bouchotSettingsDialog.bouchotSettings();
		QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
		int numRow = model->rowCount();
		model->insertRows(numRow,1);
		model->setData(model->index(numRow), QVariant(bouchotSettingsDialog.bouchotName()));
		m_bouchots.insert(bouchotSettingsDialog.bouchotName(), bSettings);
		m_newBouchots.insert(bouchotSettingsDialog.bouchotName(), bSettings);
	}
}

void QQSettingsDialog::deleteBouchot()
{
	QModelIndexList indexes = ui->bouchotListView->selectionModel()->selectedIndexes();
	QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
	QModelIndex first;
	while(!indexes.isEmpty())
	{
		first = indexes.takeFirst();

		QString bouchotName = first.data().toString();
		m_oldBouchots.insert(bouchotName, m_bouchots.value(bouchotName));
		m_newBouchots.remove(bouchotName);

		model->removeRow(first.row());
	}
}

void QQSettingsDialog::editBouchot()
{
	QModelIndexList indexes = ui->bouchotListView->selectionModel()->selectedIndexes();
	QModelIndex first;

	while(!indexes.isEmpty())
	{
		first = indexes.takeFirst();

		QString bouchotName = first.data().toString();

		QQBouchotSettingsDialog bouchotSettingsDialog(bouchotName, m_bouchots.value(bouchotName), m_listGroups, this);

		if(bouchotSettingsDialog.exec() == QDialog::Accepted)
			m_modifBouchots.insert(bouchotName, bouchotSettingsDialog.bouchotSettings());
	}
}
