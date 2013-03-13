#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqsettings.h"
#include "ui/qqpalmipede.h"
#include "ui/qqpinipede.h"
#include "ui/qqsettingsdialog.h"
#include "ui/qqtotozmanager.h"

#include <QCloseEvent>
#include <QtDebug>
#include <QIcon>
#include <QKeyEvent>
#include <QLayout>
#include <QSizePolicy>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);

	QIcon icon = QIcon(QString::fromAscii(":/img/rubber_duck_yellow.svg"));
	setWindowIcon(icon);

	m_settings = new QQSettings(this);

	m_pini = new QQPinipede(m_settings, this);
	setCentralWidget(m_pini);

	m_palmi = new QQPalmipede(this);
	m_palmi->setAllowedAreas(Qt::TopDockWidgetArea |
							 Qt::BottomDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, m_palmi, Qt::Horizontal);

	m_totozManager = new QQTotozManager(m_settings, this);
	m_totozManager->setAllowedAreas(Qt::LeftDockWidgetArea |
									Qt::RightDockWidgetArea);
	connect(m_totozManager, SIGNAL(totozClicked(QString)), m_palmi, SLOT(insertReplaceText(QString)));
	connect(m_totozManager, SIGNAL(visibilityChanged(bool)), this, SLOT(totozManagerVisibilityChanged(bool)));
	m_totozManager->setVisible(false);
	addDockWidget(Qt::RightDockWidgetArea, m_totozManager, Qt::Vertical);

	QAction * actionTotozManager = m_totozManager->toggleViewAction();
	m_ui->toolsMenu->addAction(actionTotozManager);

	QList<QQBouchot *> bouchots = m_settings->listBouchots();
	for(int i = 0; i < bouchots.size(); i++)
		initBouchot(bouchots.at(i));

	m_pini->setTotozManager(m_totozManager);
	connect(m_pini, SIGNAL(insertTextPalmi(QString, QString)), m_palmi, SLOT(insertReplaceText(QString, QString)));

	connect(m_palmi, SIGNAL(postMessage(QString,QString)), this, SLOT(doPostMessage(QString,QString)));
	connect(m_ui->actionEnregistrer_parametres, SIGNAL(triggered()), m_settings, SLOT(saveSettings()));
	connect(m_ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(m_ui->actionEtendu, SIGNAL(triggered()), this, SLOT(doTriggerMaxiPalmi()));
	connect(m_ui->actionMinimal, SIGNAL(triggered()), this, SLOT(doTriggerMiniPalmi()));
	connect(m_ui->actionOptions, SIGNAL(triggered()), this, SLOT(displayOptions()));

	if(m_settings->palmiMinimized())
		this->m_ui->actionMinimal->trigger();
	else
		this->m_ui->actionEtendu->trigger();

	if(m_settings->contains(SETTINGS_MAINWINDOW_GEOMETRY))
		restoreGeometry(m_settings->value(SETTINGS_MAINWINDOW_GEOMETRY).toByteArray());
	if(m_settings->contains(SETTINGS_MAINWINDOW_STATE))
		restoreState(m_settings->value(SETTINGS_MAINWINDOW_STATE).toByteArray());

	//Special euro<
	// s'assurer que le palmi est visible
	m_palmi->setVisible(true);
	m_palmi->setFocus(Qt::OtherFocusReason);

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

	settingsDialog.setTotozServerUrl(
					m_settings->value(SETTINGS_TOTOZ_SERVER_URL, DEFAULT_TOTOZ_SERVER_URL).toString());
	settingsDialog.setTotozServerBaseImg(
				m_settings->value(SETTINGS_TOTOZ_SERVER_BASE_IMG, DEFAULT_TOTOZ_SERVER_BASE_IMG).toString());
	settingsDialog.setTotozServerAllowSearch(
				m_settings->value(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH).toBool());
	settingsDialog.setTotozQueryPattern(
				m_settings->value(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN, DEFAULT_TOTOZ_SERVER_QUERY_SEARCH).toString());
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
		m_settings->setValue(SETTINGS_TOTOZ_SERVER_URL, settingsDialog.totozServerUrl());
		m_settings->setValue(SETTINGS_TOTOZ_SERVER_BASE_IMG, settingsDialog.totozServerBaseImg());
		m_settings->setValue(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, settingsDialog.totozServerAllowSearch());
		m_settings->setValue(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN, settingsDialog.totozQueryPattern());
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
			QString group = delBouchot->settings().group();

			m_settings->removeBouchot(delBouchot);
			m_palmi->removeBouchot(delBouchot->name());

			QList<QQBouchot *> bouchots = m_settings->listBouchots(group);
			(bouchots.size() == 0) ?
						m_pini->removePiniTab(group) :
						m_pini->purgePiniTab(group, delBouchot->name());
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
				m_pini->newPostsAvailable(modifBouchot->settings().group());

				QList<QQBouchot *> bouchots = m_settings->listBouchots(oldGroup);
				(bouchots.size() == 0) ?
							m_pini->removePiniTab(oldGroup) :
							m_pini->purgePiniTab(oldGroup, modifBouchot->name());
			}
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
	m_ui->actionEtendu->setChecked(false);
	m_ui->actionMinimal->setChecked(true);

	m_settings->setPalmiMinimized(true);
	m_palmi->setMinimal(true);
}

void MainWindow::doTriggerMaxiPalmi()
{
	m_ui->actionEtendu->setChecked(true);
	m_ui->actionMinimal->setChecked(false);

	m_settings->setPalmiMinimized(false);
	m_palmi->setMinimal(false);
}

void MainWindow::initBouchot(QQBouchot * bouchot)
{
	m_pini->addPiniTab(bouchot->settings().group());
	m_palmi->addBouchot(bouchot->name(), bouchot->settings().colorLight());

	connect(bouchot, SIGNAL(newPostsAvailable(QString)), m_pini, SLOT(newPostsAvailable(QString)));
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	QMainWindow::closeEvent(event);
	m_settings->setValue(SETTINGS_MAINWINDOW_GEOMETRY, saveGeometry());
	m_settings->setValue(SETTINGS_MAINWINDOW_STATE, saveState());
	m_settings->saveSettings();
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	bool processed = false;

	int key = event->key();
	switch(key)
	{
	case Qt::Key_F5:
		m_settings->stopBouchots();
		m_settings->startBouchots();
		processed = true;
		break;
	case Qt::Key_T:
		if(event->modifiers() == Qt::ControlModifier)
		{
			m_totozManager->setVisible(! m_totozManager->isVisible());
			processed = true;
		}
		break;
	default :
		break;
	}
	if(! processed)
		QMainWindow::keyPressEvent(event);

}

void MainWindow::totozManagerVisibilityChanged(bool visible)
{
	if(visible)
		m_totozManager->setFocus();
	else
		m_palmi->setFocus();
}
