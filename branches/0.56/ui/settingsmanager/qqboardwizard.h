#ifndef QQOLCCSWIZARD_H
#define QQOLCCSWIZARD_H

#include "core/qqbouchot.h"

#include <QWizard>

class QQBoardWizard : public QWizard
{
	Q_OBJECT

public:
	explicit QQBoardWizard(QWidget *parent = 0);

	enum PAGE_IDS { Page_Intro, Page_Olccs_Main, Page_Native_Main };

	QQBouchot::QQBouchotSettings bouchotSettings() { return m_s; }
	QString bouchotName() { return m_bName; }

	bool showAdvanced() { return m_showAdvanced; }

public slots:
	virtual void accept();

private:
	QQBouchot::QQBouchotSettings m_s;
	QString m_bName;

	bool m_showAdvanced;
};

#endif // QQOLCCSWIZARD_H
