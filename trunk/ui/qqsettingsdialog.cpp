#include "qqsettingsdialog.h"
#include "ui_qqsettingsdialog.h"

#include "core/qqbouchot.h"
#include "ui/qqbouchotsettingsdialog.h"

#include <QDebug>
#include <QStringListModel>
#include <QUrl>

QQSettingsDialog::QQSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QQSettingsDialog)
{
    ui->setupUi(this);
	ui->bouchotListView->setModel(new QStringListModel());

	connect(ui->addBouchotButton, SIGNAL(clicked()), this, SLOT(addBouchot()));
	connect(ui->deleteBouchotButton, SIGNAL(clicked()), this, SLOT(deleteBouchot()));
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

void QQSettingsDialog::setTotozServerUrl(const QUrl& totozSrvUrl)
{
	ui->srvTotozLineEdit->setText(totozSrvUrl.toString());
}

QUrl QQSettingsDialog::totozServerUrl()
{
	QUrl resUrl(ui->srvTotozLineEdit->text());
	return resUrl;
}

void QQSettingsDialog::setTotozMode(int totozMode)
{
	ui->totozModeComboBox->setCurrentIndex(totozMode);
}

int QQSettingsDialog::totozMode()
{
	return ui->totozModeComboBox->currentIndex();
}

void QQSettingsDialog::setDefaultLogin(const QString& defaultLogin)
{
	ui->defaultLoginLineEdit->setText(defaultLogin);
}

QString QQSettingsDialog::defaultLogin()
{
	return ui->defaultLoginLineEdit->text();
}

void QQSettingsDialog::setListBouchots(const QList<QQBouchot *>& currentListBouchot)
{
	m_listBouchots.clear();
	m_listBouchots << currentListBouchot;
	QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
	model->removeRows(0, model->rowCount());
	model->insertRows(0, m_listBouchots.size());
	for(int i = 0; i < m_listBouchots.size(); i++)
		model->setData(model->index(i), QVariant(m_listBouchots[i]->name()));
}

QList<QQBouchot *> QQSettingsDialog::listBouchots()
{
	return m_listBouchots;
}

void QQSettingsDialog::addBouchot()
{
	QQBouchotSettingsDialog bouchotSettingsDialog(this);
	if(bouchotSettingsDialog.exec() == QDialog::Accepted)
	{
		QQBouchot *newBouchot = bouchotSettingsDialog.getConfiguredBouchot();
		QStringListModel *model = (QStringListModel *) ui->bouchotListView->model();
		int numRow = model->rowCount();
		model->insertRows(numRow,1);
		model->setData(model->index(numRow), QVariant(newBouchot->name()));
		m_listBouchots.append(newBouchot);
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

		for(int i = 0; i < m_listBouchots.size(); i++)
		{
			if(m_listBouchots[i]->name() == first.data().toString())
			{
				delete (m_listBouchots.takeAt(i));
				break;
			}
		}
		model->removeRow(first.row());
	}
}
