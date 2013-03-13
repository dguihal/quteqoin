#ifndef QQBOARDSSETTINGS_H
#define QQBOARDSSETTINGS_H

#include "core/qqbouchot.h"

#include <QMap>
#include <QWidget>

namespace Ui {
class QQBoardsSettings;
}

class QQBoardsSettings : public QWidget
{
	Q_OBJECT

public:
	explicit QQBoardsSettings(QWidget *parent = 0);
	~QQBoardsSettings();

	void setBouchots(const QMap<QString, QQBouchot::QQBouchotSettings> currentListBouchot);

	QMap<QString, QQBouchot::QQBouchotSettings> getNewBouchots() { return m_newBouchots; }
	QMap<QString, QQBouchot::QQBouchotSettings> getModifBouchots() { return m_modifBouchots; }
	QStringList getOldBouchots() { return m_oldBouchots; }

protected slots:
	void addBouchot();
	void deleteBouchot();
	void editBouchot();

private:
	Ui::QQBoardsSettings *ui;

	QMap<QString, QQBouchot::QQBouchotSettings> m_bouchots;
	QMap<QString, QQBouchot::QQBouchotSettings> m_newBouchots;
	QMap<QString, QQBouchot::QQBouchotSettings> m_modifBouchots;
	QStringList m_oldBouchots;

	QStringList m_listGroups;
	QStringList m_listNames;
};

#endif // QQBOARDSSETTINGS_H
