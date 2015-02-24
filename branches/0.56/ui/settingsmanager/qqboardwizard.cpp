#include "qqboardwizard.h"

#include "core/qqsettings.h"

#include "ui/settingsmanager/qqboardwizardintro.h"
#include "ui/settingsmanager/qqboardwizardnative.h"
#include "ui/settingsmanager/qqboardwizardolccs.h"

#include <QtDebug>

QQBoardWizard::QQBoardWizard(QWidget *parent) :
	QWizard(parent),
	m_showAdvanced(false)
{
	setDefaultProperty("QComboBox", "currentText", "currentIndexChanged");

	setWindowTitle(tr("New Board Wizard"));

	setPage(Page_Intro, new QQBoardWizardIntro(this));

	QStringList lstGrps = QQBouchot::listGroups();

	QQBoardWizardNative *pNative = new QQBoardWizardNative(this);
	pNative->setListGroups(lstGrps);
	setPage(Page_Native_Main, pNative);

	QQBoardWizardOlccs *pOlccs = new QQBoardWizardOlccs(this);
	pOlccs->setListGroups(lstGrps);
	setPage(Page_Olccs_Main, pOlccs);

	setStartId(Page_Intro);
}

void QQBoardWizard::accept()
{
	QQSettings settings;
	bool olccs_config;

	QString fieldPrefix = "Native";
	olccs_config = field("Olccs_Mode").toBool();
	if(olccs_config)
	{
		olccs_config = true;
		fieldPrefix = "Olccs";
	}
	m_bName = field(QString("%1_Name").arg(fieldPrefix)).toString();
	m_showAdvanced = field(QString("%1_AdvSettings").arg(fieldPrefix)).toBool();

	if(olccs_config)
	{
		m_s.setColorFromString(field("Olccs_Color").toString());
		m_s.setPostUrl(QString("https://olccs.halifirien.info/t/%1/post").arg(m_bName));
		m_s.setBackendUrl(QString("https://olccs.halifirien.info/t/%1/remote.xml?last=%i").arg(m_bName));
		m_s.setPostData("message=%m");
		m_s.setSlipType(QQBouchot::SlipTagsRaw);
	}
	else
	{
		m_s = QQBouchot::getBouchotDef(m_bName);
	}
	QString defaultLogin = settings.value(SETTINGS_GENERAL_DEFAULT_LOGIN, DEFAULT_GENERAL_DEFAULT_LOGIN).toString();
	if(! defaultLogin.isEmpty())
		m_s.setLogin(defaultLogin);
	m_s.setRefreshFromString(DEFAULT_BOUCHOT_REFRESH);


	m_s.setGroup(field(QString("%1_Group").arg(fieldPrefix)).toString());
	m_s.setCookie(field(QString("%1_Cookie").arg(fieldPrefix)).toString());

	//Adv.
	m_s.setStrictHttpsCertif(true);

	QWizard::accept();
}
