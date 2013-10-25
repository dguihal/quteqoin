#ifndef QQHUNTSETTINGS_H
#define QQHUNTSETTINGS_H

#include "core/qqtypes.h"

#include <QWidget>

namespace Ui {
class QQHuntSettings;
}

class QQHuntSettings : public QWidget
{
	Q_OBJECT

public:
	explicit QQHuntSettings(QWidget *parent = 0);
	~QQHuntSettings();

	QuteQoin::QQHuntMode huntMode() { return m_huntMode; }
	void sethuntMode(QuteQoin::QQHuntMode huntMode);

	bool silentHuntEnabled() { return m_isSilentHunt; }

	unsigned int maxHuntableItems() { return m_maxHuntableItems; }

public slots:
	void setSilentHunt(bool enabled);
	void setMaxHuntableItems(int maxHuntableItems);

protected slots:
	void huntModeChanged();

private:
	Ui::QQHuntSettings *ui;

	QuteQoin::QQHuntMode m_huntMode;
	bool m_isSilentHunt;
	unsigned int m_maxHuntableItems;
};

#endif // QQHUNTSETTINGS_H
