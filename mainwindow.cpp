#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqsettings.h"
#include "core/qqnetworkaccessor.h"
#include "ui/qqboardsinfo.h"
#include "ui/qqpalmipede.h"
#include "ui/palmipede/qqdockpalmi.h"
#include "ui/qqpinipede.h"
#include "ui/qqpinisearchwidget.h"
#include "ui/qqsettingsmanager.h"
#include "ui/qqtotozmanager.h"
#include "ui/qqcmdtoolbuttons.h"

#include <QtDebug>
#include <QtGlobal>
#include <QApplication>
#include <QCloseEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QLayout>
#include <QMenu>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QToolButton>

#ifdef QML_PALMI
#include <QQuickWidget>
#include <QQuickItem>
#endif

#define MAINWINDOW_STATE_CACHE_FILE "QuteQoin_Window_State"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_trayIcon(nullptr)
{
	m_ui->setupUi(this);

	QIcon icon = QIcon(QString::fromLatin1(":/img/rubber_duck_yellow.svg"));
	setWindowIcon(icon);

	// Setup du reseau
	QQNetworkAccessor::updateProxySettings();

	// Setup du palmi
#ifdef QML_PALMI
	m_palmi = new QQuickWidget(QUrl("qrc:/qml/QQmlPalmi.qml"), this);
	m_palmi->setResizeMode(QQuickWidget::SizeRootObjectToView);

	QObject::connect(m_palmi->rootObject(), SIGNAL(post(QString, QString)),
	                 this, SLOT(doPostMessage(QString, QString)));
#else
	m_palmi = new QQPalmipede(this);
	connect(m_palmi, &QQPalmipede::postMessage, this, &MainWindow::doPostMessage);
#endif

	// Setup du dock du palmi
	m_dockPalmi = new QQDockPalmi(this);
	addDockWidget(Qt::BottomDockWidgetArea, m_dockPalmi, Qt::Horizontal);
	connect(m_dockPalmi, &QQDockPalmi::visibilityChanged, this, &MainWindow::doPalmiVisibilityChanged);

	m_actionDockPalmi = m_dockPalmi->toggleViewAction();
	m_actionDockPalmi->setShortcut(Qt::ControlModifier + Qt::Key_P);

	// Setup du totoz manager
	m_totozManager = new QQTotozManager(this);
#ifdef QML_PALMI
	qmlRegisterType<QQTotozManager>("QuteQoin.QmlComponents", 1, 0, "TotozManager");
	connect(m_totozManager, SIGNAL(totozClicked(QString)),
	        m_palmi->rootObject(), SIGNAL(insertReplaceText(QString)));
#else
	m_palmi->insertReplaceText("toto");
	connect(m_totozManager, &QQTotozManager::totozClicked, m_palmi, QOverload<const QString &>::of(&QQPalmipede::insertReplaceText));
#endif

	addDockWidget(Qt::RightDockWidgetArea, m_totozManager, Qt::Vertical);

	QAction *actionTotozManager = m_totozManager->toggleViewAction();
	actionTotozManager->setShortcut(Qt::ControlModifier + Qt::Key_T);

	// Setup du board info
	m_boardsInfo = new QQBoardsInfo(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_boardsInfo, Qt::Vertical);

	QAction *actionBoardInfo = m_boardsInfo->toggleViewAction();
	actionBoardInfo->setShortcut(Qt::ControlModifier + Qt::Key_I);

	// Setup du bouton d'options
	QQCmdToolButtons *cmdToolsBtn = new QQCmdToolButtons(this);
	cmdToolsBtn->addAction(actionBoardInfo);
	cmdToolsBtn->addAction(m_actionDockPalmi);
	cmdToolsBtn->addAction(actionTotozManager);

	connect(cmdToolsBtn, &QQCmdToolButtons::showOptions, this, &MainWindow::displayOptions);

	// Setup du pini
	QWidget *centralWidget = new QWidget(this);
	QLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setSpacing(1);

	m_pini = new QQPinipede(this);
	m_pini->setToolButton(cmdToolsBtn);
	m_pini->setTotozManager(m_totozManager);
#ifdef QML_PALMI
	connect(m_pini, SIGNAL(insertTextPalmi(QString, QString)),
	        m_palmi->rootObject(), SIGNAL(insertReplaceTextBoard(QString, QString)));
#else
	connect(m_pini, &QQPinipede::insertTextPalmi, m_palmi, QOverload<const QString &, const QString &>::of(&QQPalmipede::insertReplaceText));
#endif
	layout->addWidget(m_pini);

	m_pSearchW = new QQPiniSearchWidget(this);
	m_pSearchW->hide();
	connect(m_pSearchW, &QQPiniSearchWidget::search, m_pini, &QQPinipede::searchText);
	layout->addWidget(m_pSearchW);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	// Action complementaire
	QAction *actionSearch = new QAction(tr("Search pini"), this);
	actionSearch->setShortcut(Qt::ControlModifier + Qt::Key_F);
	actionSearch->setCheckable(true);
	connect(actionSearch, &QAction::triggered, m_pSearchW, &QQPiniSearchWidget::setVisible);
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

	//TODO : A supprimer
	if(settings.contains(SETTINGS_MAINWINDOW_GEOMETRY))
		settings.remove(SETTINGS_MAINWINDOW_GEOMETRY);
	if(settings.contains(SETTINGS_MAINWINDOW_STATE))
		settings.remove(SETTINGS_MAINWINDOW_STATE);
	//Fin TODO

	doPalmiStatusChanged(settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool(),
	                     settings.value(SETTINGS_PALMI_DOCKED, DEFAULT_PALMI_DOCKED).toBool());

	if(settings.value(SETTINGS_GENERAL_STEALTH_MODE, DEFAULT_GENERAL_STEALTH_MODE).toBool() &&
	        QSystemTrayIcon::isSystemTrayAvailable())
	{
		QMenu *m = new QMenu(this);
		QAction *quit = m->addAction(tr("&Quit"));
		connect(quit, &QAction::triggered, qApp, &QApplication::quit);

		m_trayIcon = new QSystemTrayIcon(QIcon(":/img/rubber_duck_yellow.svg"), this);
		connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

		m_trayIcon->setContextMenu(m);
		m_trayIcon->show();
	}

	// Envoyer le focus par defaut sur le palmi s'il est visible
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
	connect(&settingsManager, &QQSettingsManager::bouchotCreated, this, &MainWindow::initBouchot);
	connect(&settingsManager, &QQSettingsManager::fullRepaint, this, &MainWindow::doFullRepaint);
	connect(&settingsManager, &QQSettingsManager::networkProxySettingsChanged, this, &MainWindow::doNetworkSettingsChanged);
	connect(&settingsManager, &QQSettingsManager::palmiStatusChanged, this, &MainWindow::doPalmiStatusChanged);
	connect(&settingsManager, &QQSettingsManager::totozSearchEnabledChanged, m_totozManager, &QQTotozManager::totozSearchEnabled);
	settingsManager.exec();

	bouchots = QQBouchot::listBouchots();
	for(int i = 0; i < bouchots.size(); i++)
		bouchots.at(i)->startRefresh();
}

