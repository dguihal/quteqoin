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

    ui->setupUi(this);

    settings = new QQSettings(this);
    palmi = new QQPalmipede(this);
    pini = new QQPinipede(settings, this);

    QLayout *layout = new QVBoxLayout();
    layout->addWidget(pini);
    layout->addWidget(palmi);
    ui->centralWidget->setLayout(layout);

    QList<QQBouchot *> bouchots = settings->listBouchots();
    for(int i = 0; i < bouchots.size(); i++)
    {
        connect(bouchots.at(i), SIGNAL(newPostsInserted(QQBouchot *)), pini, SLOT(newPostsAvailable(QQBouchot *)));
        pini->addPiniTab(bouchots.at(i)->settings().group());
        palmi->addBouchot(bouchots.at(i)->name(), bouchots.at(i)->settings().colorLight());
    }
    connect(pini, SIGNAL(insertTextPalmi(QString)), palmi, SLOT(insertReplaceText(QString)));

    connect(palmi, SIGNAL(postMessage(QString,QString)), this, SLOT(doPostMessage(QString,QString)));
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
    //On arrete le refresh avant de continuer
    settings->stopBouchots();

    QQSettingsDialog settingsDialog(this);
    settingsDialog.setDefaultUA(settings->defaultUA());
    settingsDialog.setDefaultLogin(settings->defaultLogin());
    settingsDialog.setTotozServerUrl(settings->totozServerUrl());
    settingsDialog.setTotozMode(settings->totozMode());
    settingsDialog.setMaxHistoryLength(settings->maxHistoryLength());

    QMap<QString, QQBouchot::QQBouchotSettings> mapBouchotSettings;
    QList<QQBouchot *> listBouchots = settings->listBouchots();
    QQBouchot *bouchot = NULL;
    for(int i = 0; i < listBouchots.size(); i++ )
    {
        bouchot = listBouchots.at(i);
        mapBouchotSettings.insert(bouchot->name(), bouchot->settings() );
    }
    settingsDialog.setBouchots(mapBouchotSettings);

    if(settingsDialog.exec() == QDialog::Accepted)
    {
        settings->setDefaultUA(settingsDialog.defaultUA());
        settings->setDefaultLogin(settingsDialog.defaultLogin());
        settings->setTotozServerUrl(settingsDialog.totozServerUrl());
        settings->setTotozMode((QQSettings::TotozMode) settingsDialog.totozMode());
        settings->setMaxHistoryLength(settingsDialog.maxHistoryLength());

        //Les bouchots supprimes
        QMap<QString, QQBouchot::QQBouchotSettings> settingsDeletedbouchots = settingsDialog.modifiedBouchots();
        QMapIterator<QString, QQBouchot::QQBouchotSettings> iDel(settingsDeletedbouchots);
        while(iDel.hasNext())
        {
            iDel.next();
            settings->removeBouchot(iDel.key());
        }

        //Les bouchots modifies
        QMap<QString, QQBouchot::QQBouchotSettings> settingsModifiedbouchots = settingsDialog.modifiedBouchots();
        QMapIterator<QString, QQBouchot::QQBouchotSettings> iModif(settingsModifiedbouchots);
        while(iModif.hasNext())
        {
            iModif.next();
            QQBouchot * modifBouchot = settings->bouchot(iModif.key());
            modifBouchot->setSettings(iModif.value());
            settings->setDirty();
        }

        //Les bouchots ajoutes
        QMap<QString, QQBouchot::QQBouchotSettings> settingsNewBouchots = settingsDialog.newBouchots();
        QMapIterator<QString, QQBouchot::QQBouchotSettings> iNew(settingsNewBouchots);
        while(iNew.hasNext())
        {
            iNew.next();
            QQBouchot * newBouchot = new QQBouchot(iNew.key(), settings);
            newBouchot->setSettings(iNew.value());

            connect(newBouchot, SIGNAL(newPostsInserted(QQBouchot *)), pini, SLOT(newPostsAvailable(QQBouchot *)));
            pini->addPiniTab(newBouchot->settings().group());
            palmi->addBouchot(newBouchot->name(), newBouchot->settings().colorLight());
            settings->addBouchot(newBouchot);
        }
    }
    settings->startBouchots();
}

void MainWindow::doPostMessage(const QString & bouchot, const QString & message)
{
    QQBouchot * bouchotDest = settings->bouchot(bouchot);

    bouchotDest->postMessage(message);
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
