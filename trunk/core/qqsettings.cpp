#include "qqsettings.h"

#include "core/qqbouchot.h"
#include "ui/qqpinipede.h"

#include <QtDebug>
#include <QFile>
#include <QList>
#include <QMessageBox>

#define DEFAULT_TOTOZ_MODE QQSettings::Bald_Mode

QQSettings::QQSettings(QObject * parent) :
	QSettings(parent)
{
}

QQSettings::~QQSettings()
{
}

QStringList QQSettings::listBouchots()
{
	QString bouchots = value(SETTINGS_LIST_BOUCHOTS, "").toString();
	return bouchots.split(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR),
						  QString::SkipEmptyParts);
}

void QQSettings::removeBouchot(const QString &name)
{
	beginGroup(name);

	remove(SETTINGS_BOUCHOT_COLOR);
	remove(SETTINGS_BOUCHOT_ALIASES);
	remove(SETTINGS_BOUCHOT_REFRESH);
	remove(SETTINGS_BOUCHOT_LOGIN);
	remove(SETTINGS_BOUCHOT_COOKIES);
	remove(SETTINGS_BOUCHOT_UA);
	remove(SETTINGS_BOUCHOT_BACKENDURL);
	remove(SETTINGS_BOUCHOT_POSTURL);
	remove(SETTINGS_BOUCHOT_POSTDATA);
	remove(SETTINGS_BOUCHOT_GROUP);
	remove(SETTINGS_BOUCHOT_SLIPTYPE);

	endGroup();
}

void QQSettings::addBouchot(const QString &name, const QQBouchot::QQBouchotSettings &bouchotSettings)
{
	beginGroup(name);

	setValue(SETTINGS_BOUCHOT_COLOR, bouchotSettings.colorToString());
	setValue(SETTINGS_BOUCHOT_ALIASES, bouchotSettings.aliasesToString());
	setValue(SETTINGS_BOUCHOT_REFRESH, bouchotSettings.refreshToString());
	setValue(SETTINGS_BOUCHOT_LOGIN, bouchotSettings.login());
	setValue(SETTINGS_BOUCHOT_COOKIES, bouchotSettings.cookie());
	setValue(SETTINGS_BOUCHOT_UA, bouchotSettings.ua());
	setValue(SETTINGS_BOUCHOT_BACKENDURL, bouchotSettings.backendUrl());
	setValue(SETTINGS_BOUCHOT_POSTURL, bouchotSettings.postUrl());
	setValue(SETTINGS_BOUCHOT_POSTDATA, bouchotSettings.postData());
	setValue(SETTINGS_BOUCHOT_GROUP, bouchotSettings.group());
	setValue(SETTINGS_BOUCHOT_SLIPTYPE, bouchotSettings.slipType());

	endGroup();
}

