#include "qqboardwizard.h"

#include "core/qqsettings.h"

#include "ui/settingsmanager/qqboardwizardintro.h"
#include "ui/settingsmanager/qqboardwizardnative.h"
#include "ui/settingsmanager/qqboardwizardolccs.h"
#include "ui/settingsmanager/qqboardwizardadv.h"

#include <QtDebug>

QQBoardWizard::QQBoardWizard(QWidget *parent) :
	QWizard(parent)
{
	setDefaultProperty("QComboBox", "currentText", "editTextChanged");

	setWindowTitle(tr("New Board Wizard"));

	setPage(Page_Intro, new QQBoardWizardIntro(this));

	QQBoardWizardOlccs *pOlccs = new QQBoardWizardOlccs(this);
	QStringList lstGrps = QQBouchot::listGroups();
	pOlccs->setListGroups(lstGrps);
	setPage(Page_Olccs_Main, pOlccs);
	setPage(Page_Adv, new QQBoardWizardAdv(this));
	setStartId(Page_Intro);


	qDebug() << Q_FUNC_INFO << pageIds();
}

void QQBoardWizard::accept()
{
	QQSettings settings;

	m_bName = field("Name").toString();

	m_s.setGroup(field("Group").toString());
	m_s.setColorFromString(field("Color").toString());

	//TODO advanced
	QString defaultLogin = settings.value(SETTINGS_GENERAL_DEFAULT_LOGIN, DEFAULT_GENERAL_DEFAULT_LOGIN).toString();
	if(! defaultLogin.isEmpty())
		m_s.setLogin(defaultLogin);
	//s.setUa("");
	//s.setAliasesFromString("");
	m_s.setRefreshFromString(DEFAULT_BOUCHOT_REFRESH);

	m_s.setCookie(field("Cookie").toString());
	m_s.setPostUrl(QString("https://olccs.halifirien.info/t/%1/post").arg(m_bName));
	m_s.setBackendUrl(QString("https://olccs.halifirien.info/t/%1/remote.xml?last=%i").arg(m_bName));
	m_s.setPostData("message=%m");
	m_s.setSlipType(QQBouchot::SlipTagsRaw);

	//Adv.
	m_s.setStrictHttpsCertif(true);

	QWizard::accept();
}
