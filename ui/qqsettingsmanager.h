#ifndef QQSETTINGSMANAGER_H
#define QQSETTINGSMANAGER_H

#include "core/qqbouchot.h"

#include <QDialog>
#include <QSet>
#include <QString>

class QQBoardsSettings;
class QQGeneralSettings;
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
	void hidePalmi(bool hide = false);

protected slots:
	virtual void accept();
	void configItemChanged();

private:
	void initBoardsSettings();
	void saveBoardsSettings();
	void initGeneralSettings();
	void saveGeneralSettings();
	void initPalmiSettings();
	void savePalmiSettings();
	void initTotozSettings();
	void saveTotozSettings();

	Ui::QQSettingsManager *ui;

	QQBoardsSettings *m_boardsSettingsW;
	QQGeneralSettings *m_generalSettingsW;
	QQPalmiSettings *m_palmiSettingsW;
	QQTotozSettings *m_totozSettingsW;
};

#endif // QQSETTINGSMANAGER_H
