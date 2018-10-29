#include "qqboardwizard.h"

#include "core/qqsettings.h"

#include "ui/settingsmanager/qqboardwizardnative.h"

#include <QtDebug>

QQBoardWizard::QQBoardWizard(QWidget *parent) :
    QWizard(parent),
    m_showAdvanced(false)
{
	setDefaultProperty("QComboBox", "currentText", "currentIndexChanged");

	setWindowTitle(tr("New Board Wizard"));

	QStringList lstGrps = QQBouchot::listGroups();

	QQBoardWizardNative *pNative = new QQBoardWizardNative(this);
	pNative->setListGroups(lstGrps);
	setPage(Page_Native_Main, pNative);

	setStartId(Page_Native_Main);
}

void QQBoardWizard::accept()
{
	QQSettings settings;

	QString fieldPrefix = "Native";
	m_bName = field(QString("%1_Name").arg(fieldPrefix)).toString();
	m_showAdvanced = field(QString("%1_AdvSettings").arg(fieldPrefix)).toBool();

	m_s = QQBouchot::getBouchotDef(m_bName);
	m_s.setRefresh(DEFAULT_BOUCHOT_REFRESH);
	m_s.setGroup(field(QString("%1_Group").arg(fieldPrefix)).toString());
	m_s.setCookies(field(QString("%1_Cookie").arg(fieldPrefix)).toString());

	//Adv.
	m_s.setStrictHttpsCertif(true);

	QWizard::accept();
}