void MainWindow::doPostMessage(const QString &bouchot, const QString &message)
{
	qDebug() << Q_FUNC_INFO << bouchot << message;
	QQBouchot *bouchotDest = QQBouchot::bouchot(bouchot);

	if(bouchotDest != nullptr)
		bouchotDest->postMessage(message);
	//else
	// Bouchot non trouvé ???
}

void MainWindow::doPalmiStatusChanged(bool isPalmiMini, bool isPalmiDocked)
{
#ifdef QML_PALMI
	QQuickWidget::Status s = m_palmi->status();
	qDebug() << Q_FUNC_INFO << s;
	if(s == QQuickWidget::Error)
	{
		foreach (QQmlError e, m_palmi->errors()) {
			qDebug() << Q_FUNC_INFO << e.description();
		}
	}

	QQuickItem *palmiItem = m_palmi->rootObject();

	if(isPalmiMini)
		palmiItem->setState("MINIMIZED");
	else
		palmiItem->setState("MAXIMIZED");
#else
	m_palmi->setMinimal(isPalmiMini);
#endif

	m_actionDockPalmi->setVisible(isPalmiDocked);

	QWidget *w = centralWidget();
	QLayout *l = w->layout();
	if(isPalmiDocked)
	{
		l->removeWidget(m_palmi);
		m_palmi->setParent(m_dockPalmi);
		m_dockPalmi->takePalmiWidget(m_palmi);
		m_dockPalmi->show();
	}
	else
	{
		m_dockPalmi->releasePalmiWidget();
		m_palmi->setParent(w);
		l->addWidget(m_palmi);
		m_dockPalmi->hide();
	}

	m_palmi->setVisible(true);
}

