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
    ui(new Ui::MainWindow)
{

	pini = new QQPinipede(this);

	settings = new QQSettings(this);
	settings->setPinipede(pini);

	palmi = new QQPalmipede(this);

	ui->setupUi(this);

	QLayout *layout = new QVBoxLayout();
	layout->addWidget(pini);
	layout->addWidget(palmi);
	ui->centralWidget->setLayout(layout);

	connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(displayOptions()));

	connect(ui->actionEnregistrer_parametres, SIGNAL(triggered()), settings, SLOT(saveSettings()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

	settings->startBouchots();
}

MainWindow::~MainWindow()
{
    delete ui;
	delete pini;
	delete palmi;
	delete settings;
}


void MainWindow::displayOptions()
{
	settings->stopBouchots();
	QQSettingsDialog settingsDialog(this);
	settingsDialog.setDefaultUA(settings->defaultUA());
	settingsDialog.setDefaultLogin(settings->defaultLogin());
	settingsDialog.setTotozServerUrl(settings->totozServerUrl());
	settingsDialog.setTotozMode(settings->totozMode());
	settingsDialog.setMaxHistoryLength(settings->maxHistoryLength());
	settingsDialog.setListBouchots(settings->listBouchots());
	if(settingsDialog.exec() == QDialog::Accepted)
	{
		settings->setDefaultUA(settingsDialog.defaultUA());
		settings->setDefaultLogin(settingsDialog.defaultLogin());
		settings->setTotozServerUrl(settingsDialog.totozServerUrl());
		settings->setTotozMode((QQSettings::TotozMode) settingsDialog.totozMode());
		settings->setMaxHistoryLength(settingsDialog.maxHistoryLength());
		settings->setListBouchots(settingsDialog.listBouchots());

		settings->setPinipede(pini);
	}
	settings->startBouchots();
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
	qDebug()<<"MainWindow::closeEvent";
	if(settings->maybeSave())
	{
		settings->saveSettings();
		event->accept();
	} else
		 event->ignore();
}
