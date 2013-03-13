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

class QQSettings : public QSettings
{
	Q_OBJECT

public:
	enum TotozMode { Inline_Mode = 0, Popup_Mode = 1, Bald_Mode = 2 };

	explicit QQSettings(QObject * parent = 0);
	~QQSettings();

	void addBouchot(const QString &name, const QQBouchot::QQBouchotSettings &bouchotSettings);
	QQBouchot * loadBouchot(const QString &name);
	void removeBouchot(const QString &name);

	QStringList listBouchots();
	QStringList listTabs();
};

#endif // QQSETTINGS_H
