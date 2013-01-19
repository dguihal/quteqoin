#include "qqsettings.h"

#include "core/qqbouchot.h"
#include "ui/qqpinipede.h"
#include "ui/qqproxyauthdialog.h"

#include <QAuthenticator>
#include <QtDebug>
#include <QFile>
#include <QList>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QApplication>

#define BOUCHOTS_SPLIT_CHAR ';'
#define DEFAULT_MAX_HIST_LEN 500
#define DEFAULT_UA "quteqoin crash edition"
#define DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH 1
#define DEFAULT_TOTOZ_MODE QQSettings::Bald_Mode
#define DEFAULT_TOTOZ_QUERY_PATTERN "search.xml?terms=%t&offset=%o"
#define DEFAULT_TOTOZ_SERVER_URL "http://totoz.eu/gif/"
#define DEFAULT_PALMI_MINI "0"

QQSettings::QQSettings(QObject *parent) :
	QObject(parent)
{
	m_palmiMini = DEFAULT_PALMI_MINI;
	m_maxHistoryLength = DEFAULT_MAX_HIST_LEN;

	m_totozQueryPattern = QString(DEFAULT_TOTOZ_QUERY_PATTERN);
	m_totozServerAllowSearch = DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH;
	m_totozServerUrl = QString(DEFAULT_TOTOZ_SERVER_URL);
	m_totozMode = DEFAULT_TOTOZ_MODE;

	readSettings();
}

QQSettings::~QQSettings()
{
}

QString QQSettings::defaultUA()
{
	return m_defaultUA.size() == 0 ?
				QString::fromUtf8(DEFAULT_UA) :
				m_defaultUA;
}

QQBouchot * QQSettings::bouchot(QString bouchotName)
{
	for(int i = 0; i < m_listBouchots.size(); i++)
		if(m_listBouchots.at(i)->name() == bouchotName ||
		   m_listBouchots.at(i)->settings().containsAlias(bouchotName))
			return m_listBouchots.at(i);
	return NULL;
}

QList<QQBouchot *> QQSettings::listBouchots(QString group)
{
	QList<QQBouchot *> rep;

	QListIterator<QQBouchot *> i(m_listBouchots);
	while (i.hasNext())
	{
		QQBouchot * b = i.next();
		if(b->settings().group() == group)
			rep.append(b);
	}
	return rep;
}

void QQSettings::removeBouchot(QQBouchot * bouchot)
{
	if( bouchot != NULL)
	{
		m_listBouchots.removeOne(bouchot);
		delete bouchot;
	}
}

void QQSettings::removeBouchot(const QString bouchotName)
{
	removeBouchot(this->bouchot(bouchotName));
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
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Moules Corp", "quteqoin");

	setMaxHistoryLength(settings
						.value("max_hist_len",
							   QVariant(DEFAULT_MAX_HIST_LEN))
						.toInt());

	setDefaultUA(settings
				 .value("default_ua")
				 .toString());

	setPalmiMinimized(settings
					  .value("palmi_minimized",
							 QVariant(DEFAULT_PALMI_MINI))
					  .toBool());

	QString totozUrl = settings
					   .value("totoz_server_url",
							  QVariant(DEFAULT_TOTOZ_SERVER_URL))
					   .toString();
	setTotozServerUrl(totozUrl);

	setTotozServerAllowSearch(settings
							  .value("totoz_serveur_allow_search",
									 QVariant(DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH))
							  .toBool());

	QString totozQueryPattern = settings
					   .value("totoz_query_pattern",
							  QVariant(DEFAULT_TOTOZ_QUERY_PATTERN))
					   .toString();
	setTotozQueryPattern(totozQueryPattern);

	setTotozMode((QQSettings::TotozMode)settings
				 .value("totoz_mode",
						QVariant(DEFAULT_TOTOZ_MODE))
				 .toInt());

	QString bouchots = settings.value("bouchots", "").toString();
	QStringList bouchotsSplit = bouchots.split(
									QChar::fromAscii(BOUCHOTS_SPLIT_CHAR),
									QString::SkipEmptyParts);

	setWinGeometry(settings
				   .value(QString::fromAscii("win_geometry"),
						  QVariant(QRect()))
				   .toRect());

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

	return true;
}

bool QQSettings::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Moules Corp", "quteqoin");

	settings.setValue(QString::fromAscii("max_hist_len"),
					  QVariant(m_maxHistoryLength));
	settings.setValue(QString::fromAscii("default_ua"),
					  QVariant(m_defaultUA));
	settings.setValue(QString::fromAscii("palmi_minimized"),
					  QVariant(m_palmiMini));
	settings.setValue(QString::fromAscii("totoz_server_url"),
					  QVariant(m_totozServerUrl));
	settings.setValue(QString::fromAscii("totoz_serveur_allow_search"),
					  QVariant(m_totozServerAllowSearch));
	settings.setValue(QString::fromAscii("totoz_query_pattern"),
					  QVariant(m_totozQueryPattern));
	settings.setValue(QString::fromAscii("totoz_mode"),
					  QVariant(m_totozMode));
	settings.setValue(QString::fromAscii("win_geometry"),
					  QVariant(m_wGeometry));

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

	return true;
}

void QQSettings::proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
{
	// Pour éviter le warning
	(void) proxy;

	qDebug() << "QQSettings::proxyAuthenticationRequired";
	//Premier echec
	if(m_proxyUser.size() != 0 &&
	   authenticator->user() != m_proxyUser)
	{
		authenticator->setUser(m_proxyUser);
		authenticator->setPassword(m_proxyPasswd);
	}
	else // Echec ulterieur, ou pas de conf proxy, on redemande
	{
		if(m_proxyPopupMutex.tryLock())
		{
			QQProxyAuthDialog * proxyDialog = new QQProxyAuthDialog();
			proxyDialog->setLogin(m_proxyUser);
			proxyDialog->setPasswd(m_proxyPasswd);
			if(proxyDialog->exec() == QDialog::Accepted)
			{
				m_proxyUser = proxyDialog->login();
				m_proxyPasswd = proxyDialog->passwd();

				authenticator->setUser(m_proxyUser);
				authenticator->setPassword(m_proxyPasswd);
			}
			m_proxyPopupMutex.unlock();
			startBouchots();
		}
		else
		{
			QApplication::processEvents();
		}
	}
}
