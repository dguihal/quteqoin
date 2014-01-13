#include "qqsettings.h"

#include "core/qqbouchot.h"
#include "ui/qqpinipede.h"

#include <QtDebug>
#include <QFile>
#include <QList>
#include <QPair>
#include <QMessageBox>

#define DEFAULT_TOTOZ_MODE QQSettings::Bald_Mode

QQSettings::QQSettings(QObject * parent) :
	QSettings(parent)
{
	fillStaticPalmiShortcuts();
	if(! contains(SETTINGS_PALMI_SHORCUTS))
		m_userPalmiShortcuts = defaultUserPalmiShortcuts();
	else
		m_userPalmiShortcuts = savedUserPalmiShortcuts();
}

QQSettings::~QQSettings()
{
}

void QQSettings::removeBouchot(const QString &name)
{
	beginGroup(QString::fromLatin1("bouchot"));
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
	setValue(SETTINGS_LIST_BOUCHOTS, QVariant(listBouchotsNames.join(QChar::fromLatin1(BOUCHOTS_SPLIT_CHAR))));
}

QStringList QQSettings::listTotozSrvPresets()
{
	QDir dir(SETTINGS_TOTOZ_PRESETS_PATH);
	return dir.entryList();
}

QQTotozSrvPreset QQSettings::getTotozSrvPreset(QString totozSrvPreset, bool labelOnly)
{
	QFile file(QString::fromLatin1(SETTINGS_TOTOZ_PRESETS_PATH) + "/" + totozSrvPreset);
	QQTotozSrvPreset preset;

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return preset;

	while (!file.atEnd())
	{
		QString line = QString::fromUtf8(file.readLine()).trimmed();
		int pos = line.indexOf('=');
		QString key = line.left(pos);
		QString value = line.right(line.length() - (pos + 1));

		if(key == "label")
		{
			preset.label = value;
			if(labelOnly)
				break;
		}
		else if(key == "url")
			preset.url = value;
		else if(key == "base_img_dir")
			preset.baseImgDir = value;
		else if(key == "img_suffix")
			preset.imgSuffix = value;
		else if(key == "search_pattern")
			preset.searchPattern = value;
		else if(key == "info_pattern")
			preset.infoPattern = value;
		else
			qWarning() << "Unknow totoz Server preset key :" << key;
	}
	return preset;
}

void QQSettings::saveBouchot(const QString &name, const QQBouchot::QQBouchotSettings &bouchotSettings)
{
	beginGroup(QString::fromLatin1("bouchot"));
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
	setValue(SETTINGS_BOUCHOT_STRICTHTTPSCERTIF, bouchotSettings.isStrictHttpsCertif());

	endGroup();
	endGroup();

	QStringList listBouchotsNames = listBouchots();
	if(! listBouchotsNames.contains(name, Qt::CaseInsensitive))
		listBouchotsNames.append(name);
	setValue(SETTINGS_LIST_BOUCHOTS, QVariant(listBouchotsNames.join(QChar::fromLatin1(BOUCHOTS_SPLIT_CHAR))));
}

QQBouchot * QQSettings::loadBouchot(const QString &name)
{
	QQBouchot::QQBouchotSettings newBouchotSettings;

	beginGroup(QString::fromLatin1("bouchot"));
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
	newBouchotSettings.setStrictHttpsCertif(value(SETTINGS_BOUCHOT_STRICTHTTPSCERTIF, true).toBool());

	endGroup(); //beginGroup(name);
	endGroup(); //beginGroup(QString::fromLatin1("bouchot"));


	QQBouchot *newBouchot = new QQBouchot(name, NULL);
	newBouchot->setSettings(newBouchotSettings);

	return newBouchot;
}

QStringList QQSettings::listBouchots()
{
	QString bouchots = value(SETTINGS_LIST_BOUCHOTS, "").toString();
	return bouchots.split(QChar::fromLatin1(BOUCHOTS_SPLIT_CHAR),
						  QString::SkipEmptyParts);
}

//////////////////////////////////////////////////////////////
/// \brief QQSettings::setValueWithDefault
/// \param key
/// \param newValue
/// \param defaultValue
/// \return true if the stored value changed
///
bool QQSettings::setValueWithDefault(const QString &key, const QVariant &newValue, const QVariant &defaultValue)
{
	QVariant oldValue = value(key, defaultValue);

	if(newValue == defaultValue)
		remove(key);
	else
		setValue(key, newValue);

	return (oldValue != newValue);
}

void QQSettings::setUserPalmiShorcuts(QList< QPair<QChar, QString> > userPalmiShortcuts)
{
	m_userPalmiShortcuts = userPalmiShortcuts;
	if(userPalmiShortcuts != defaultUserPalmiShortcuts())
	{
		QStringList listShortcuts;
		for(int i = 0; i < m_userPalmiShortcuts.size(); i++)
		{
			QPair<QChar, QString> shortcut = m_userPalmiShortcuts.at(i);
			listShortcuts.append(QString(shortcut.first).append(shortcut.second));
		}
		setValue(SETTINGS_PALMI_SHORCUTS, listShortcuts);
	}
	else
		remove(SETTINGS_PALMI_SHORCUTS);
}

void QQSettings::fillStaticPalmiShortcuts()
{
	m_staticPalmiShortcuts.append(qMakePair(QChar('b'), QString("<b>%s</b>")));
	m_staticPalmiShortcuts.append(qMakePair(QChar('i'), QString("<i>%s</i>")));
	m_staticPalmiShortcuts.append(qMakePair(QChar('m'), QString("====> <b>Moment %s</b> <====")));
	m_staticPalmiShortcuts.append(qMakePair(QChar('o'), QString("_o/* <b>BLAM</b>! ")));
	m_staticPalmiShortcuts.append(qMakePair(QChar('p'), QString("_o/* <b>paf!</b> ")));
	m_staticPalmiShortcuts.append(qMakePair(QChar('s'), QString("<s>%s</s>")));
	m_staticPalmiShortcuts.append(qMakePair(QChar('u'), QString("<u>%s</u>")));
}

QList< QPair<QChar, QString> > QQSettings::defaultUserPalmiShortcuts() const
{
	QList< QPair<QChar, QString> > shortcuts;
	shortcuts.append(qMakePair(QChar('j'), QString("\\o/")));
	shortcuts.append(qMakePair(QChar('k'), QString("/o\\")));
	return shortcuts;
}

QList< QPair<QChar, QString> > QQSettings::savedUserPalmiShortcuts() const
{
	QList< QPair<QChar, QString> > res;

	QStringList listShortcuts = value(SETTINGS_PALMI_SHORCUTS, QStringList()).toStringList();

	for(int i = 0; i < listShortcuts.size(); i++)
	{
		QString val = listShortcuts.at(i);
		QChar key = val.at(0);
		QString str = val.remove(0, 1);
		res.append(qMakePair(key, str));
	}

	return res;
}
