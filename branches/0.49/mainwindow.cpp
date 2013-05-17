#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqsettings.h"
#include "ui/qqpalmipede.h"
#include "ui/qqpinipede.h"
#include "ui/qqsettingsdialog.h"
#include "ui/qqsettingsmanager.h"
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

	// Setup du palmi
	m_palmi = new QQPalmipede(this);
	m_palmi->setAllowedAreas(Qt::TopDockWidgetArea |
							 Qt::BottomDockWidgetArea);
	connect(m_palmi, SIGNAL(postMessage(QString,QString)), this, SLOT(doPostMessage(QString,QString)));
	connect(m_palmi, SIGNAL(visibilityChanged(bool)), this, SLOT(palmiVisibilityChanged(bool)));
	addDockWidget(Qt::BottomDockWidgetArea, m_palmi, Qt::Horizontal);

	// Setup du totoz manager
	m_totozManager = new QQTotozManager(this);
	m_totozManager->setAllowedAreas(Qt::LeftDockWidgetArea |
									Qt::RightDockWidgetArea);
	connect(m_totozManager, SIGNAL(totozClicked(QString)), m_palmi, SLOT(insertReplaceText(QString)));
	connect(m_totozManager, SIGNAL(visibilityChanged(bool)), this, SLOT(totozManagerVisibilityChanged(bool)));
	m_totozManager->setVisible(false);
	addDockWidget(Qt::RightDockWidgetArea, m_totozManager, Qt::Vertical);

	QAction *actionTotozManager = m_totozManager->toggleViewAction();
	actionTotozManager->setShortcut(Qt::ControlModifier + Qt::Key_T);
	m_ui->toolsMenu->addAction(actionTotozManager);

	// Setup du pini
	m_pini = new QQPinipede(this);
	m_pini->setTotozManager(m_totozManager);
	connect(m_pini, SIGNAL(insertTextPalmi(QString, QString)), m_palmi, SLOT(insertReplaceText(QString, QString)));
	setCentralWidget(m_pini);

	connect(m_ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(m_ui->actionPalmiExt, SIGNAL(triggered()), this, SLOT(doTriggerMaxiPalmi()));
	connect(m_ui->actionPalmiMini, SIGNAL(triggered()), this, SLOT(doTriggerMiniPalmi()));
	connect(m_ui->actionPalmiHidden, SIGNAL(triggered(bool)), this, SLOT(doTriggerHiddenPalmi(bool)));
	connect(m_ui->actionOptions, SIGNAL(triggered()), this, SLOT(displayOptions()));

	QQSettings settings;
	if(settings.contains(SETTINGS_MAINWINDOW_GEOMETRY))
		restoreGeometry(settings.value(SETTINGS_MAINWINDOW_GEOMETRY).toByteArray());
	if(settings.contains(SETTINGS_MAINWINDOW_STATE))
		restoreState(settings.value(SETTINGS_MAINWINDOW_STATE).toByteArray());

	if(settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool())
		m_ui->actionPalmiMini->trigger();
	else
		m_ui->actionPalmiExt->trigger();

	if(settings.value(SETTINGS_PALMI_HIDDEN, DEFAULT_PALMI_HIDDEN).toBool())
		m_ui->actionPalmiHidden->trigger();

	// Envoyer le focus par defaut sur le palmi s'il est visible
	if(m_palmi->isVisible())
		m_palmi->setFocus(Qt::OtherFocusReason);

	initBouchots();
}

MainWindow::~MainWindow()
{
}

void MainWindow::displayOptions()
{
	QList<QQBouchot *> bouchots = QQBouchot::listBouchots();
	for(int i = 0; i < bouchots.size(); i++)
		bouchots.at(i)->stopRefresh();

	QQSettingsManager settingsManager(this);
	connect(&settingsManager, SIGNAL(bouchotCreated(QQBouchot*)), this, SLOT(initBouchot(QQBouchot*)));
	connect(&settingsManager, SIGNAL(minimizePalmi()), this, SLOT(doTriggerMiniPalmi()));
	connect(&settingsManager, SIGNAL(maximizePalmi()), this, SLOT(doTriggerMaxiPalmi()));
	connect(&settingsManager, SIGNAL(hidePalmi(bool)), this, SLOT(doTriggerHiddenPalmi(bool)));
	connect(&settingsManager, SIGNAL(totozSearchEnabledChanged(bool)), m_totozManager, SLOT(totozSearchEnabled(bool)));
	settingsManager.exec();

	bouchots = QQBouchot::listBouchots();
	for(int i = 0; i < bouchots.size(); i++)
		bouchots.at(i)->startRefresh();
}

