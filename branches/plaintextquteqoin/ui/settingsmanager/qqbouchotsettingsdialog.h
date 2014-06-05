#ifndef QQBOUCHOTSETTINGSDIALOG_H
#define QQBOUCHOTSETTINGSDIALOG_H

#include "core/qqbouchot.h"

#include <QDialog>
#include <QStringList>

class QQBouchot;

namespace Ui {
	class QQBouchotSettingsDialog;
}

class QQBouchotSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	QQBouchotSettingsDialog(QWidget *parent = 0);
	QQBouchotSettingsDialog(QString bouchotName, QQBouchot::QQBouchotSettings bouchotSettings, QWidget *parent = 0);
	~QQBouchotSettingsDialog();

	QQBouchot::QQBouchotSettings bouchotSettings() { return m_bouchotSettings; }
	QString bouchotName() { return m_bouchotName; }

	void setGroups(QStringList listGroups);
	void setNames(QStringList listNames) { m_listNames = listNames; }

public slots:
	void usePresetPressed();
	void showColorDialog();
	void colorChanged(QString text);
	void nameChanged(QString text);
	virtual void accept();

private:
	void setBouchot();

	Ui::QQBouchotSettingsDialog *ui;
	QString m_bouchotName;
	QQBouchot::QQBouchotSettings m_bouchotSettings;
	QStringList m_listNames;
};

#endif // QQBOUCHOTSETTINGSDIALOG_H