void MainWindow::doPalmiVisibilityChanged(bool isVisible)
{
	if(isVisible)
		m_palmi->setFocus();
	else
		m_pini->setFocus();
}

void MainWindow::changeEvent(QEvent *event)
{
	if((event->type() == QEvent::WindowStateChange) &&
	        (m_trayIcon != nullptr))
	{
		if (isMinimized() == true)
			hide();
	}

	return QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QDir dirData(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

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
	case Qt::Key_Escape:
		setWindowState(Qt::WindowMinimized);
		break;
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

#ifdef QML_PALMI
	QMetaObject::invokeMethod(m_palmi->rootObject(), "removeBoard",
	                          Q_ARG(QVariant, bouchot->name()));
#else
	m_palmi->removeBouchot(bouchot->name());
#endif

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
	for(QString group : QQBouchot::listGroups())
		m_pini->repaintPiniTab(group);
}

void MainWindow::doNetworkSettingsChanged()
{
	QQNetworkAccessor::updateProxySettings();
}

void MainWindow::initBouchot(QQBouchot *bouchot)
{
	if(bouchot == nullptr)
		return;

	bouchot->setParent(this);
	bouchot->registerForEventNotification(m_pini, QQBouchot::NewPostsAvailable | QQBouchot::StateChanged);
	m_pini->addPiniTab(bouchot->settings().group());
#ifdef QML_PALMI
	QMetaObject::invokeMethod(m_palmi->rootObject(), "addBoard",
	                          Q_ARG(QVariant, bouchot->name()),
	                          Q_ARG(QVariant, bouchot->settings().color()),
	                          Q_ARG(QVariant, bouchot->settings().colorLight()) );
#else
	if(! bouchot->isReadOnly())
		m_palmi->addBouchot(bouchot->name(), bouchot->settings().colorLight());
#endif

	connect(bouchot, &QQBouchot::destroyed, this, &MainWindow::bouchotDestroyed);
	connect(bouchot, &QQBouchot::groupChanged, this, &MainWindow::bouchotGroupChanged);
	connect(bouchot, &QQBouchot::visibilitychanged, m_pini, &QQPinipede::bouchotVisibilityChanged);

	m_boardsInfo->updateBoardList();
}


void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger)
	{
		if(isHidden())
			showNormal();
		else
			hide();
	}
}

void MainWindow::initBouchots()
{
	QQSettings settings;

	QStringList list = settings.listBouchots();
	QQBouchot *bouchot = nullptr;
	for(int i = 0; i < list.size(); i++)
	{
		bouchot = settings.loadBouchot(list.at(i));
		if(bouchot == nullptr)
			continue;

		initBouchot(bouchot);
		bouchot->startRefresh();
	}
}
