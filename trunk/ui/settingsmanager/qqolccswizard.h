#ifndef QQOLCCSWIZARD_H
#define QQOLCCSWIZARD_H

#include "core/qqbouchot.h"

#include <QWizard>

class QQOlccsWizard : public QWizard
{
	Q_OBJECT

public:
	explicit QQOlccsWizard(QWidget *parent = 0);

	enum PAGE_IDS { Page_Main, Page_Adv };

	QQBouchot::QQBouchotSettings bouchotSettings() { return m_s; }
	QString bouchotName() { return m_bName; }

public slots:
	virtual void accept();

private:
	QQBouchot::QQBouchotSettings m_s;
	QString m_bName;
};

#endif // QQOLCCSWIZARD_H
