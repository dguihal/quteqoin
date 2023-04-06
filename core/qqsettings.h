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

enum QQEmojiType { EMOJI, CAT };
struct QQEmojiDef {
	QString symbol;
	QString name;
	QQEmojiType type;
};

struct QQEmojiCat : QQEmojiDef {
	QList<QQEmojiDef> emojis;
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

	static QStringList listTotozSrvPresets();
	static QQTotozSrvPreset getTotozSrvPreset(const QString& totozSrvPreset, bool labelOnly = false);

	static QList<QQEmojiCat> listEmojis();

	QStringList listBouchots();

	QList< QPair<QChar, QString> > palmiShorcuts() const { return m_staticPalmiShortcuts + m_userPalmiShortcuts; }
	QList< QPair<QChar, QString> > staticPalmiShorcuts() const { return m_staticPalmiShortcuts; }
	void setUserPalmiShorcuts(const QList< QPair<QChar, QString> >& userPalmiShortcuts);
	QList< QPair<QChar, QString> > userPalmiShorcuts() const { return m_userPalmiShortcuts; }

	bool setValueWithDefault(const QString &key, const QVariant &newValue, const QVariant &defaultValue);

private:
	void fillStaticPalmiShortcuts();
	static QList< QPair<QChar, QString> > defaultUserPalmiShortcuts() ;
	QList< QPair<QChar, QString> > savedUserPalmiShortcuts() const;

	QList< QPair<QChar, QString> > m_staticPalmiShortcuts;
	QList< QPair<QChar, QString> > m_userPalmiShortcuts;
};

#endif // QQSETTINGS_H
