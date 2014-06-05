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
	void setHuntMode(QuteQoin::QQHuntMode huntMode);
	QuteQoin::QQSLHuntMode slHuntMode() { return m_slHuntMode; }
	void setSlHuntMode(QuteQoin::QQSLHuntMode slHuntMode);

	bool silentHuntEnabled() { return m_isSilentHunt; }

	unsigned int maxHuntableItems() { return m_maxHuntableItems; }

public slots:
	void setSilentHunt(bool enabled);
	void setMaxHuntableItems(int maxHuntableItems);

protected slots:
	void huntModeChanged();
	void slHuntModeChanged();

private:
	void setWidgetsVisibility();

	Ui::QQHuntSettings *ui;

	QuteQoin::QQHuntMode m_huntMode;
	QuteQoin::QQSLHuntMode m_slHuntMode;
	bool m_isSilentHunt;
	unsigned int m_maxHuntableItems;
};

#endif // QQHUNTSETTINGS_H
