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
#define DEFAULT_DEFAULT_UA "qoinqoin 0.01 alpha"
#define DEFAULT_TOTOZ_SERVER_URL "http://totoz.eu/"
#define DEFAULT_TOTOZ_MODE "1"

QQSettings::QQSettings(QObject *parent) :
    QObject(parent)
{
	dirty = false;
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
		dirty = true;
	}

}

uint QQSettings::maxHistoryLength()
{
	return m_maxHistoryLength;
}

void QQSettings::setDefaultUA(const QString& defaultUA)
{
	qDebug() << "setDefaultUA = " << defaultUA;
	if(this->m_defaultUA != defaultUA)
	{
		m_defaultUA = defaultUA;
		dirty = true;
	}
}

QString QQSettings::defaultUA()
{
	return m_defaultUA;
}

void QQSettings::setTotozServerUrl(const QUrl& totozServerUrl)
{
	qDebug() << "setTotozServerUrl = " << totozServerUrl;
	if(this->m_totozServerUrl.toString() != totozServerUrl.toString())
	{
		m_totozServerUrl = totozServerUrl;
		dirty = true;
	}
}

QUrl QQSettings::totozServerUrl()
{
	return m_totozServerUrl;
}

void QQSettings::setTotozMode(QQSettings::TotozMode totozMode)
{
	qDebug() << "setTotozMode = " << totozMode;
	if(this->m_totozMode != totozMode)
	{
		m_totozMode = totozMode;
		dirty = true;
	}
}

QQSettings::TotozMode QQSettings::totozMode()
{
	return m_totozMode;
}

void QQSettings::setDefaultLogin(const QString& defaultLogin)
{
	qDebug() << "setDefaultLogin = " << defaultLogin;
	if(this->m_defaultLogin != defaultLogin)
	{
		m_defaultLogin = defaultLogin;
		dirty = true;
	}
}

QString QQSettings::defaultLogin()
{
	return m_defaultLogin;
}

void QQSettings::setListBouchots(const QList<QQBouchot *>& currentListBouchot)
{
	while(m_listBouchots.length() > 0)
		free(m_listBouchots.takeFirst());

	m_listBouchots << currentListBouchot;

	dirty = true;
}

QList<QQBouchot *> QQSettings::listBouchots()
{
	QList<QQBouchot *> nListBouchots;

	for(int i = 0; i < m_listBouchots.length(); i++)
		nListBouchots.append(new QQBouchot(m_listBouchots[i]));

	return nListBouchots;
}

bool QQSettings::hasBouchot(QQBouchot *bouchot)
{
	return m_listBouchots.contains(bouchot);
}

void QQSettings::addBouchot(QQBouchot *bouchot)
{
	m_listBouchots.append(bouchot);
}

void QQSettings::addBouchots(const QList<QQBouchot *>& newBouchots)
{
	m_listBouchots << newBouchots;
}

void QQSettings::removeBouchots(const QList<QQBouchot *>& oldBouchots)
{
	for(int i = 0; i < oldBouchots.size(); i++)
		m_listBouchots.removeOne(oldBouchots[i]);
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

QList<QString> QQSettings::getListTabs()
{
	QList<QString> rep;
	for(int i = 0; i < m_listBouchots.size(); i++)
		if(!rep.contains(m_listBouchots[i]->group()))
			rep.append(m_listBouchots[i]->group());

	qDebug() << "QQSettings::getListTabs : " << rep;
	return rep;
}

void QQSettings::setPinipede( QQPinipede *pini )
{
	qDebug() << "QQSettings::setPinipede, m_listBouchots.size()=" << m_listBouchots.size();
	for(int i = 0; i < m_listBouchots.size(); i++)
	{
		disconnect(m_listBouchots[i], SIGNAL(newPostsInserted(QQBouchot *)), 0, 0);
		connect(m_listBouchots[i], SIGNAL(newPostsInserted(QQBouchot *)),
				pini, SLOT(newPostsAvailable(QQBouchot *)));
	}
	pini->createPiniTabs( getListTabs() );
}

bool QQSettings::readSettings()
{
	QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "Moules Corp", "qoinqoin");

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

		settings.beginGroup(newBouchot->name());

		newBouchot->setColorFromString(
					settings.value(QString::fromAscii("color"), "").toString());
		newBouchot->setAliasesFromString(
					settings.value(QString::fromAscii("aliases"), "").toString());
		newBouchot->setRefresh(
					settings.value(QString::fromAscii("refresh"), "").toInt());
		//A modifier pour la gestion des multis
		newBouchot->setLogin(
					settings.value(QString::fromAscii("login"), "").toString());
		newBouchot->setCookie(
					settings.value(QString::fromAscii("cookies"), "").toString());
		//
		newBouchot->setUa(
					settings.value(QString::fromAscii("ua"), "").toString());
		newBouchot->setBackendUrl(
					settings.value(QString::fromAscii("backendUrl"), "").toString());
		newBouchot->setPostUrl(
					settings.value(QString::fromAscii("postUrl"), "").toString());
		newBouchot->setPostData(
					settings.value(QString::fromAscii("postData"), "").toString());
		newBouchot->setGroup(
					settings.value(QString::fromAscii("group"), "").toString());
		newBouchot->setSlipType(
					(QQBouchot::TypeSlip) settings.value(QString::fromAscii("slipType"), "").toInt());

		settings.endGroup();

		m_listBouchots.append(newBouchot);
	}
	settings.endGroup();

	if(! QFile::exists(settings.fileName()))
		dirty = true;
	else
		dirty = false;
	return true;
}

bool QQSettings::saveSettings()
{
	if(! dirty)
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

		settings.beginGroup(bouchot->name());
		settings.setValue(QString::fromAscii("color"),
						  QVariant(bouchot->colorToString()));
		settings.setValue(QString::fromAscii("aliases"),
						  QVariant(bouchot->aliasesToString()));
		settings.setValue(QString::fromAscii("refresh"),
						  QVariant(bouchot->refreshToString()));
		//A modifier pour la gestion des multis
		settings.setValue(QString::fromAscii("login"),
						  QVariant(bouchot->login()));
		settings.setValue(QString::fromAscii("cookies"),
						  QVariant(bouchot->cookie()));
		//
		settings.setValue(QString::fromAscii("ua"),
						  QVariant(bouchot->ua()));
		settings.setValue(QString::fromAscii("backendUrl"),
						  QVariant(bouchot->backendUrl()));
		settings.setValue(QString::fromAscii("postUrl"),
						  QVariant(bouchot->postUrl()));
		settings.setValue(QString::fromAscii("postData"),
						  QVariant(bouchot->postData()));
		settings.setValue(QString::fromAscii("group"),
						  QVariant(bouchot->group()));
		settings.setValue(QString::fromAscii("slipType"),
						  QVariant(bouchot->slipType()));

		settings.endGroup();
		bouchotNameList<<bouchot->name();

	}
	settings.endGroup();
	settings.setValue("bouchots", QVariant(bouchotNameList.join(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR))));
	//qDebug() << settings.fileName();

	dirty = false;
	return true;
}

bool QQSettings::maybeSave()
{
	if(dirty)
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
