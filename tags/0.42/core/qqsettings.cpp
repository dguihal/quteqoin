#include "qqsettings.h"

#include "core/qqbouchot.h"
#include "ui/qqpinipede.h"
#include "ui/qqproxyauthdialog.h"

#include <QAuthenticator>
#include <QtDebug>
#include <QFile>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QApplication>

#define BOUCHOTS_SPLIT_CHAR ';'
#define DEFAULT_MAX_HIST_LEN 500
#define DEFAULT_UA "quteqoin/0.42"
#define DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH 1
#define DEFAULT_TOTOZ_MODE QQSettings::Bald_Mode
#define DEFAULT_TOTOZ_QUERY_PATTERN "search.xml?terms=%t&offset=%o"
#define DEFAULT_PALMI_MINI "0"

QQSettings::QQSettings(QObject * parent) :
	QSettings(parent)
{

	m_palmiMini = DEFAULT_PALMI_MINI;
	m_maxHistoryLength = DEFAULT_MAX_HIST_LEN;

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
	setMaxHistoryLength(value("max_hist_len",
							   QVariant(DEFAULT_MAX_HIST_LEN))
						.toInt());

	setDefaultUA(value("default_ua")
				 .toString());

	setPalmiMinimized(value("palmi_minimized",
							 QVariant(DEFAULT_PALMI_MINI))
					  .toBool());

	setTotozMode((QQSettings::TotozMode) value("totoz_mode",
						QVariant(DEFAULT_TOTOZ_MODE))
				 .toInt());

	QString bouchots = value("bouchots", "").toString();
	QStringList bouchotsSplit = bouchots.split(
									QChar::fromAscii(BOUCHOTS_SPLIT_CHAR),
									QString::SkipEmptyParts);

	beginGroup(QString::fromAscii("bouchot"));
	for(int i = 0; i < bouchotsSplit.size(); i++)
	{
		QQBouchot *newBouchot = new QQBouchot(bouchotsSplit[i], this);

		QQBouchot::QQBouchotSettings newBouchotSettings = newBouchot->settings();

		beginGroup(newBouchot->name());

		newBouchotSettings.setColorFromString(
					value(QString::fromAscii("color"), "").toString());
		newBouchotSettings.setAliasesFromString(
					value(QString::fromAscii("aliases"), "").toString());
		newBouchotSettings.setRefresh(
					value(QString::fromAscii("refresh"), "").toInt());
		//A modifier pour la gestion des multis
		newBouchotSettings.setLogin(
					value(QString::fromAscii("login"), "").toString());
		newBouchotSettings.setCookie(
					value(QString::fromAscii("cookies"), "").toString());
		//
		newBouchotSettings.setUa(
					value(QString::fromAscii("ua"), "").toString());
		newBouchotSettings.setBackendUrl(
					value(QString::fromAscii("backendUrl"), "").toString());
		newBouchotSettings.setPostUrl(
					value(QString::fromAscii("postUrl"), "").toString());
		newBouchotSettings.setPostData(
					value(QString::fromAscii("postData"), "").toString());
		newBouchotSettings.setGroup(
					value(QString::fromAscii("group"), "").toString());
		newBouchotSettings.setSlipType(
					(QQBouchot::TypeSlip) value(QString::fromAscii("slipType"), "").toInt());

		endGroup();

		newBouchot->setSettings(newBouchotSettings);

		m_listBouchots.append(newBouchot);
	}
	endGroup();

	return true;
}

bool QQSettings::saveSettings()
{
	setValue(QString::fromAscii("max_hist_len"),
					  QVariant(m_maxHistoryLength));
	setValue(QString::fromAscii("default_ua"),
					  QVariant(m_defaultUA));
	setValue(QString::fromAscii("palmi_minimized"),
					  QVariant(m_palmiMini));
	setValue(QString::fromAscii("totoz_mode"),
					  QVariant(m_totozMode));

	QStringList bouchotNameList;

	beginGroup(QString::fromAscii("bouchot"));
	for(int i = 0; i < m_listBouchots.size(); i++)
	{
		QQBouchot *bouchot = m_listBouchots[i];

		QQBouchot::QQBouchotSettings bouchotSettings = bouchot->settings();

		beginGroup(bouchot->name());
		setValue(QString::fromAscii("color"),
						  QVariant(bouchotSettings.colorToString()));
		setValue(QString::fromAscii("aliases"),
						  QVariant(bouchotSettings.aliasesToString()));
		setValue(QString::fromAscii("refresh"),
						  QVariant(bouchotSettings.refreshToString()));
		//A modifier pour la gestion des multis
		setValue(QString::fromAscii("login"),
						  QVariant(bouchotSettings.login()));
		setValue(QString::fromAscii("cookies"),
						  QVariant(bouchotSettings.cookie()));
		//
		setValue(QString::fromAscii("ua"),
						  QVariant(bouchotSettings.ua()));
		setValue(QString::fromAscii("backendUrl"),
						  QVariant(bouchotSettings.backendUrl()));
		setValue(QString::fromAscii("postUrl"),
						  QVariant(bouchotSettings.postUrl()));
		setValue(QString::fromAscii("postData"),
						  QVariant(bouchotSettings.postData()));
		setValue(QString::fromAscii("group"),
						  QVariant(bouchotSettings.group()));
		setValue(QString::fromAscii("slipType"),
						  QVariant(bouchotSettings.slipType()));

		endGroup();
		bouchotNameList<<bouchot->name();

	}
	endGroup();
	setValue("bouchots", QVariant(bouchotNameList.join(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR))));
	//qDebug() << settings.fileName();

	sync();
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
