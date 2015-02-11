#ifndef QQSETTINGSMANAGER_H
#define QQSETTINGSMANAGER_H

#include "core/qqbouchot.h"

#include <QDialog>
#include <QSet>
#include <QString>

class QQBoardsSettings;
class QQFilterSettings;
class QQGeneralSettings;
class QQHuntSettings;
class QQNetworkSettings;
class QQPalmiSettings;
class QQTotozSettings;

class QListWidgetItem;

namespace Ui {
class QQSettingsManager;
}

class QQSettingsManager : public QDialog
{
	Q_OBJECT

public:
	explicit QQSettingsManager(QWidget *parent = 0);
	~QQSettingsManager();

signals:
	void bouchotCreated(QQBouchot *);
	void totozSearchEnabledChanged(bool);
	void palmiStatusChanged(bool isPalmiMini, bool isPalmiDocked);
	void networkProxySettingsChanged();
	void fullRepaint();

protected slots:
	virtual void accept();
	void configItemChanged();

private:
	void initBoardsSettings(const QQSettings &settings);
	void saveBoardsSettings(QQSettings &settings);
	void initFilterSettings(const QQSettings &settings);
	void saveFilterSettings(QQSettings &settings);
	void initGeneralSettings(const QQSettings &settings);
	void saveGeneralSettings(QQSettings &settings);
	void initHuntSettings(const QQSettings &settings);
	void saveHuntSettings(QQSettings &settings);
	void initNetworkSettings(const QQSettings &settings);
	void saveNetworkSettings(QQSettings &settings);
	void initPalmiSettings(const QQSettings &settings);
	void savePalmiSettings(QQSettings &settings);
	void initTotozSettings(const QQSettings &settings);
	void saveTotozSettings(QQSettings &settings);

	Ui::QQSettingsManager *ui;

	QQBoardsSettings *m_boardsSettingsW;
	QQFilterSettings *m_filterSettingsW;
	QQGeneralSettings *m_generalSettingsW;
	QQHuntSettings *m_huntSettingsW;
	QQNetworkSettings *m_networkSettingsW;
	QQPalmiSettings *m_palmiSettingsW;
	QQTotozSettings *m_totozSettingsW;

	bool needPiniFullRepaint;
};

#endif // QQSETTINGSMANAGER_H
