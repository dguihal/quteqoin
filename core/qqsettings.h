#ifndef QQSETTINGS_H
#define QQSETTINGS_H

#include "qqsettingsparams.h"
#include "core/qqbouchot.h"

#include <QSettings>
#include <QString>
#include <QStringList>

class QQBouchot;
class QQPinipede;

class QAuthenticator;
class QSettings;
class QString;

struct QQTotozSrvPreset
{
	QString label;
	QString url;
	QString baseImgDir;
	QString imgSuffix;
	QString searchPattern;
	QString infoPattern;
};

class QQSettings : public QSettings
{
	Q_OBJECT

public:
	enum TotozMode { Inline_Mode = 0, Popup_Mode = 1, Bald_Mode = 2 };

	explicit QQSettings(QObject * parent = 0);
	~QQSettings();

	void saveBouchot(const QString &name, const QQBouchot::QQBouchotSettings &bouchotSettings);
	QQBouchot * loadBouchot(const QString &name);
	void removeBouchot(const QString &name);

	QStringList listTotozSrvPresets();
	QQTotozSrvPreset getTotozSrvPreset(QString totozSrvPreset, bool labelOnly = false);

	QStringList listBouchots();

	QList< QPair<QChar, QString> > palmiShorcuts() const { return m_staticPalmiShortcuts + m_userPalmiShortcuts; }
	QList< QPair<QChar, QString> > staticPalmiShorcuts() const { return m_staticPalmiShortcuts; }
	void setUserPalmiShorcuts(QList< QPair<QChar, QString> > userPalmiShortcuts);
	QList< QPair<QChar, QString> > userPalmiShorcuts() const { return m_userPalmiShortcuts; }

private:
	void fillStaticPalmiShortcuts();
	QList< QPair<QChar, QString> > defaultUserPalmiShortcuts() const;
	QList< QPair<QChar, QString> > savedUserPalmiShortcuts() const;

	QList< QPair<QChar, QString> > m_staticPalmiShortcuts;
	QList< QPair<QChar, QString> > m_userPalmiShortcuts;
};

#endif // QQSETTINGS_H
