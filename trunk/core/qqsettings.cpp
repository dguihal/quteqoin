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
	beginGroup(QString::fromAscii("bouchot"));
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
	endGroup();

	QStringList listBouchotsNames = listBouchots();
	listBouchotsNames.removeAll(name);
	setValue(SETTINGS_LIST_BOUCHOTS, QVariant(listBouchotsNames.join(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR))));
}

void QQSettings::saveBouchot(const QString &name, const QQBouchot::QQBouchotSettings &bouchotSettings)
{
	beginGroup(QString::fromAscii("bouchot"));
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
	endGroup();

	QStringList listBouchotsNames = listBouchots();
	if(! listBouchotsNames.contains(name, Qt::CaseInsensitive))
		listBouchotsNames.append(name);
	setValue(SETTINGS_LIST_BOUCHOTS, QVariant(listBouchotsNames.join(QChar::fromAscii(BOUCHOTS_SPLIT_CHAR))));
}

QQBouchot * QQSettings::loadBouchot(const QString &name)
{
	QQBouchot::QQBouchotSettings newBouchotSettings;

	beginGroup(QString::fromAscii("bouchot"));
	beginGroup(name);

	// Pas d'url de backend , pas de bouchot
	if(! contains(SETTINGS_BOUCHOT_BACKENDURL))
		return NULL;

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