QQBouchot * QQSettings::loadBouchot(const QString &name)
{
	QQBouchot::QQBouchotSettings newBouchotSettings;

	beginGroup(QString::fromAscii("bouchot"));
	beginGroup(name);

	newBouchotSettings.setColorFromString(value(SETTINGS_BOUCHOT_COLOR, "").toString());
	newBouchotSettings.setAliasesFromString(value(SETTINGS_BOUCHOT_ALIASES, "").toString());
	newBouchotSettings.setRefresh(value(SETTINGS_BOUCHOT_REFRESH, "").toInt());
	newBouchotSettings.setLogin(value(SETTINGS_BOUCHOT_LOGIN, "").toString());
	newBouchotSettings.setCookie(value(SETTINGS_BOUCHOT_COOKIES, "").toString());
	newBouchotSettings.setUa(value(SETTINGS_BOUCHOT_UA, "").toString());
	newBouchotSettings.setBackendUrl(value(SETTINGS_BOUCHOT_BACKENDURL, "").toString());
	newBouchotSettings.setPostUrl(value(SETTINGS_BOUCHOT_POSTURL, "").toString());
	newBouchotSettings.setPostData(value(SETTINGS_BOUCHOT_POSTDATA, "").toString());
	newBouchotSettings.setGroup(value(SETTINGS_BOUCHOT_GROUP, "").toString());
	newBouchotSettings.setSlipType((QQBouchot::TypeSlip) value(SETTINGS_BOUCHOT_SLIPTYPE, "").toInt());

	endGroup(); //beginGroup(name);
	endGroup(); //beginGroup(QString::fromAscii("bouchot"));

	QQBouchot *newBouchot = new QQBouchot(name, NULL);
	newBouchot->setSettings(newBouchotSettings);

	return newBouchot;
}
/*

bool QQSettings::readSettings()
{
	setPalmiMinimized(value("palmi_minimized",
							 QVariant(DEFAULT_PALMI_MINI))
					  .toBool());

	QString bouchots = value(SETTINGS_LIST_BOUCHOTS, "").toString();
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
					value(SETTINGS_BOUCHOT_COLOR, "").toString());
		newBouchotSettings.setAliasesFromString(
					value(SETTINGS_BOUCHOT_ALIASES, "").toString());
		newBouchotSettings.setRefresh(
					value(SETTINGS_BOUCHOT_REFRESH, "").toInt());
		//A modifier pour la gestion des multis
		newBouchotSettings.setLogin(
					value(SETTINGS_BOUCHOT_LOGIN, "").toString());
		newBouchotSettings.setCookie(
					value(SETTINGS_BOUCHOT_COOKIES, "").toString());
		//
		newBouchotSettings.setUa(
					value(SETTINGS_BOUCHOT_UA, "").toString());
		newBouchotSettings.setBackendUrl(
					value(SETTINGS_BOUCHOT_BACKENDURL, "").toString());
		newBouchotSettings.setPostUrl(
					value(SETTINGS_BOUCHOT_POSTURL, "").toString());
		newBouchotSettings.setPostData(
					value(SETTINGS_BOUCHOT_POSTDATA, "").toString());
		newBouchotSettings.setGroup(
					value(SETTINGS_BOUCHOT_GROUP, "").toString());
		newBouchotSettings.setSlipType(
					(QQBouchot::TypeSlip) value(SETTINGS_BOUCHOT_SLIPTYPE, "").toInt());

		endGroup();

		newBouchot->setSettings(newBouchotSettings);

		m_listBouchots.append(newBouchot);
	}
	endGroup();

	return true;
}

bool QQSettings::saveSettings()
{	setValue(QString::fromAscii("palmi_minimized"),
					  QVariant(m_palmiMini));

	QStringList bouchotNameList;

	beginGroup(QString::fromAscii("bouchot"));
	for(int i = 0; i < m_listBouchots.size(); i++)
	{
		QQBouchot *bouchot = m_listBouchots[i];

		QQBouchot::QQBouchotSettings bouchotSettings = bouchot->settings();

		beginGroup(bouchot->name());
		setValue(SETTINGS_BOUCHOT_COLOR,
						  QVariant(bouchotSettings.colorToString()));
		setValue(SETTINGS_BOUCHOT_ALIASES,
						  QVariant(bouchotSettings.aliasesToString()));
		setValue(SETTINGS_BOUCHOT_REFRESH,
						  QVariant(bouchotSettings.refreshToString()));
		//A modifier pour la gestion des multis
		setValue(SETTINGS_BOUCHOT_LOGIN,
						  QVariant(bouchotSettings.login()));
		setValue(SETTINGS_BOUCHOT_COOKIES,
						  QVariant(bouchotSettings.cookie()));
		//
		setValue(SETTINGS_BOUCHOT_UA,
						  QVariant(bouchotSettings.ua()));
		setValue(SETTINGS_BOUCHOT_BACKENDURL,
						  QVariant(bouchotSettings.backendUrl()));
		setValue(SETTINGS_BOUCHOT_POSTURL,
						  QVariant(bouchotSettings.postUrl()));
		setValue(SETTINGS_BOUCHOT_POSTDATA,
						  QVariant(bouchotSettings.postData()));
		setValue(SETTINGS_BOUCHOT_GROUP,
						  QVariant(bouchotSettings.group()));
		setValue(SETTINGS_BOUCHOT_SLIPTYPE,
						  QVariant(bouchotSettings.slipType()));

		endGroup();
		bouchotNameList<<bouchot->name();

	}
	endGroup();
	setValue(SETTINGS_LIST_BOUCHOTS, QVariant(bouchotNameList.join(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR))));
	//qDebug() << settings.fileName();

	sync();
	return true;
}
*/
