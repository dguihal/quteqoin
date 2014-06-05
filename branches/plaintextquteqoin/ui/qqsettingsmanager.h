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
	void minimizePalmi();
	void maximizePalmi();
	void fullRepaint();

protected slots:
	virtual void accept();
	void configItemChanged();

private:
	void initBoardsSettings();
	void saveBoardsSettings();
	void initFilterSettings();
	void saveFilterSettings();
	void initGeneralSettings();
	void saveGeneralSettings();
	void initHuntSettings();
	void saveHuntSettings();
	void initPalmiSettings();
	void savePalmiSettings();
	void initTotozSettings();
	void saveTotozSettings();

	Ui::QQSettingsManager *ui;

	QQBoardsSettings *m_boardsSettingsW;
	QQFilterSettings *m_filterSettingsW;
	QQGeneralSettings *m_generalSettingsW;
	QQHuntSettings *m_huntSettingsW;
	QQPalmiSettings *m_palmiSettingsW;
	QQTotozSettings *m_totozSettingsW;

	bool needPiniFullRepaint;
};

#endif // QQSETTINGSMANAGER_H
