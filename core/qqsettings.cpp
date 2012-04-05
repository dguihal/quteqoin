#include "qqsettings.h"

#include "core/qqbouchot.h"
#include "ui/qqpinipede.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>

#define BOUCHOTS_SPLIT_CHAR ';'
#define DEFAULT_MAX_HIST_LEN "500"
#define DEFAULT_DEFAULT_UA "quteqoin 0.01 alpha"
#define DEFAULT_TOTOZ_SERVER_URL "http://totoz.eu/"
#define DEFAULT_TOTOZ_MODE "1"

QQSettings::QQSettings(QObject *parent) :
    QObject(parent)
{
    m_dirty = false;
    readSettings();
}

QQSettings::~QQSettings()
{
}

void QQSettings::setMaxHistoryLength(uint maxHistoryLength)
{
    qDebug() << "setMaxHistoryLength = " << maxHistoryLength;
    if(this->m_maxHistoryLength != maxHistoryLength)
    {
        this->m_maxHistoryLength = maxHistoryLength;
        m_dirty = true;
    }

}

void QQSettings::setDefaultUA(const QString& defaultUA)
{
    qDebug() << "setDefaultUA = " << defaultUA;
    if(this->m_defaultUA != defaultUA)
    {
        m_defaultUA = defaultUA;
        m_dirty = true;
    }
}

void QQSettings::setTotozServerUrl(const QUrl& totozServerUrl)
{
    qDebug() << "setTotozServerUrl = " << totozServerUrl;
    if(this->m_totozServerUrl.toString() != totozServerUrl.toString())
    {
        m_totozServerUrl = totozServerUrl;
        m_dirty = true;
    }
}

void QQSettings::setTotozMode(QQSettings::TotozMode totozMode)
{
    qDebug() << "setTotozMode = " << totozMode;
    if(this->m_totozMode != totozMode)
    {
        m_totozMode = totozMode;
        m_dirty = true;
    }
}

void QQSettings::setDefaultLogin(const QString& defaultLogin)
{
    qDebug() << "setDefaultLogin = " << defaultLogin;
    if(this->m_defaultLogin != defaultLogin)
    {
        m_defaultLogin = defaultLogin;
        m_dirty = true;
    }
}

QQBouchot * QQSettings::bouchot(QString bouchotName)
{
    for(int i = 0; i < m_listBouchots.size(); i++)
        if(m_listBouchots.at(i)->name().compare(bouchotName) == 0)
            return m_listBouchots.at(i);
    return NULL;
}

void QQSettings::addBouchot(QQBouchot *bouchot)
{
    m_listBouchots.append(bouchot);
    m_dirty=true;
}

void QQSettings::addBouchots(const QList<QQBouchot *>& newBouchots)
{
    m_listBouchots << newBouchots;
    m_dirty=true;
}

void QQSettings::removeBouchot(const QString bouchotName)
{
    QQBouchot * bouchot = this->bouchot(bouchotName);

    if( bouchot != NULL)
    {
        m_listBouchots.removeOne(bouchot);
        delete bouchot;
        m_dirty=true;
    }
}

void QQSettings::startBouchots()
{
    for(int i = 0; i < m_listBouchots.size(); i++)
        m_listBouchots[i]->startRefresh();
}

void QQSettings::startBouchot(QString &nomBouchot)
{
    for(int i = 0; i < m_listBouchots.size(); i++)
    {
        if(m_listBouchots[i]->name() == nomBouchot)
        {
            m_listBouchots[i]->startRefresh();
            break;
        }
    }
}

void QQSettings::stopBouchots()
{
    for(int i = 0; i < m_listBouchots.size(); i++)
        m_listBouchots[i]->stopRefresh();
}

void QQSettings::stopBouchot(QString &nomBouchot)
{
    for(int i = 0; i < m_listBouchots.size(); i++)
    {
        if(m_listBouchots[i]->name() == nomBouchot)
        {
            m_listBouchots[i]->stopRefresh();
            break;
        }
    }
}

QList<QString> QQSettings::listTabs()
{
    QList<QString> rep;
    for(int i = 0; i < m_listBouchots.size(); i++)
        if(!rep.contains(m_listBouchots[i]->settings().group()))
            rep.append(m_listBouchots[i]->settings().group());

    qDebug() << "QQSettings::getListTabs : " << rep;
    return rep;
}

