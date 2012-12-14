#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqsettings.h"
#include "ui/qqpalmipede.h"
#include "ui/qqpinipede.h"
#include "ui/qqsettingsdialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QLayout>
#include <QSizePolicy>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{

	m_ui->setupUi(this);

	m_settings = new QQSettings(this);
	if(m_settings->hasWGeometry())
		setGeometry(m_settings->winGeometry());
	m_palmi = new QQPalmipede(this);
	m_pini = new QQPinipede(m_settings, this);

	QLayout *layout = new QVBoxLayout();
	int leftM, topM, rightM, bottomM;
	layout->getContentsMargins(& leftM, & topM,
							   & rightM, &bottomM);
	layout->setContentsMargins(0, topM, 0, bottomM);
	layout->addWidget(m_pini);
	layout->addWidget(m_palmi);
	m_ui->centralWidget->setLayout(layout);

	QList<QQBouchot *> bouchots = m_settings->listBouchots();
	for(int i = 0; i < bouchots.size(); i++)
		initBouchot(bouchots.at(i));

	connect(m_pini, SIGNAL(insertTextPalmi(QString)), m_palmi, SLOT(insertReplaceText(QString)));

	connect(m_palmi, SIGNAL(postMessage(QString,QString)), this, SLOT(doPostMessage(QString,QString)));
	connect(m_ui->actionOptions, SIGNAL(triggered()), this, SLOT(displayOptions()));
	connect(m_ui->actionEnregistrer_parametres, SIGNAL(triggered()), m_settings, SLOT(saveSettings()));
	connect(m_ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(this->m_ui->actionEtendu, SIGNAL(triggered()), this, SLOT(doTriggerMaxiPalmi()));
	connect(this->m_ui->actionMinimal, SIGNAL(triggered()), this, SLOT(doTriggerMiniPalmi()));
	connect(this->m_ui->actionEtendu, SIGNAL(triggered()), this->m_palmi, SLOT(maximizePalmi()));
	connect(this->m_ui->actionMinimal, SIGNAL(triggered()), this->m_palmi, SLOT(minimizePalmi()));

	if(m_settings->palmiMinimized())
		this->m_ui->actionMinimal->trigger();
	else
		this->m_ui->actionEtendu->trigger();

	m_settings->startBouchots();
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_pini;
	delete m_palmi;
	delete m_settings;
}