void MainWindow::doPostMessage(const QString &bouchot, const QString &message)
{
	QQBouchot *bouchotDest = QQBouchot::bouchot(bouchot);

	if( bouchotDest != NULL)
		bouchotDest->postMessage(message);
	//else
	// Bouchot non trouvé ???
}

void MainWindow::doTriggerHiddenPalmi(bool checked)
{
	m_ui->actionPalmiHidden->setChecked(checked);
	m_palmi->setHidden(checked);

	QQSettings settings;
	settings.setValueWithDefault(SETTINGS_PALMI_HIDDEN, checked, DEFAULT_PALMI_HIDDEN);

}

void MainWindow::doTriggerMaxiPalmi()
{
	minimizePalmi(false);
}

void MainWindow::doTriggerMiniPalmi()
{
	minimizePalmi(true);
}

void MainWindow::minimizePalmi(bool isPalmiMini)
{
	m_ui->actionPalmiExt->setChecked(! isPalmiMini);
	m_ui->actionPalmiMini->setChecked(isPalmiMini);

	QQSettings settings;
	settings.setValueWithDefault(SETTINGS_PALMI_MINI, isPalmiMini, DEFAULT_PALMI_MINI);

	m_palmi->setMinimal(isPalmiMini);
}

void MainWindow::palmiVisibilityChanged(bool visible)
{
	m_ui->actionPalmiHidden->setChecked(! visible);
	if(visible)
		m_palmi->setFocus();
	else
		m_pini->setFocus();
}

void MainWindow::totozManagerVisibilityChanged(bool visible)
{
	if(visible)
		m_totozManager->setFocus();
	else if(m_palmi->isVisible())
		m_palmi->setFocus();
	else
		m_pini->setFocus();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QQSettings settings;
	settings.setValue(SETTINGS_MAINWINDOW_GEOMETRY, saveGeometry());
	settings.setValue(SETTINGS_MAINWINDOW_STATE, saveState());

	QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	bool processed = false;

	switch(event->key())
	{
	case Qt::Key_F5:
	{
		QList<QQBouchot *> bouchots = QQBouchot::listBouchots();
		for(int i = 0; i < bouchots.size(); i++)
		{
			QQBouchot *bouchot = bouchots.at(i);
			bouchot->stopRefresh();
			bouchot->startRefresh();
		}
		processed = true;
		break;
	}
	default :
		break;
	}

	if(! processed)
	QMainWindow::keyPressEvent(event);
}

void MainWindow::bouchotDestroyed(QQBouchot *bouchot)
{
	QString name = bouchot->name();
	QString group = bouchot->settings().group();

	m_palmi->removeBouchot(bouchot->name());

	QList<QQBouchot *> bouchots = QQBouchot::listBouchotsGroup(group);
	(bouchots.size() == 0) ?
				m_pini->removePiniTab(group) :
				m_pini->purgePiniTab(group, name);

}

void MainWindow::bouchotGroupChanged(QQBouchot *bouchot, QString oldGroupName)
{
	QString name = bouchot->name();
	QString group = bouchot->settings().group();

	QList<QQBouchot *> bouchots = QQBouchot::listBouchotsGroup(oldGroupName);
	(bouchots.size() == 0) ?
				m_pini->removePiniTab(oldGroupName) :
				m_pini->purgePiniTab(oldGroupName, name);

	m_pini->addPiniTab(bouchot->settings().group());
	bouchot->setNewPostsFromHistory();
	m_pini->newPostsAvailable(bouchot->settings().group());
}

void MainWindow::initBouchot(QQBouchot *bouchot)
{
	if(bouchot == NULL)
		return;

	bouchot->setParent(this);
	m_pini->addPiniTab(bouchot->settings().group());
	m_palmi->addBouchot(bouchot->name(), bouchot->settings().colorLight());

	connect(bouchot, SIGNAL(newPostsAvailable(QString)), m_pini, SLOT(newPostsAvailable(QString)));
	connect(bouchot, SIGNAL(destroyed(QQBouchot*)), this, SLOT(bouchotDestroyed(QQBouchot *)));
	connect(bouchot, SIGNAL(groupChanged(QQBouchot*,QString)), this, SLOT(bouchotGroupChanged(QQBouchot*,QString)));
}


void MainWindow::initBouchots()
{
	QQSettings settings;

	QStringList list = settings.listBouchots();
	QQBouchot *bouchot = NULL;
	for(int i = 0; i < list.size(); i++)
	{
		bouchot = settings.loadBouchot(list.at(i));
		if(bouchot == NULL)
			continue;

		initBouchot(bouchot);
		bouchot->startRefresh();
	}
}