bool QQSettings::readSettings()
{
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "Moules Corp", "quteqoin");

    setMaxHistoryLength(settings.value("max_hist_len", QVariant(DEFAULT_MAX_HIST_LEN)).toInt());
    setDefaultUA(settings.value("default_ua", QVariant(DEFAULT_DEFAULT_UA)).toString());
    QUrl totozUrl(settings.value("totoz_server_url", QVariant(DEFAULT_TOTOZ_SERVER_URL)).toString());
    setTotozServerUrl(totozUrl);
    setTotozMode((QQSettings::TotozMode)settings.value("totoz_mode", QVariant(DEFAULT_TOTOZ_MODE)).toInt());

    QString bouchots = settings.value("bouchots", "").toString();
    QStringList bouchotsSplit = bouchots.split(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR), QString::SkipEmptyParts);

    settings.beginGroup(QString::fromAscii("bouchot"));
    for(int i = 0; i < bouchotsSplit.size(); i++)
    {
        QQBouchot *newBouchot = new QQBouchot(bouchotsSplit[i], this);

        QQBouchot::QQBouchotSettings newBouchotSettings = newBouchot->settings();

        settings.beginGroup(newBouchot->name());

        newBouchotSettings.setColorFromString(
                    settings.value(QString::fromAscii("color"), "").toString());
        newBouchotSettings.setAliasesFromString(
                    settings.value(QString::fromAscii("aliases"), "").toString());
        newBouchotSettings.setRefresh(
                    settings.value(QString::fromAscii("refresh"), "").toInt());
        //A modifier pour la gestion des multis
        newBouchotSettings.setLogin(
                    settings.value(QString::fromAscii("login"), "").toString());
        newBouchotSettings.setCookie(
                    settings.value(QString::fromAscii("cookies"), "").toString());
        //
        newBouchotSettings.setUa(
                    settings.value(QString::fromAscii("ua"), "").toString());
        newBouchotSettings.setBackendUrl(
                    settings.value(QString::fromAscii("backendUrl"), "").toString());
        newBouchotSettings.setPostUrl(
                    settings.value(QString::fromAscii("postUrl"), "").toString());
        newBouchotSettings.setPostData(
                    settings.value(QString::fromAscii("postData"), "").toString());
        newBouchotSettings.setGroup(
                    settings.value(QString::fromAscii("group"), "").toString());
        newBouchotSettings.setSlipType(
                    (QQBouchot::TypeSlip) settings.value(QString::fromAscii("slipType"), "").toInt());

        settings.endGroup();

        newBouchot->setSettings(newBouchotSettings);

        m_listBouchots.append(newBouchot);
    }
    settings.endGroup();

    if(! QFile::exists(settings.fileName()))
        m_dirty = true;
    else
        m_dirty = false;
    return true;
}

bool QQSettings::saveSettings()
{
    if(! m_dirty)
        return true;

    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "Moules Corp", "qoinqoin");

    settings.setValue(QString::fromAscii("max_hist_len"),
                      QVariant(m_maxHistoryLength));
    settings.setValue(QString::fromAscii("default_ua"),
                      QVariant(m_defaultUA));
    settings.setValue(QString::fromAscii("totoz_server_url"),
                      QVariant(m_totozServerUrl.toString()));
    settings.setValue(QString::fromAscii("totoz_mode"),
                      QVariant(m_totozMode));

    QStringList bouchotNameList;

    settings.beginGroup(QString::fromAscii("bouchot"));
    for(int i = 0; i < m_listBouchots.size(); i++)
    {
        QQBouchot *bouchot = m_listBouchots[i];

        QQBouchot::QQBouchotSettings bouchotSettings = bouchot->settings();

        settings.beginGroup(bouchot->name());
        settings.setValue(QString::fromAscii("color"),
                          QVariant(bouchotSettings.colorToString()));
        settings.setValue(QString::fromAscii("aliases"),
                          QVariant(bouchotSettings.aliasesToString()));
        settings.setValue(QString::fromAscii("refresh"),
                          QVariant(bouchotSettings.refreshToString()));
        //A modifier pour la gestion des multis
        settings.setValue(QString::fromAscii("login"),
                          QVariant(bouchotSettings.login()));
        settings.setValue(QString::fromAscii("cookies"),
                          QVariant(bouchotSettings.cookie()));
        //
        settings.setValue(QString::fromAscii("ua"),
                          QVariant(bouchotSettings.ua()));
        settings.setValue(QString::fromAscii("backendUrl"),
                          QVariant(bouchotSettings.backendUrl()));
        settings.setValue(QString::fromAscii("postUrl"),
                          QVariant(bouchotSettings.postUrl()));
        settings.setValue(QString::fromAscii("postData"),
                          QVariant(bouchotSettings.postData()));
        settings.setValue(QString::fromAscii("group"),
                          QVariant(bouchotSettings.group()));
        settings.setValue(QString::fromAscii("slipType"),
                          QVariant(bouchotSettings.slipType()));

        settings.endGroup();
        bouchotNameList<<bouchot->name();

    }
    settings.endGroup();
    settings.setValue("bouchots", QVariant(bouchotNameList.join(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR))));
    //qDebug() << settings.fileName();

    m_dirty = false;
    return true;
}

bool QQSettings::maybeSave()
{
    if(m_dirty)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Settings have changed."));
        msgBox.setInformativeText(tr("Save Settings?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Save)
            return saveSettings();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