void MainWindow::displayOptions()
{
	//On arrete le refresh avant de continuer
	m_settings->stopBouchots();

	QQSettingsDialog settingsDialog(this);
	settingsDialog.setDefaultUA(m_settings->defaultUA());
	settingsDialog.setDefaultLogin(m_settings->defaultLogin());
	settingsDialog.setTotozServerUrl(m_settings->totozServerUrl());
	settingsDialog.setTotozMode(m_settings->totozMode());
	settingsDialog.setMaxHistoryLength(m_settings->maxHistoryLength());

	QMap<QString, QQBouchot::QQBouchotSettings> mapBouchotSettings;
	QList<QQBouchot *> listBouchots = m_settings->listBouchots();
	QQBouchot *bouchot = NULL;
	for(int i = 0; i < listBouchots.size(); i++ )
	{
		bouchot = listBouchots.at(i);
		mapBouchotSettings.insert(bouchot->name(), bouchot->settings() );
	}
	settingsDialog.setBouchots(mapBouchotSettings);

	if(settingsDialog.exec() == QDialog::Accepted)
	{
		m_settings->setDefaultUA(settingsDialog.defaultUA());
		m_settings->setDefaultLogin(settingsDialog.defaultLogin());
		m_settings->setTotozServerUrl(settingsDialog.totozServerUrl());
		m_settings->setTotozMode((QQSettings::TotozMode) settingsDialog.totozMode());

		unsigned int oldMaxHLength = m_settings->maxHistoryLength();
		m_settings->setMaxHistoryLength(settingsDialog.maxHistoryLength());
		if(m_settings->maxHistoryLength() < oldMaxHLength)
		{
			QList<QString> listTabs = m_settings->listTabs();
			for(int i = 0; i < listTabs.size(); i++)
				m_pini->purgePinitabHistory(listTabs.at(i));
		}

		//Les bouchots supprimes
		QMap<QString, QQBouchot::QQBouchotSettings> settingsDeletedbouchots = settingsDialog.oldBouchots();
		QMapIterator<QString, QQBouchot::QQBouchotSettings> iDel(settingsDeletedbouchots);
		while(iDel.hasNext())
		{
			iDel.next();
			QQBouchot * delBouchot = m_settings->bouchot(iDel.key());
			m_pini->purgePiniTab(delBouchot->settings().group(), delBouchot->name());
			m_settings->removeBouchot(delBouchot);
		}

		//Les bouchots modifies
		QMap<QString, QQBouchot::QQBouchotSettings> settingsModifiedbouchots = settingsDialog.modifiedBouchots();
		QMapIterator<QString, QQBouchot::QQBouchotSettings> iModif(settingsModifiedbouchots);
		while(iModif.hasNext())
		{
			iModif.next();
			QQBouchot * modifBouchot = m_settings->bouchot(iModif.key());
			//Sauvegarde de l'ancien groupe au cas ou il aurait été modifié pour pouvoir
			// effectuer une maj du pini au cas ou ...
			QString oldGroup = modifBouchot->settings().group();

			modifBouchot->setSettings(iModif.value());

			// Si c'est le groupe qui est modifié il faut impérativement faire un rafraichissement du Pini
			// afin d'effectuer la migration
			if(modifBouchot->settings().group() != oldGroup)
			{
				m_pini->addPiniTab(modifBouchot->settings().group());
				modifBouchot->setNewPostsFromHistory();
				m_pini->newPostsAvailable(modifBouchot);
				m_pini->purgePiniTab(oldGroup, modifBouchot->name());
			}

			m_settings->setDirty();
		}

		//Les bouchots ajoutes
		QMap<QString, QQBouchot::QQBouchotSettings> settingsNewBouchots = settingsDialog.newBouchots();
		QMapIterator<QString, QQBouchot::QQBouchotSettings> iNew(settingsNewBouchots);
		while(iNew.hasNext())
		{
			iNew.next();
			QQBouchot * newBouchot = new QQBouchot(iNew.key(), m_settings);
			newBouchot->setSettings(iNew.value());

			initBouchot(newBouchot);
			m_settings->addBouchot(newBouchot);
		}
	}
	m_settings->startBouchots();
}

void MainWindow::doPostMessage(const QString & bouchot, const QString & message)
{
	QQBouchot * bouchotDest = m_settings->bouchot(bouchot);

	if( bouchotDest != NULL)
		bouchotDest->postMessage(message);
	//else
	// Bouchot non trouvé ???
}

void MainWindow::doTriggerMiniPalmi()
{
	this->m_ui->actionEtendu->setChecked(false);
	this->m_ui->actionMinimal->setChecked(true);

	m_settings->setPalmiMinimized(true);
}

void MainWindow::doTriggerMaxiPalmi()
{
	this->m_ui->actionEtendu->setChecked(true);
	this->m_ui->actionMinimal->setChecked(false);

	m_settings->setPalmiMinimized(false);
}

void MainWindow::initBouchot(QQBouchot * bouchot)
{
	connect(bouchot, SIGNAL(newPostsInserted(QQBouchot *)), m_pini, SLOT(newPostsAvailable(QQBouchot *)));

	m_pini->addPiniTab(bouchot->settings().group());
	m_palmi->addBouchot(bouchot->name(), bouchot->settings().colorLight());
}

void MainWindow::closeEvent(QCloseEvent * event)
 {
	qDebug()<<"MainWindow::closeEvent";
	if(m_settings->maybeSave())
		event->accept();
	else
		event->ignore();
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
	QMainWindow::resizeEvent(event);
	m_settings->setWinGeometry(geometry());
}
