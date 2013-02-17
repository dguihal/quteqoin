#ifndef QQSETTINGSDIALOG_H
#define QQSETTINGSDIALOG_H

#include "core/qqbouchot.h"

#include <QDialog>
#include <QList>
#include <QMap>
#include <QStringList>

class QQBouchot;

namespace Ui {
	class QQSettingsDialog;
}

class QQSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit QQSettingsDialog(QWidget *parent = 0);
	~QQSettingsDialog();

	void setMaxHistoryLength(uint);
	uint maxHistoryLength();

	void setDefaultUA(const QString&);
	QString defaultUA();

	void setTotozServerUrl(const QString & totozSrvUrl);
	QString totozServerUrl();

	void setTotozServerBaseImg(const QString & totozBaseImg);
	QString totozServerBaseImg();

	bool totozServerAllowSearch();

	void setTotozQueryPattern(const QString & totozQueryPattern);
	QString totozQueryPattern();

	void setTotozMode(int totozMode);
	int totozMode();

	void setDefaultLogin(const QString&);
	QString defaultLogin();

	void setBouchots(const QMap<QString, QQBouchot::QQBouchotSettings>);
	QMap<QString, QQBouchot::QQBouchotSettings> bouchots() { return m_bouchots; }
	QMap<QString, QQBouchot::QQBouchotSettings> modifiedBouchots() { return m_modifBouchots; }
	QMap<QString, QQBouchot::QQBouchotSettings> newBouchots() { return m_newBouchots; }
	QMap<QString, QQBouchot::QQBouchotSettings> oldBouchots() { return m_oldBouchots; }

public slots:
	void setTotozServerAllowSearch(bool totozServerAllowSearch);
	void addBouchot();
	void deleteBouchot();
	void editBouchot();

private:
	Ui::QQSettingsDialog *ui;
	QMap<QString, QQBouchot::QQBouchotSettings > m_bouchots;
	QMap<QString, QQBouchot::QQBouchotSettings > m_newBouchots;
	QMap<QString, QQBouchot::QQBouchotSettings > m_oldBouchots;
	QMap<QString, QQBouchot::QQBouchotSettings > m_modifBouchots;

	QStringList m_listGroups;
};

#endif // QQSETTINGSDIALOG_H
