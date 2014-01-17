#include "qqhuntsettings.h"
#include "ui_qqhuntsettings.h"

QQHuntSettings::QQHuntSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQHuntSettings)
{
	ui->setupUi(this);

	connect(ui->silentModeCB, SIGNAL(toggled(bool)), this, SLOT(setSilentHunt(bool)));
	connect(ui->maxObjSB, SIGNAL(valueChanged(int)), this, SLOT(setMaxHuntableItems(int)));
	connect(ui->HuntAutoRB, SIGNAL(clicked()), this, SLOT(huntModeChanged()));
	connect(ui->huntDisabledRB, SIGNAL(clicked()), this, SLOT(huntModeChanged()));
	connect(ui->huntManualRB, SIGNAL(clicked()), this, SLOT(huntModeChanged()));

	connect(ui->slHuntHiddenRB, SIGNAL(clicked()), this, SLOT(slHuntModeChanged()));
	connect(ui->slHuntSilentRB, SIGNAL(clicked()), this, SLOT(slHuntModeChanged()));
}

QQHuntSettings::~QQHuntSettings()
{
	delete ui;
}

void QQHuntSettings::setHuntMode(QuteQoin::QQHuntMode huntMode)
{
	m_huntMode = huntMode;
	switch (m_huntMode) {
		case QuteQoin::HuntMode_Auto:
			ui->HuntAutoRB->setChecked(true);
			break;
		case QuteQoin::HuntMode_Disabled:
			ui->huntDisabledRB->setChecked(true);
			break;
		default: // QuteQoin::Manual:
			ui->huntManualRB->setChecked(true);
			break;
	}
	setWidgetsVisibility();
}

void QQHuntSettings::setSlHuntMode(QuteQoin::QQSLHuntMode slHuntMode)
{
	m_slHuntMode = slHuntMode;
	switch (m_slHuntMode) {
		case QuteQoin::SLHuntMode_Disabled:
			ui->slHuntHiddenRB->setChecked(true);
			break;
		default: // QuteQoin::SLHuntMode_Silent
			ui->slHuntSilentRB->setChecked(true);
			break;
	}
}

void QQHuntSettings::setMaxHuntableItems(int maxHuntableItems)
{
	m_maxHuntableItems = maxHuntableItems;
	if(ui->maxObjSB->value() != maxHuntableItems)
		ui->maxObjSB->setValue(m_maxHuntableItems);
}

void QQHuntSettings::setSilentHunt(bool enabled)
{
	m_isSilentHunt = enabled;
	if(ui->silentModeCB->isChecked() != m_isSilentHunt)
		ui->silentModeCB->toggle();
}

void QQHuntSettings::huntModeChanged()
{
	if(ui->huntDisabledRB->isChecked())
		m_huntMode = QuteQoin::HuntMode_Disabled;
	else if(ui->HuntAutoRB->isChecked())
		m_huntMode = QuteQoin::HuntMode_Auto;
	else
		m_huntMode = QuteQoin::HuntMode_Manual;

	setWidgetsVisibility();
}

void QQHuntSettings::slHuntModeChanged()
{
	if(ui->slHuntHiddenRB->isChecked())
		m_slHuntMode = QuteQoin::SLHuntMode_Disabled;
	else
		m_slHuntMode = QuteQoin::SLHuntMode_Silent;

}

void QQHuntSettings::setWidgetsVisibility()
{
	switch (m_huntMode) {
		case QuteQoin::HuntMode_Disabled:
			ui->silentModeCB->setDisabled(true);
			ui->slHuntHiddenRB->setDisabled(true);
			ui->slHuntSilentRB->setDisabled(true);
			ui->maxObjSB->setDisabled(true);
			break;
		case QuteQoin::HuntMode_Auto:
			ui->silentModeCB->setDisabled(false);
			ui->slHuntHiddenRB->setDisabled(false);
			ui->slHuntSilentRB->setDisabled(false);
			ui->maxObjSB->setDisabled(false);
			break;
		default: // QuteQoin::HuntMode_Manual:
			ui->silentModeCB->setDisabled(false);
			ui->slHuntHiddenRB->setDisabled(false);
			ui->slHuntSilentRB->setDisabled(false);
			ui->maxObjSB->setDisabled(false);
			break;
	}
}
