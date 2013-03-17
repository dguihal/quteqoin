#include "qqboardssettings.h"
#include "ui_qqboardssettings.h"

#include "ui/qqbouchotsettingsdialog.h"

#include <QtDebug>
#include <QStringListModel>

QQBoardsSettings::QQBoardsSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQBoardsSettings)
{
	ui->setupUi(this);
	ui->bouchotListView->setModel(new QStringListModel());

	connect(ui->addBouchotButton, SIGNAL(clicked()), this, SLOT(addBouchot()));
	connect(ui->deleteBouchotButton, SIGNAL(clicked()), this, SLOT(deleteBouchot()));
	connect(ui->editBouchotButton, SIGNAL(clicked()), this, SLOT(editBouchot()));
}

QQBoardsSettings::~QQBoardsSettings()
{
	delete ui;
}

void QQBoardsSettings::setBouchots(const QMap<QString, QQBouchot::QQBouchotSettings> currentListBouchot)
{
	m_bouchots = currentListBouchot;

	QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
	model->removeRows(0, model->rowCount());
	model->insertRows(0, m_bouchots.size());

	m_listNames = currentListBouchot.keys();

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

void QQBoardsSettings::addBouchot()
{
	QQBouchotSettingsDialog bouchotSettingsDialog(this);
	bouchotSettingsDialog.setGroups(m_listGroups);
	bouchotSettingsDialog.setNames(m_listNames);

	if(bouchotSettingsDialog.exec() == QDialog::Accepted)
	{
		QQBouchot::QQBouchotSettings bSettings = bouchotSettingsDialog.bouchotSettings();
		QString bouchotName = bouchotSettingsDialog.bouchotName();

		if(m_listNames.contains(bouchotName))
		{
			qWarning() << "Trying to insert a bouchot with an already existing name, that's forbidden";
		}
		else
		{
			m_listNames.append(bouchotName);

			if(! m_listGroups.contains(bSettings.group()))
				m_listGroups.append(bSettings.group());

			QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
			int numRow = model->rowCount();
			model->insertRows(numRow,1);
			model->setData(model->index(numRow), QVariant(bouchotSettingsDialog.bouchotName()));
			m_newBouchots.insert(bouchotSettingsDialog.bouchotName(), bSettings);
			m_oldBouchots.removeAll(bouchotSettingsDialog.bouchotName());
		}
	}
}

void QQBoardsSettings::deleteBouchot()
{
	QModelIndexList indexes = ui->bouchotListView->selectionModel()->selectedIndexes();
	QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
	QModelIndex first;

	while(!indexes.isEmpty())
	{
		first = indexes.takeFirst();

		QString bouchotName = first.data().toString();
		if(! m_oldBouchots.contains(bouchotName))
			m_oldBouchots.append(bouchotName);
		m_newBouchots.remove(bouchotName);
		m_bouchots.remove(bouchotName);
		m_listNames.removeOne(bouchotName);

		model->removeRow(first.row());
	}
}

void QQBoardsSettings::editBouchot()
{
	QModelIndexList indexes = ui->bouchotListView->selectionModel()->selectedIndexes();
	QModelIndex first;

	while(!indexes.isEmpty())
	{
		first = indexes.takeFirst();

		QString bouchotName = first.data().toString();

		QMap<QString, QQBouchot::QQBouchotSettings> *refMap = NULL;
		if(m_newBouchots.contains(bouchotName))
			refMap = &m_newBouchots; // nouveaux
		else if(m_modifBouchots.contains(bouchotName))
			refMap = &m_modifBouchots; // existants deja modifies
		else
			refMap = &m_bouchots; // existants non modifies

		QQBouchotSettingsDialog bouchotSettingsDialog(bouchotName, refMap->value(bouchotName), this);
		bouchotSettingsDialog.setGroups(m_listGroups);

		if(bouchotSettingsDialog.exec() == QDialog::Accepted)
		{
			if(m_newBouchots.contains(bouchotName))
				refMap = &m_newBouchots;
			else
				refMap = &m_modifBouchots;
			refMap->insert(bouchotName, bouchotSettingsDialog.bouchotSettings());
		}
	}
}
