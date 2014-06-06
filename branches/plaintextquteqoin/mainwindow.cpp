#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqsettings.h"
#include "ui/qqboardsinfo.h"
#include "ui/qqpalmipede.h"
#include "ui/qqpinipede.h"
#include "ui/qqpinisearchwidget.h"
#include "ui/qqsettingsmanager.h"
#include "ui/qqtotozmanager.h"
#include "ui/qqcmdtoolbuttons.h"

#include <QtDebug>
#include <QCloseEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QToolButton>

#define MAINWINDOW_STATE_CACHE_FILE "QuteQoin_Window_State"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);

	QIcon icon = QIcon(QString::fromLatin1(":/img/rubber_duck_yellow.svg"));
	setWindowIcon(icon);

	// Setup du palmi
	m_palmi = new QQPalmipede(this);
	m_palmi->setAllowedAreas(Qt::TopDockWidgetArea |
							 Qt::BottomDockWidgetArea);
	connect(m_palmi, SIGNAL(postMessage(QString,QString)), this, SLOT(doPostMessage(QString,QString)));
	connect(m_palmi, SIGNAL(visibilityChanged(bool)), this, SLOT(palmiVisibilityChanged(bool)));
	addDockWidget(Qt::BottomDockWidgetArea, m_palmi, Qt::Horizontal);

	QAction *actionPalmi = m_palmi->toggleViewAction();
	actionPalmi->setShortcut(Qt::ControlModifier + Qt::Key_P);

	// Setup du totoz manager
	m_totozManager = new QQTotozManager(this);
	m_totozManager->setAllowedAreas(Qt::LeftDockWidgetArea |
									Qt::RightDockWidgetArea);
	connect(m_totozManager, SIGNAL(totozClicked(QString)), m_palmi, SLOT(insertReplaceText(QString)));
	addDockWidget(Qt::RightDockWidgetArea, m_totozManager, Qt::Vertical);

	QAction *actionTotozManager = m_totozManager->toggleViewAction();
	actionTotozManager->setShortcut(Qt::ControlModifier + Qt::Key_T);

	// Setup du board info
	m_boardsInfo = new QQBoardsInfo(this);
	m_boardsInfo->setAllowedAreas(Qt::LeftDockWidgetArea |
									Qt::RightDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, m_boardsInfo, Qt::Vertical);

	QAction *actionBoardInfo = m_boardsInfo->toggleViewAction();
	actionBoardInfo->setShortcut(Qt::ControlModifier + Qt::Key_I);

	// Setup du bouton d'options
	QQCmdToolButtons *cmdToolsBtn = new QQCmdToolButtons(this);
	cmdToolsBtn->addAction(actionBoardInfo);
	cmdToolsBtn->addAction(actionPalmi);
	cmdToolsBtn->addAction(actionTotozManager);

	connect(cmdToolsBtn, SIGNAL(showOptions()), this, SLOT(displayOptions()));

	// Setup du pini
	QWidget *centralWidget = new QWidget(this);
	QLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(1, 1, 1, 1);

	m_pini = new QQPinipede(this);
	m_pini->setToolButton(cmdToolsBtn);
	m_pini->setTotozManager(m_totozManager);
	connect(m_pini, SIGNAL(insertTextPalmi(QString, QString)), m_palmi, SLOT(insertReplaceText(QString, QString)));
	layout->addWidget(m_pini);

	m_pSearchW = new QQPiniSearchWidget(this);
	m_pSearchW->hide();
	connect(m_pSearchW, SIGNAL(search(QString,bool)), m_pini, SLOT(searchText(QString,bool)));
	layout->addWidget(m_pSearchW);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	// Action complementaire
	QAction *actionSearch = new QAction(tr("Search pini"), this);
	actionSearch->setShortcut(Qt::ControlModifier + Qt::Key_F);
	actionSearch->setCheckable(true);
	connect(actionSearch, SIGNAL(triggered(bool)), m_pSearchW, SLOT(setVisible(bool)));
	cmdToolsBtn->addAction(actionSearch);

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QDir dirData(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#else
	QDir dirData(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif

	if(!dirData.exists())
			dirData.mkpath(dirData.path());

	QFile windowsStateCacheFile(dirData.filePath(MAINWINDOW_STATE_CACHE_FILE));
	bool success = false;
	if(windowsStateCacheFile.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		success = restoreGeometry(QByteArray::fromBase64(QString(windowsStateCacheFile.readLine().trimmed()).toUtf8()));
		success &= restoreState(QByteArray::fromBase64(QString(windowsStateCacheFile.readLine().trimmed()).toUtf8()));
	}
	windowsStateCacheFile.close();
	if(!success)
		windowsStateCacheFile.remove();

	QQSettings settings;
	//TODO : Remove later
	if(settings.contains(SETTINGS_MAINWINDOW_GEOMETRY))
		settings.remove(SETTINGS_MAINWINDOW_GEOMETRY);
	if(settings.contains(SETTINGS_MAINWINDOW_STATE))
		settings.remove(SETTINGS_MAINWINDOW_STATE);

	if(settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool())
		doTriggerMiniPalmi();
	else
		doTriggerMaxiPalmi();

	// Envoyer le focus par defaut sur le palmi s'il est visible
	if(m_palmi->isVisible())
		m_palmi->setFocus();

	initBouchots();
}

MainWindow::~MainWindow()
{
	delete m_ui;
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
	connect(&settingsManager, SIGNAL(fullRepaint()), this, SLOT(doFullRepaint()));
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

void MainWindow::closeEvent(QCloseEvent *event)
{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QDir dirData(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#else
	QDir dirData(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif

	if(!dirData.exists())
			dirData.mkpath(dirData.path());

	QFile windowsStateCacheFile(dirData.filePath(MAINWINDOW_STATE_CACHE_FILE));
	if(windowsStateCacheFile.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		windowsStateCacheFile.write(saveGeometry().toBase64());
		windowsStateCacheFile.write("\n");
		windowsStateCacheFile.write(saveState().toBase64());
		windowsStateCacheFile.write("\n");
	}
	windowsStateCacheFile.close();

	QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	bool processed = true;

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
		break;
	}
	default :
		processed = false;
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

	m_boardsInfo->updateBoardList();

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
}

void MainWindow::doFullRepaint()
{
	foreach(QString group, QQBouchot::listGroups())
		m_pini->repaintPiniTab(group);
}

void MainWindow::initBouchot(QQBouchot *bouchot)
{
	if(bouchot == NULL)
		return;

	bouchot->setParent(this);
	bouchot->registerForEventNotification(m_pini, QQBouchot::NewPostsAvailable | QQBouchot::StateChanged);
	m_pini->addPiniTab(bouchot->settings().group());
	m_palmi->addBouchot(bouchot->name(), bouchot->settings().colorLight());

	connect(bouchot, SIGNAL(destroyed(QQBouchot*)), this, SLOT(bouchotDestroyed(QQBouchot *)));
	connect(bouchot, SIGNAL(groupChanged(QQBouchot*,QString)), this, SLOT(bouchotGroupChanged(QQBouchot*,QString)));
	connect(bouchot, SIGNAL(visibilitychanged(QString)), m_pini, SLOT(bouchotVisibilityChanged(QString)));

	m_boardsInfo->updateBoardList();
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
