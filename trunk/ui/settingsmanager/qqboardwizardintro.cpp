#include "qqboardwizardintro.h"

#include "ui/settingsmanager/qqboardwizard.h"

#include <QBoxLayout>
#include <QSpacerItem>
#include <QRadioButton>

QQBoardWizardIntro::QQBoardWizardIntro(QWidget *parent) :
	QWizardPage(parent)
{
	setTitle(tr("Board access method selection"));
	setSubTitle(tr("With quteqoin you can fetch backend and post messages using either Olccs or directly.<br>Please select the mode you intend to use for this board"));

	QVBoxLayout *layout = new QVBoxLayout;

	layout->addItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Expanding));

	m_modeNativeRB = new QRadioButton("Native (direct) mode", this);
	m_modeNativeRB->setChecked(true);
	layout->addWidget(m_modeNativeRB);

	m_modeOlccsRB = new QRadioButton("Olccs mode", this);
	layout->addWidget(m_modeOlccsRB);

	layout->addItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Expanding));

	setLayout(layout);
}

int	QQBoardWizardIntro::nextId () const
{
	if(m_modeNativeRB->isChecked())
		return QQBoardWizard::Page_Native_Main;
	else
		return QQBoardWizard::Page_Olccs_Main;
}
