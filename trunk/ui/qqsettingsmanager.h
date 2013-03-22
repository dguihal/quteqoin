#ifndef QQSETTINGSMANAGER_H
#define QQSETTINGSMANAGER_H

#include "core/qqbouchot.h"

#include <QDialog>
#include <QSet>
#include <QString>

class QQGeneralSettings;
class QQTotozSettings;
class QQBoardsSettings;

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

protected slots:
	virtual void accept();
	void configItemChanged();

private:
	void initGeneralSettings();
	void saveGeneralSettings();
	void initTotozSettings();
	void saveTotozSettings();
	void initBoardsSettings();
	void saveBoardsSettings();

	Ui::QQSettingsManager *ui;

	QQGeneralSettings * m_generalSettingsW;
	QQTotozSettings * m_totozSettingsW;
	QQBoardsSettings * m_boardsSettingsW;
};

#endif // QQSETTINGSMANAGER_H
